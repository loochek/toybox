#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Negative : public P::PluginInterface
{
public:
    Negative() : P::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const P::PluginInfo *get_info() const override;
    virtual P::Status init(const P::AppInterface* appInterface) const override;
    virtual P::Status deinit() const override;
    virtual void dump()const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const P::Vec2f &position) const override;
    virtual void tool_on_release(const P::Vec2f &position) const override;
    virtual void tool_on_move(const P::Vec2f &from, const P::Vec2f &to) const override;

    virtual void show_settings() const override;
};

const P::AppInterface* gAppInterface = nullptr;

const Negative gPluginInterface;

const P::PluginInfo gPluginInfo =
{
    PSTD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Negative",
    "2.0",
    "loochek",
    "Simple negative effect",

    nullptr,                // icon
    
    P::PluginType::EFFECT
};

extern "C" const P::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

bool Negative::ext_enable(const char *name) const
{
    return false;
}

void *Negative::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Negative::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const P::PluginInfo *Negative::get_info() const
{
    return &gPluginInfo;
}

P::Status Negative::init(const P::AppInterface* appInterface) const
{
    gAppInterface = appInterface;
    appInterface->log("Negative: succesful initialization!");
    return P::Status::OK;
}

P::Status Negative::deinit() const
{
    return P::Status::OK;
}

void Negative::dump() const
{
}

void Negative::on_tick(double dt) const
{
}

void Negative::effect_apply() const
{
    P::RenderTarget *activeLayer = gAppInterface->get_target();
    P::Vec2s layerSize(activeLayer->get_size());

    P::RGBA *pixels = activeLayer->get_pixels();

    for (int y = 0; y < layerSize.y; y++)
    {
        for (int x = 0; x < layerSize.x; x++)
        {
            P::RGBA color = pixels[layerSize.x * y + x];
            color.r = 255 - color.r;
            color.g = 255 - color.g;
            color.b = 255 - color.b;

            pixels[layerSize.x * y + x] = color;
        }
    }

    P::RenderMode mode(P::BlendMode::COPY);
    activeLayer->render_pixels(P::Vec2f(0, 0), layerSize, pixels, mode);
    
    delete[] pixels;
    delete activeLayer;
}

void Negative::tool_on_press(const P::Vec2f &position) const
{
}

void Negative::tool_on_release(const P::Vec2f &position) const
{
}

void Negative::tool_on_move(const P::Vec2f &from, const P::Vec2f &to) const
{
}

void Negative::show_settings() const
{
}