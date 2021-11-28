#include <deque>
#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "../../Utils/Vec2.hpp"
#include "../../LGL/Color.hpp"

const Vec2i DIRS[] = { Vec2i(-1, 0), Vec2i(1, 0), Vec2i(0, -1), Vec2i(0, 1) };

static PPluginStatus init(const PAppInterface* appInterface);
static PPluginStatus deinit();

static void dump();
static void onUpdate(double elapsedTime);

static const PPluginInfo  *getInfo();
static PPreviewLayerPolicy getFlushPolicy();

static void onMousePressed(PVec2f mousePos);
static void onMouseMove(PVec2f mouseOldPos, PVec2f mouseNewPos);
static void onMouseReleased(PVec2f mousePos);

static bool enableExtension(const char *name);
static void *getExtensionFunc(const char *name);

static bool areColorsEqual(const PRGBA &first, const PRGBA &second);


const PPluginInterface gPluginInterface =
{
    0, // std_version
    0, // reserved
    
    enableExtension,
    getExtensionFunc,

    // general
    getInfo,
    init,
    deinit,
    dump,
    onUpdate,
    nullptr,
    getFlushPolicy,

    // effect
    nullptr,

    // tool
    onMousePressed,
    onMouseReleased,
    onMouseMove
};

const PPluginInfo gPluginInfo =
{
    0, // std_version
    0, // reserved

    &gPluginInterface,

    "Fill",
    "1.0",
    "loochek",
    "Simple Paint-like fill",
    
    PPT_TOOL
};


const PAppInterface *gAppInterface = nullptr;


extern "C" const PPluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

static PPluginStatus init(const PAppInterface* appInterface)
{
    gAppInterface = appInterface;
    appInterface->general.log("Fill: succesful initialization!");
    return PPS_OK; 
}

static PPluginStatus deinit()
{
    return PPS_OK;
}

static void dump()
{
}

static const PPluginInfo *getInfo()
{
    return &gPluginInfo;
}

static void onUpdate(double elapsedTime)
{
}

static PPreviewLayerPolicy getFlushPolicy()
{
    return PPLP_BLEND;
}

static void onMousePressed(PVec2f mousePos)
{
    PRenderMode render_mode = { PPBM_COPY, PPDP_ACTIVE, nullptr };

    Vec2i clickPos(mousePos.x, mousePos.y);

    size_t layerWidth = 0, layerHeight = 0;
    gAppInterface->target.get_size(&layerWidth, &layerHeight);

    if (clickPos.x < 0 || clickPos.x >= layerWidth || clickPos.y < 0 || clickPos.y >= layerHeight)
        return;

    PRGBA fillColor = gAppInterface->general.get_color();

    PRGBA *pixels = gAppInterface->target.get_pixels();

    std::deque<Vec2i> fillQueue;
    fillQueue.push_back(clickPos);

    while (!fillQueue.empty())
    {
        Vec2i currPixel = fillQueue.front();
        fillQueue.pop_front();

        if (areColorsEqual(pixels[layerWidth * currPixel.y + currPixel.x], fillColor))
            continue;

        for (Vec2i dir : DIRS)
        {
            if (currPixel.x + dir.x >= 0 && currPixel.x + dir.x < layerWidth &&
                currPixel.y + dir.y >= 0 && currPixel.y + dir.y < layerHeight)
            {
                Vec2i sidePixel = currPixel + dir;
                if (areColorsEqual(pixels[layerWidth * currPixel.y + currPixel.x],
                                   pixels[layerWidth * sidePixel.y + sidePixel.x]))
                {
                    fillQueue.push_back(sidePixel);
                }
            }
        }

        pixels[layerWidth * currPixel.y + currPixel.x] = fillColor;
    }

    gAppInterface->render.pixels(PVec2f(0, 0), pixels, layerWidth, layerHeight, &render_mode);
    gAppInterface->general.release_pixels(pixels);
}

static void onMouseMove(PVec2f mouseOldPos, PVec2f mouseNewPos)
{
}

static void onMouseReleased(PVec2f mousePos)
{
}

static bool enableExtension(const char *name)
{
    return false;
}

static void *getExtensionFunc(const char *name)
{
    return nullptr;
}

static bool areColorsEqual(const PRGBA &first, const PRGBA &second)
{
    return first.r == second.r && first.g == second.g && first.b == second.b && first.a == second.a;
}