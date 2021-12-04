#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <vector>
#include "Plugin.hpp"

class PaintController;

/**
 * Singleton which stores plugins and implements PAppInterface functions
 */
class PluginManager
{
public:
    void loadPlugin(const char *fileName);

    void setActiveCanvas(Canvas *canvas);
    void onColorChange(const LGL::Color &color) { mCurrColor = color; };
    void onSizeChange(float size) { mCurrSize = size; };

    Plugin *getPlugin(int idx);
    int getPluginsCount() { return mPlugins.size(); };

    // Assigns app controller to create windows
    void setPaintController(PaintController *controller) { mPaintController = controller; };

    static PluginManager *getInstance();

    //---------------------------------------------//
    // Implementations of PAppInterface functions  //
    //---------------------------------------------//

    void   aiGeneralLog            (const char *fmt, va_list args);
    double aiGeneralGetAbsoluteTime();
    void   aiGeneralReleasePixels  (PRGBA *pixels);
    PRGBA  aiGeneralGetColor       ();
    float  aiGeneralGetSize        ();
    PRGBA *aiTargetGetPixels       ();
    void   aiTargetGetSize         (size_t *width, size_t *height);
    void   aiRenderCircle          (PVec2f position, float radius, PRGBA color, const PRenderMode *render_mode);
    void   aiRenderLine            (PVec2f start, PVec2f end, PRGBA color, const PRenderMode *render_mode);
    void   aiRenderTriangle        (PVec2f p1, PVec2f p2, PVec2f p3, PRGBA color, const PRenderMode *render_mode);
    void   aiRenderRectangle       (PVec2f p1, PVec2f p2, PRGBA color, const PRenderMode *render_mode);
    void   aiRenderPixels          (PVec2f position, const PRGBA *data, size_t width, size_t height,
                                    const PRenderMode *render_mode);
    bool   aiExtensionsEnable      (const char *name);
    void  *aiExtensionsGetFunc     (const char *extension, const char *func);
    void   aiSettingsCreateSurface (const PPluginInterface *self, size_t width, size_t height);
    void   aiSettingsDestroySurface(const PPluginInterface *self);
    void  *aiSettingsAdd           (const PPluginInterface *self, PSettingType type, const char *name);
    void   aiSettingsGet           (const PPluginInterface *self, void *handle, void *answer);

private:
    PluginManager();
    ~PluginManager();

    Plugin             *searchPluginByHandle(const PPluginInterface *interface);
    LGL::RenderTexture *handleRenderMode(const PRenderMode *render_mode);

private:
    std::vector<Plugin*> mPlugins;

    PAppInterface mAppInterface;
    PaintController *mPaintController;

    Vec2i mCurrCanvasSize;

    LGL::Color mCurrColor;
    float mCurrSize;

    LGL::RenderTexture *mCurrMainLayer;
    LGL::RenderTexture *mCurrPreviewLayer;
};

#endif