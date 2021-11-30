#include <cstdarg>
#include <cstring>
#include <cassert>
#include "../Utils/Logger.hpp"
#include "PluginManager.hpp"

/// Supported version of the standart
const uint32_t PLUGIN_STD_VERSION = 0;

static void ai_general_log(const char *fmt, ...);
static double ai_general_get_absolute_time();
static void ai_general_release_pixels(PRGBA *pixels);
static PRGBA ai_general_get_color();
static float ai_general_get_size();
static PRGBA *ai_target_get_pixels();
static void ai_target_get_size(size_t *width, size_t *height);
static void ai_render_circle(PVec2f position, float radius, PRGBA color, const PRenderMode *render_mode);
static void ai_render_line(PVec2f start, PVec2f end, PRGBA color, const PRenderMode *render_mode);
static void ai_render_triangle(PVec2f p1, PVec2f p2, PVec2f p3, PRGBA color, const PRenderMode *render_mode);
static void ai_render_rectangle(PVec2f p1_ext, PVec2f p2_ext, PRGBA color, const PRenderMode *render_mode);
static void ai_render_pixels(PVec2f position, const PRGBA *data, size_t width, size_t height, const PRenderMode *render_mode);
static bool ai_extensions_enable(const char *name);
static void *ai_extensions_get_func(const char *name);


PluginManager::PluginManager() : mCurrSize(0.0f), mCurrMainLayer(nullptr), mCurrPreviewLayer(nullptr)
{
    mAppInterface.std_version                  = PLUGIN_STD_VERSION;
    mAppInterface.general.feature_level        = (PFeatureLevel)0;
    
    mAppInterface.general.log                  = ai_general_log;
    mAppInterface.general.get_absolute_time    = ai_general_get_absolute_time;
    mAppInterface.general.release_pixels       = ai_general_release_pixels;
    mAppInterface.general.get_color            = ai_general_get_color;
    mAppInterface.general.get_size             = ai_general_get_size;

    mAppInterface.target.get_pixels            = ai_target_get_pixels;
    mAppInterface.target.get_size              = ai_target_get_size;

    mAppInterface.render.circle                = ai_render_circle;
    mAppInterface.render.line                  = ai_render_line;
    mAppInterface.render.triangle              = ai_render_triangle;
    mAppInterface.render.rectangle             = ai_render_rectangle;
    mAppInterface.render.pixels                = ai_render_pixels;

    mAppInterface.extensions.enable            = ai_extensions_enable;
    mAppInterface.extensions.get_func          = ai_extensions_get_func;

    mAppInterface.shader.apply                 = nullptr;
    mAppInterface.shader.compile               = nullptr;
    mAppInterface.shader.release               = nullptr;
    mAppInterface.shader.set_uniform_float     = nullptr;
    mAppInterface.shader.set_uniform_float_arr = nullptr;
    mAppInterface.shader.set_uniform_int       = nullptr;
    mAppInterface.shader.set_uniform_int_arr   = nullptr;

    mAppInterface.settings.add                 = nullptr;
    mAppInterface.settings.get                 = nullptr;
    mAppInterface.settings.create_surface      = nullptr;
    mAppInterface.settings.destroy_surface     = nullptr;
}

PluginManager::~PluginManager()
{
    for (Plugin *plugin : mPlugins)
        delete plugin;
}

void PluginManager::loadPlugin(const char *fileName)
{
    mPlugins.push_back(new Plugin(fileName, &mAppInterface));
}

void PluginManager::setActiveCanvas(Canvas *canvas)
{
    mCurrCanvasSize = canvas->mSize;

    mCurrMainLayer    = canvas->mLayers[canvas->mCurrLayer];
    mCurrPreviewLayer = &canvas->mPreviewLayer;
}

Plugin *PluginManager::getPlugin(int idx)
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

/// Helper function
static LGL::RenderTexture *handleRenderMode(const PRenderMode *render_mode)
{
    PluginManager *mgr = PluginManager::getInstance();

    LGL::RenderTexture *whereToDraw = nullptr;
    if (render_mode->draw_policy == PPDP_ACTIVE)
        whereToDraw = mgr->mCurrMainLayer;
    else
        whereToDraw = mgr->mCurrPreviewLayer;

    if (render_mode->blend == PPBM_ALPHA_BLEND)
        whereToDraw->setBlendMode(true);
    else
        whereToDraw->setBlendMode(false);

    return whereToDraw;
}

static void ai_general_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Logger::log(LogLevel::Info, fmt, args);
    va_end(args);
}

static double ai_general_get_absolute_time()
{
    /// TODO:
    return 0;
}

static void ai_general_release_pixels(PRGBA *pixels)
{
    delete[] pixels;
}

static PRGBA ai_general_get_color()
{
    return Plugin::toPluginColor(PluginManager::getInstance()->mCurrColor);
}

static float ai_general_get_size()
{
    return PluginManager::getInstance()->mCurrSize;
}

static PRGBA *ai_target_get_pixels()
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*PluginManager::getInstance()->mCurrMainLayer);

    LGL::Image image = texture.copyToImage();
    Vec2i imageSize = image.getSize();

    PRGBA *pixels = new PRGBA[imageSize.x * imageSize.y];
    memcpy(pixels, image.getPixels(), sizeof(PRGBA) * imageSize.x * imageSize.y);
    return pixels;
}

static void ai_target_get_size(size_t *width, size_t *height)
{
    PluginManager *mgr = PluginManager::getInstance();

    *width  = mgr->mCurrCanvasSize.x;
    *height = mgr->mCurrCanvasSize.y;
}

static void ai_render_circle(PVec2f position, float radius, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawCircle(Plugin::fromPluginVec(position), radius, Plugin::fromPluginColor(color));
}

static void ai_render_line(PVec2f start, PVec2f end, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawLine(Plugin::fromPluginVec(start),
                          Plugin::fromPluginVec(end),
                          LINE_THICKNESS,
                          Plugin::fromPluginColor(color));
}

static void ai_render_triangle(PVec2f p1, PVec2f p2, PVec2f p3, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    whereToDraw->drawTriangle(Plugin::fromPluginVec(p1),
                              Plugin::fromPluginVec(p2),
                              Plugin::fromPluginVec(p3),
                              Plugin::fromPluginColor(color));
}

static void ai_render_rectangle(PVec2f p1_ext, PVec2f p2_ext, PRGBA color, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);

    Vec2f p1 = Plugin::fromPluginVec(p1_ext);
    Vec2f p2 = Plugin::fromPluginVec(p2_ext);

    whereToDraw->drawRect(FloatRect(p1, p2 - p1), Plugin::fromPluginColor(color));
}

static void ai_render_pixels(PVec2f position, const PRGBA *data, size_t width, size_t height, const PRenderMode *render_mode)
{
    LGL::RenderTexture *whereToDraw = handleRenderMode(render_mode);
    
    LGL::Image image;
    image.create(width, height, (const uint8_t*)data);

    LGL::Texture texture;
    texture.loadFromImage(image);

    whereToDraw->drawTexture(texture, Plugin::fromPluginVec(position));
}

static bool ai_extensions_enable(const char *name)
{
    return false;
}

static void *ai_extensions_get_func(const char *name)
{
    return nullptr;
}