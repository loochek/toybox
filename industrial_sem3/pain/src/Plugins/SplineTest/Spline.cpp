#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "SplineImpl.hpp"

class Spline : public PUPPY::PluginInterface
{
public:
    Spline() : PUPPY::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface*, const std::filesystem::path& path) override;
    virtual PUPPY::Status deinit() override;
    virtual void dump() const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_release(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

    virtual void show_settings() const override;
};

const PUPPY::AppInterface* gAppInterface = nullptr;

const Spline gPluginInterface;

PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Spline",
    "2.0",
    "loochek",
    "Catmull-Rom spline",

    nullptr,                // icon
    
    PUPPY::PluginType::EXTENSION
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

bool Spline::ext_enable(const char *name) const
{
    return false;
}

void *Spline::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Spline::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *Spline::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Spline::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;

    PUPPY::Widget *root = appInterface->get_root_widget();

    new SplineImpl({{200, 200}, {200, 200}}, appInterface, root);

    appInterface->log("Spline: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Spline::deinit()
{
    return PUPPY::Status::OK;
}

void Spline::dump() const
{
}

void Spline::on_tick(double dt) const
{
}

void Spline::effect_apply() const
{
}

void Spline::tool_on_press(const PUPPY::Vec2f &position) const
{
}

void Spline::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Spline::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void Spline::show_settings() const
{
}