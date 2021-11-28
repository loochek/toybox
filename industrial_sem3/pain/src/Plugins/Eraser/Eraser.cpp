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

PPluginStatus init(const PAppInterface* appInterface)
{
    gAppInterface = appInterface;
    appInterface->general.log("Eraser: succesful initialization!");
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
    PRenderMode render_mode = { PPBM_COPY, PPDP_ACTIVE };

    float currSize = gAppInterface->general.get_size();

    PVec2f p1(mousePos.x - currSize, mousePos.y - currSize);
    PVec2f p2(mousePos.x + currSize, mousePos.y + currSize);

    gAppInterface->render.rectangle(p1, p2, PRGBA(0, 0, 0, 0), &render_mode);
}