#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

PPluginStatus init(const PAppInterface* appInterface);
PPluginStatus deinit();
void dump();
const PPluginInfo *getInfo();
void onUpdate(double elapsedTime);
PPreviewLayerPolicy getFlushPolicy();
void onMousePressed(PVec2f mousePos);
void onMouseMove(PVec2f mouseOldPos, PVec2f mouseNewPos);
void onMouseReleased(PVec2f mousePos);

static void draw(PVec2f mousePos);

const PPluginInterface gPluginInterface =
{
    0, // std_version
    0, // reserved
    
    nullptr, // extensions.enable
    nullptr, // extensions.get_func

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

PPluginStatus init(const PAppInterface* appInterface)
{
    gAppInterface = appInterface;
    appInterface->general.log("Brush: succesful initialization!");
    return PPS_OK; 
}

PPluginStatus deinit()
{
    return PPS_OK;
}

void dump()
{
}

const PPluginInfo *getInfo()
{
    return &gPluginInfo;
}

void onUpdate(double elapsedTime)
{
}

PPreviewLayerPolicy getFlushPolicy()
{
    return PPLP_BLEND;
}

void onMousePressed(PVec2f mousePos)
{
    draw(mousePos);
}

void onMouseMove(PVec2f mouseOldPos, PVec2f mouseNewPos)
{
    draw(mouseNewPos);
}

void onMouseReleased(PVec2f mousePos)
{
}

static void draw(PVec2f mousePos)
{
    PRenderMode render_mode = { PPBM_COPY, PPDP_PREVIEW };
    gAppInterface->render.circle(mousePos,
                                 gAppInterface->general.get_size(),
                                 gAppInterface->general.get_color(),
                                 &render_mode);
}