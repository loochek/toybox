#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Brush : public PUPPY::PluginInterface
{
public:
    Brush() : PUPPY::PluginInterface() {};

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

const Brush gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
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

struct {
    PUPPY::Window *window;
    PUPPY::TextField *field;
    PUPPY::Slider *slider;
    PUPPY::ColorPicker *picker;
    PUPPY::Button *button;
} r_settings;

PUPPY::Status Brush::init(const PUPPY::AppInterface* appInterface) const
{
    gAppInterface = appInterface;

    r_settings.window = appInterface->factory.widget->window("SHRPY", {{100, 100}, {200, 320}});
    r_settings.field = appInterface->factory.widget->text_field({{50, 5}, {100, 30}}, r_settings.window);
    r_settings.slider = appInterface->factory.widget->slider(PUPPY::Slider::Type::X, {{20, 40}, {160, 20}}, r_settings.window);
    r_settings.picker = appInterface->factory.widget->color_picker({{0, 70}, {200, 200}}, r_settings.window);
    
    r_settings.button = appInterface->factory.widget->button({{75, 275}, {50, 30}}, r_settings.window);

    auto bl = appInterface->factory.widget->button({{10, 275}, {50, 30}}, r_settings.window);
    auto br = appInterface->factory.widget->button({{140, 275}, {50, 30}}, r_settings.window);

    auto size = fmax(1, r_settings.button->get_body().size.y - 5);
    r_settings.button->set_caption("KCTF", size);
    r_settings.button->set_handler([](){gAppInterface->log("praise the ABOBA");});

    r_settings.field->set_text("30");
    r_settings.slider->set_fraction(0.30);

    br->set_caption("<<<", size);
    bl->set_caption(">>>", size);

    // PUPPY::Window *window = appInterface->factory.widget->window("ABOBA", { { 200, 200 }, { 200, 200 } }, nullptr);
    // window->set_name("ABOBA");

    // PUPPY::Button *button = appInterface->factory.widget->button({{ 40, 40 }, { 100, 100 }}, window);
    // button->set_handler([appInterface]()
    // {
    //     appInterface->log("ABOBA pressed");
    // });
    // window->add_child(button);

    // window->add_child(appInterface->factory.widget->slider(PUPPY::Slider::Type::X, {{ 40, 150 }, { 100, 100}}, window));

    appInterface->log("Brush: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Brush::deinit() const
{
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