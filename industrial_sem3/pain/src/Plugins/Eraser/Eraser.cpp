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

    "Eraser",
    "1.0",
    "loochek",
    "Simple square eraser",
    
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
    appInterface->general.log("Eraser: succesful initialization!");
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
    PRenderMode render_mode = { PPBM_COPY, PPDP_ACTIVE, nullptr };

    float currSize = gAppInterface->general.get_size();

    PVec2f p1(mousePos.x - currSize, mousePos.y - currSize);
    PVec2f p2(mousePos.x + currSize, mousePos.y + currSize);

    gAppInterface->render.rectangle(p1, p2, PRGBA(0, 0, 0, 0), &render_mode);
}