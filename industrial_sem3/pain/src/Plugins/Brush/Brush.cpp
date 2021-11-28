#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

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

static void draw(PVec2f mousePos);


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

    "Brush",
    "1.0",
    "loochek",
    "Simple circle brush",
    
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
    appInterface->general.log("Brush: succesful initialization!");
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
    draw(mousePos);
}

static void onMouseMove(PVec2f mouseOldPos, PVec2f mouseNewPos)
{
    draw(mouseNewPos);
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

static void draw(PVec2f mousePos)
{
    PRenderMode render_mode = { PPBM_COPY, PPDP_PREVIEW, nullptr };
    gAppInterface->render.circle(mousePos,
                                 gAppInterface->general.get_size(),
                                 gAppInterface->general.get_color(),
                                 &render_mode);
}