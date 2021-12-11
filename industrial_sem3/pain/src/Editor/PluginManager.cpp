#include <cstdarg>
#include <cstring>
#include <cassert>
#include <dlfcn.h>
#include "../Utils/FormattedError.hpp"
#include "../Utils/Logger.hpp"
#include "../EditorLogic/PaintController.hpp"
#include "../EditorWidgets/PluginConfigWindow.hpp"
#include "../BaseGUI/Label.hpp"
#include "PluginManager.hpp"

typedef const P::PluginInterface* (*PIFunc)();

/// Supported version of the standart
const uint32_t PLUGIN_STD_VERSION = 0;


PluginManager::PluginManager() :
    mCurrSize(0.0f), mCurrMainLayer(nullptr), mCurrPreviewLayer(nullptr), mPaintController(nullptr)
{
}

PluginManager::~PluginManager()
{
    for (int i = 0; i < mPlugins.size(); i++)
    {
        const P::PluginInterface *plugin = mPlugins[i];

        P::Status status = plugin->deinit();
        if (status != P::Status::OK)
            Logger::log(LogLevel::Warning, "Unable to deinit plugin %s", plugin->get_info()->name);

        if (dlclose(mLibraryHandles[i]) != 0)
            Logger::log(LogLevel::Warning, "Unable to close plugin %s", dlerror());
    }
}

void PluginManager::loadPlugin(const char *fileName)
{
    void *libraryHandle = dlopen(fileName, RTLD_NOW);
    if (libraryHandle == nullptr)
        throw FormattedError("unable to load plugin %s: %s", fileName, dlerror());

    PIFunc getPluginInterface = (PIFunc)dlsym(libraryHandle, PGET_INTERFACE_FUNC);
    if (getPluginInterface == nullptr)
    {
        FormattedError error("unable to load %s from %s: %s", PGET_INTERFACE_FUNC, fileName, dlerror());
        dlclose(libraryHandle);
        //sCurrInitPlugin = nullptr;
        throw error;
    }

    const P::PluginInterface *pluginInterface = getPluginInterface();

    //sCurrInitPlugin = this;

    P::Status status = pluginInterface->init();
    if (status != P::Status::OK)
    {
        FormattedError error("plugin %s initialization failed", fileName);
        dlclose(libraryHandle);
        //sCurrInitPlugin = nullptr;
        throw error;
    }

    //sCurrInitPlugin = nullptr;
    mPlugins.push_back(pluginInterface);
    mLibraryHandles.push_back(libraryHandle);
}

void PluginManager::setActiveCanvas(Canvas *canvas)
{
    mCurrCanvasSize = canvas->mSize;

    mCurrMainLayer    = canvas->mLayers[canvas->mCurrLayer];
    mCurrPreviewLayer = &canvas->mPreviewLayer;
}

const P::PluginInterface *PluginManager::getPluginInterface(int idx)
{
    assert(idx >= 0 && idx < mPlugins.size());

    return mPlugins[idx];
}

PluginManager *PluginManager::getInstance()
{
    static PluginManager instance;
    return &instance;
}

//---------------------------------------------//
// Implementations of PAppInterface functions  //
//---------------------------------------------//

const float LINE_THICKNESS = 1.0f;


void PluginManager::aiGeneralLog(const char *fmt, va_list args)
{
    Logger::log(LogLevel::Info, fmt, args);
}

double PluginManager::aiGeneralGetAbsoluteTime()
{
    /// TODO:
    return 0;
}

void PluginManager::aiGeneralReleasePixels(PRGBA *pixels)
{
    delete[] pixels;
}

PRGBA PluginManager::aiGeneralGetColor()
{
    return Plugin::toPluginColor(mCurrColor);
}

float PluginManager::aiGeneralGetSize()
{
    return mCurrSize;
}

PRGBA *PluginManager::aiTargetGetPixels()
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mCurrMainLayer);

    LGL::Image image = texture.copyToImage();
    Vec2i imageSize = image.getSize();

    PRGBA *pixels = new PRGBA[imageSize.x * imageSize.y];
    memcpy(pixels, image.getPixels(), sizeof(PRGBA) * imageSize.x * imageSize.y);
    return pixels;
}

void PluginManager::aiTargetGetSize(size_t *width, size_t *height)
{
    *width  = mCurrCanvasSize.x;
    *height = mCurrCanvasSize.y;
}

void PluginManager::aiRenderCircle(PVec2f position, float radius, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawCircle(Plugin::fromPluginVec(position), radius, Plugin::fromPluginColor(color));
}

void PluginManager::aiRenderLine(PVec2f start, PVec2f end, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawLine(Plugin::fromPluginVec(start),
                          Plugin::fromPluginVec(end),
                          LINE_THICKNESS,
                          Plugin::fromPluginColor(color));
}

void PluginManager::aiRenderTriangle(PVec2f p1, PVec2f p2, PVec2f p3, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawTriangle(Plugin::fromPluginVec(p1),
                              Plugin::fromPluginVec(p2),
                              Plugin::fromPluginVec(p3),
                              Plugin::fromPluginColor(color));
}

void PluginManager::aiRenderRectangle(PVec2f p1_ext, PVec2f p2_ext, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);

    Vec2f p1 = Plugin::fromPluginVec(p1_ext);
    Vec2f p2 = Plugin::fromPluginVec(p2_ext);

    whereToDraw->drawRect(FloatRect(p1, p2 - p1), Plugin::fromPluginColor(color));
}

void PluginManager::aiRenderPixels(PVec2f position, const PRGBA *data, size_t width, size_t height,
                                   const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    
    LGL::Image image;
    image.create(width, height, (const uint8_t*)data);

    LGL::Texture texture;
    texture.loadFromImage(image);

    whereToDraw->drawTexture(texture, Plugin::fromPluginVec(position));
}

