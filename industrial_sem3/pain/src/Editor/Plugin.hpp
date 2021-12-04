#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "../Utils/Vec2.hpp"
#include "EditorPluginAPI/plugin_std.hpp"
#include "Canvas.hpp"

class PluginConfigWindow;

/**
 * Handles plugin and provides its interface
 */
class Plugin
{
public:
    ~Plugin();

    const PPluginInfo *getInfo() { return mPluginInterface->general.get_info(); }

    void onUpdate(float elapsedTime) { mPluginInterface->general.on_tick(elapsedTime); }

    void dump() { mPluginInterface->general.dump(); }

    PPreviewLayerPolicy getFlushPolicy() { return mPluginInterface->general.get_flush_policy(); }

    void onMouseClicked(const Vec2i &position)
    {
        if (mPluginInterface->tool.on_press != nullptr)
            mPluginInterface->tool.on_press(toPluginVec(position));
    }

    void onMouseMove(const Vec2i &oldPos, const Vec2i &newPos)
    {
        if (mPluginInterface->tool.on_move != nullptr)
            mPluginInterface->tool.on_move(toPluginVec(oldPos), toPluginVec(newPos));
    }

    void onMouseReleased(const Vec2i &position)
    {
        if (mPluginInterface->tool.on_release != nullptr)
            mPluginInterface->tool.on_release(toPluginVec(position));
    }

    void apply()
    {
        if (mPluginInterface->effect.apply != nullptr)
            mPluginInterface->effect.apply();
    }

    static PVec2f toPluginVec(Vec2f vec) { return PVec2f(vec.x, vec.y); }

    static PRGBA toPluginColor(const LGL::Color &color)
    {
        return PRGBA(color.r * EXTERNAL_RGB_BASE,
                     color.g * EXTERNAL_RGB_BASE,
                     color.b * EXTERNAL_RGB_BASE,
                     color.a * EXTERNAL_RGB_BASE);
    }

    static Vec2f fromPluginVec(PVec2f vec) { return Vec2f(vec.x, vec.y); }

    static LGL::Color fromPluginColor(const PRGBA &color)
    {
        return LGL::Color(color.r / (float)EXTERNAL_RGB_BASE,
                          color.g / (float)EXTERNAL_RGB_BASE,
                          color.b / (float)EXTERNAL_RGB_BASE,
                          color.a / (float)EXTERNAL_RGB_BASE);
    }

private:
    Plugin(const char *fileName, const PAppInterface *appInterface);

public:
    /// Plugin which us initializing now
    static Plugin *sCurrInitPlugin;

private:
    const PPluginInterface *mPluginInterface;
    void *mLibraryHandle;

    PluginConfigWindow *mConfigWindow;

    friend class PluginManager;
};

#endif