#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Eraser : public PUPPY::PluginInterface
{
public:
    Eraser() : PUPPY::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface* appInterface) const override;
    virtual PUPPY::Status deinit() const override;
    virtual void dump()const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_release(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

    virtual void show_settings() const override;

private:
    virtual void draw(const PUPPY::Vec2f mousePos) const;
};

const PUPPY::AppInterface* gAppInterface = nullptr;

const Eraser gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Eraser",
    "2.0",
    "loochek",
    "Simple square eraser",

    nullptr,                // icon
    
    PUPPY::PluginType::TOOL
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
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

const PUPPY::PluginInfo *Eraser::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Eraser::init(const PUPPY::AppInterface* appInterface) const
{
    gAppInterface = appInterface;
    appInterface->log("Eraser: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Eraser::deinit() const
{
    return PUPPY::Status::OK;
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

void Eraser::tool_on_press(const PUPPY::Vec2f &position) const
{
    draw(position);
}

void Eraser::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Eraser::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
    draw(to);
}

void Eraser::show_settings() const
{
}

void Eraser::draw(PUPPY::Vec2f mousePos) const
{
    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);

    float currSize = gAppInterface->get_size();

    PUPPY::Vec2f p1(mousePos.x - currSize, mousePos.y - currSize);
    PUPPY::Vec2f p2(mousePos.x + currSize, mousePos.y + currSize);

    PUPPY::RenderTarget *activeLayer = gAppInterface->get_target();
    activeLayer->render_rectangle(p1, p2, PUPPY::RGBA(0), mode);
    delete activeLayer;
}