bool PluginManager::aiExtensionsEnable(const char *name)
{
    return false;
}

void *PluginManager::aiExtensionsGetFunc(const char *extension, const char *func)
{
    return nullptr;
}

void PluginManager::aiSettingsCreateSurface(const PPluginInterface *self, size_t width, size_t height)
{
    Plugin *plugin = searchPluginByHandle(self);
    if (plugin == nullptr)
        return;

    if (mPaintController != nullptr)
    {
        plugin->mConfigWindow = mPaintController->createPluginSettingsWindow(plugin);

        char title[MAX_LABEL_SIZE + 1] = {0};
        snprintf(title, MAX_LABEL_SIZE, "%s's settings", plugin->getInfo()->name);
        plugin->mConfigWindow->setTitle(title);
    }
        
}

void PluginManager::aiSettingsDestroySurface(const PPluginInterface *self)
{
    Plugin *plugin = searchPluginByHandle(self);
    if (plugin == nullptr)
        return;

    if (plugin->mConfigWindow == nullptr)
        return;
        
    plugin->mConfigWindow->scheduleForDeletion();
}

void *PluginManager::aiSettingsAdd(const PPluginInterface *self, PSettingType type, const char *name)
{
    Plugin *plugin = searchPluginByHandle(self);
    if (plugin == nullptr)
        return nullptr;

    if (plugin->mConfigWindow == nullptr)
        return nullptr;

    return plugin->mConfigWindow->addParameter(type, name);
}

void PluginManager::aiSettingsGet(const PPluginInterface *self, void *handle, void *answer)
{
    Plugin *plugin = searchPluginByHandle(self);
    if (plugin == nullptr)
        return;

    if (plugin->mConfigWindow == nullptr)
        return;

    plugin->mConfigWindow->getParameter(handle, answer);
}


Plugin *PluginManager::searchPluginByHandle(const PPluginInterface *interface)
{
    if (Plugin::sCurrInitPlugin != nullptr && Plugin::sCurrInitPlugin->mPluginInterface == interface)
        return Plugin::sCurrInitPlugin;

    for (Plugin *plugin : mPlugins)
    {
        if (plugin->mPluginInterface == interface)
            return plugin;
    }

    return nullptr;
}

LGL::RenderTexture *PluginManager::handleRenderMode(const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = nullptr;
    if (render_mode->draw_policy == PPDP_ACTIVE)
        whereToDraw = mCurrMainLayer;
    else
        whereToDraw = mCurrPreviewLayer;

    if (render_mode->blend == PPBM_ALPHA_BLEND)
        whereToDraw->setBlendMode(true);
    else
        whereToDraw->setBlendMode(false);

    return whereToDraw;
}


//----------------------------------------------//
// C wrappers to PAppInterface implementations  //
//----------------------------------------------//

static void ai_general_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    PluginManager::getInstance()->aiGeneralLog(fmt, args);
    va_end(args);
}

static double ai_general_get_absolute_time()
{
    return PluginManager::getInstance()->aiGeneralGetAbsoluteTime();
}

static void ai_general_release_pixels(PRGBA *pixels)
{
    PluginManager::getInstance()->aiGeneralReleasePixels(pixels);
}

static PRGBA ai_general_get_color()
{
    return PluginManager::getInstance()->aiGeneralGetColor();
}

static float ai_general_get_size()
{
    return PluginManager::getInstance()->aiGeneralGetSize();
}

static PRGBA *ai_target_get_pixels()
{
    return PluginManager::getInstance()->aiTargetGetPixels();
}

static void ai_target_get_size(size_t *width, size_t *height)
{
    PluginManager::getInstance()->aiTargetGetSize(width, height);
}

static void ai_render_circle(PVec2f position, float radius, PRGBA color, const PRenderMode *render_mode)
{
    PluginManager::getInstance()->aiRenderCircle(position, radius, color, render_mode);
}

static void ai_render_line(PVec2f start, PVec2f end, PRGBA color, const PRenderMode *render_mode)
{
    PluginManager::getInstance()->aiRenderLine(start, end, color, render_mode);
}

static void ai_render_triangle(PVec2f p1, PVec2f p2, PVec2f p3, PRGBA color, const PRenderMode *render_mode)
{
    PluginManager::getInstance()->aiRenderTriangle(p1, p2, p3, color, render_mode);
}

static void ai_render_rectangle(PVec2f p1, PVec2f p2, PRGBA color, const PRenderMode *render_mode)
{
    PluginManager::getInstance()->aiRenderRectangle(p1, p2, color, render_mode);
}

static void ai_render_pixels(PVec2f position, const PRGBA *data, size_t width, size_t height,
                             const PRenderMode *render_mode)
{
    PluginManager::getInstance()->aiRenderPixels(position, data, width, height, render_mode);
}

static bool ai_extensions_enable(const char *name)
{
    return PluginManager::getInstance()->aiExtensionsEnable(name);
}

static void *ai_extensions_get_func(const char *extension, const char *func)
{
    return PluginManager::getInstance()->aiExtensionsGetFunc(extension, func);
}

static void ai_settings_create_surface(const PPluginInterface *self, size_t width, size_t height)
{
    PluginManager::getInstance()->aiSettingsCreateSurface(self, width, height);
}

static void ai_settings_destroy_surface(const PPluginInterface *self)
{
    PluginManager::getInstance()->aiSettingsDestroySurface(self);
}

static void *ai_settings_add(const PPluginInterface *self, PSettingType type, const char *name)
{
    return PluginManager::getInstance()->aiSettingsAdd(self, type, name);
}

static void ai_settings_get(const PPluginInterface *self, void *handle, void *answer)
{
    PluginManager::getInstance()->aiSettingsGet(self, handle, answer);
}