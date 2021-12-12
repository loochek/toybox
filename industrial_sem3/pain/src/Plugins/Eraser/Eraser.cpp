#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Eraser : public P::PluginInterface
{
public:
    Eraser() : P::PluginInterface() {};

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

const Eraser gPluginInterface;

const P::PluginInfo gPluginInfo =
{
    PSTD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Eraser",
    "2.0",
    "loochek",
    "Simple square eraser",

    nullptr,                // icon
    
    P::PluginType::TOOL
};

extern "C" const P::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

bool Eraser::ext_enable(const char *name) const
{
    return false;
}

void *Eraser::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Eraser::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const P::PluginInfo *Eraser::get_info() const
{
    return &gPluginInfo;
}

P::Status Eraser::init(const P::AppInterface* appInterface) const
{
    gAppInterface = appInterface;
    appInterface->log("Eraser: succesful initialization!");
    return P::Status::OK;
}

P::Status Eraser::deinit() const
{
    return P::Status::OK;
}

void Eraser::dump() const
{
}

void Eraser::on_tick(double dt) const
{
}

void Eraser::effect_apply() const
{
}

void Eraser::tool_on_press(const P::Vec2f &position) const
{
    draw(position);
}

void Eraser::tool_on_release(const P::Vec2f &position) const
{
}

void Eraser::tool_on_move(const P::Vec2f &from, const P::Vec2f &to) const
{
    draw(to);
}

void Eraser::show_settings() const
{
}

void Eraser::draw(P::Vec2f mousePos) const
{
    P::RenderMode mode(P::BlendMode::COPY);

    float currSize = gAppInterface->get_size();

    P::Vec2f p1(mousePos.x - currSize, mousePos.y - currSize);
    P::Vec2f p2(mousePos.x + currSize, mousePos.y + currSize);

    P::RenderTarget *activeLayer = gAppInterface->get_target();
    activeLayer->render_rectangle(p1, p2, P::RGBA(0), mode);
    delete activeLayer;
}