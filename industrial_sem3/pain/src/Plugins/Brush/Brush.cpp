#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Brush : public PUPPY::PluginInterface
{
public:
    Brush() : PUPPY::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface*, const std::filesystem::path& path) override;
    virtual PUPPY::Status deinit() override;
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

const Brush gPluginInterface;

PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Brush",
    "2.0",
    "loochek",
    "Simple circle brush",

    nullptr,                // icon
    
    PUPPY::PluginType::TOOL
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
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

const PUPPY::PluginInfo *Brush::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Brush::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;

    std::filesystem::path iconPath(path / "loochek/brush.png");
    gPluginInfo.icon = appInterface->factory.target->from_file(iconPath.c_str());

    appInterface->log("Brush: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Brush::deinit()
{
    delete gPluginInfo.icon;
    return PUPPY::Status::OK;
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

void Brush::tool_on_press(const PUPPY::Vec2f &position) const
{
    draw(position);
}

void Brush::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Brush::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
    draw(to);
}

void Brush::show_settings() const
{
}

void Brush::draw(PUPPY::Vec2f mousePos) const
{
    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);

    PUPPY::RenderTarget *preview = gAppInterface->get_preview();
    preview->render_circle(mousePos, gAppInterface->get_size(), gAppInterface->get_color(), mode);
    delete preview;
}