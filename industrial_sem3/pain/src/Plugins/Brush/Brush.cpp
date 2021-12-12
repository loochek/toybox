#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Brush : public P::PluginInterface
{
public:
    Brush() : P::PluginInterface() {};

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

private:
    virtual void draw(const P::Vec2f mousePos) const;
};

const P::AppInterface* gAppInterface = nullptr;

const Brush gPluginInterface;

const P::PluginInfo gPluginInfo =
{
    PSTD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Brush",
    "2.0",
    "loochek",
    "Simple circle brush",

    nullptr,                // icon
    
    P::PluginType::TOOL
};

extern "C" const P::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

bool Brush::ext_enable(const char *name) const
{
    return false;
}

void *Brush::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Brush::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const P::PluginInfo *Brush::get_info() const
{
    return &gPluginInfo;
}

P::Status Brush::init(const P::AppInterface* appInterface) const
{
    gAppInterface = appInterface;
    appInterface->log("Brush: succesful initialization!");
    return P::Status::OK;
}

P::Status Brush::deinit() const
{
    return P::Status::OK;
}

void Brush::dump() const
{
}

void Brush::on_tick(double dt) const
{
}

void Brush::effect_apply() const
{
}

void Brush::tool_on_press(const P::Vec2f &position) const
{
    draw(position);
}

void Brush::tool_on_release(const P::Vec2f &position) const
{
}

void Brush::tool_on_move(const P::Vec2f &from, const P::Vec2f &to) const
{
    draw(to);
}

void Brush::show_settings() const
{
}

void Brush::draw(P::Vec2f mousePos) const
{
    P::RenderMode mode(P::BlendMode::COPY);

    P::RenderTarget *preview = gAppInterface->get_preview();
    preview->render_circle(mousePos, gAppInterface->get_size(), gAppInterface->get_color(), mode);
    delete preview;
}