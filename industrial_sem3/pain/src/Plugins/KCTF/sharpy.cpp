#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "utils.h"

#include "kctf_widget_library.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

// ============================================================================ Info

const auto PTYPE = PUPPY::TOOL;

const char *PNAME    = "Sharpy";
const char *PVERSION = "1.0";
const char *PAUTHOR  = "KCTF";
const char *PDESCR   = "Spawns random triangles in a circle, blends them on active layer";

// ============================================================================ Resources

void *r_max_size_setting = nullptr;

struct {
    PUPPY::Window *window;
    PUPPY::TextField *field;
    PUPPY::Slider *slider;
    PUPPY::ColorPicker *picker;
    PUPPY::Button *button;
} r_settings;

// ============================================================================ General

#define PEXT
#include "plugin_interface.h"
#undef PEXT

MyPluginInterface PINTERFACE {};

const PUPPY::PluginInfo PINFO =
{
    PUPPY::STD_VERSION, // std_version
    nullptr,     // reserved

    &PINTERFACE,

    PNAME,
    PVERSION,
    PAUTHOR,
    PDESCR,
    nullptr, // icon
    
    PTYPE
};

const PUPPY::AppInterface *APPI = nullptr;


extern "C" const PUPPY::PluginInterface *get_plugin_interface() {
    return &PINTERFACE;
}
 
// ============================================================================ Logic

void generate_triangle(const PUPPY::Vec2f &pos, float radius, PUPPY::Vec2f &p1, PUPPY::Vec2f &p2, PUPPY::Vec2f &p3);

PUPPY::Status MyPluginInterface::init(const PUPPY::AppInterface *app_interface) const {
    srand(time(NULL));

    APPI = app_interface;

    if (APPI->factory.widget) {
        r_settings.window = APPI->factory.widget->window("SHRPY", {{100, 100}, {270, 370}});
        r_settings.field = APPI->factory.widget->text_field({{50, 5}, {100, 30}}, r_settings.window);
        r_settings.slider = APPI->factory.widget->slider(PUPPY::Slider::Type::X, {{20, 40}, {160, 20}}, r_settings.window);
        r_settings.picker = APPI->factory.widget->color_picker({{0, 70}, {200, 200}}, r_settings.window);
        
        r_settings.button = APPI->factory.widget->button({{75, 335}, {50, 30}}, r_settings.window);

        auto bl = APPI->factory.widget->button({{10, 335}, {50, 30}}, r_settings.window);
        auto br = APPI->factory.widget->button({{140, 335}, {50, 30}}, r_settings.window);

        auto size = fmax(1, r_settings.button->get_body().size.y - 5);
        r_settings.button->set_caption("KCTF", size);
        r_settings.button->set_handler([](){APPI->log("praise the %s", PAUTHOR);});

        r_settings.field->set_text("30");
        r_settings.slider->set_fraction(0.30);

        br->set_caption("<<<", size);
        bl->set_caption(">>>", size);
    }

    APPI->ext_register_as("sharpy");
    PINTERFACE.exts.set("generate_triangle", (void*) generate_triangle);

    APPI->log("[plugin](%s) inited", PINFO.name);
    return PUPPY::OK;
}

PUPPY::Status MyPluginInterface::deinit() const {
    if (r_settings.window) {
        r_settings.window->set_to_delete();
    }

    APPI->log("[plugin](%s) deinited | %s thanks you for using it", PINFO.name, PINFO.author);
    return PUPPY::OK;
}

void MyPluginInterface::dump() const {
    APPI->log("[plugin](%s) is active", PINFO.name);
}

const PUPPY::PluginInfo *MyPluginInterface::get_info() const {
    return &PINFO;
}

void MyPluginInterface::on_tick(double /*dt*/) const {
}

void MyPluginInterface::tool_on_press(const PUPPY::Vec2f &pos) const {
    draw(pos);

    auto factory = (SuperWidgetFactory*) APPI->ext_get_interface("KCTFSuperWidget", "SuperWidgetFactory");
    if (!factory) {
        APPI->log("[SuperWidgetFactory] from extension [KCTFSuperWidget] is not available, can't spawn radio button");
        return;
    }

    auto root = APPI->get_root_widget();
    APPI->log("root %p", root);
    auto rb = factory->radio_button({{70, 90}, {140, 100}}, root);
    if (!rb) {
        APPI->log("[SuperWidgetFactory] from extension [KCTFSuperWidget] return nullptr on spawning radio button, why?");
        return;
    }

    rb->set_on(true);
    rb->set_handler([rb](){ if (rb->get_on()) APPI->log("ON"); });
}

void MyPluginInterface::tool_on_move(const PUPPY::Vec2f &/*from*/, const PUPPY::Vec2f &to) const {
    draw(to);
}

void MyPluginInterface::tool_on_release(const PUPPY::Vec2f &/*pos*/) const {}

void MyPluginInterface::show_settings() const {
    r_settings.window->show();
}

void MyPluginInterface::effect_apply() const {}

inline unsigned long long read_next_long_long(const char **buffer) {
    const char *c = *buffer;
    while (c && (*c == ' ' || *c == '\n')) ++c;
 
    unsigned long long l = 0;
    while (*c >= '0' && *c <= '9') {
        l = l * 10 + *c - '0';
        ++c;
    }
 
    *buffer = c;
    return l;
}

unsigned long long read(const char *text) {
    unsigned long long wanted_size = read_next_long_long(&text);
    return wanted_size;
}

typedef void (*generate_triangle_type)(const PUPPY::Vec2f &pos, float radius, PUPPY::Vec2f &p1, PUPPY::Vec2f &p2, PUPPY::Vec2f &p3);

void generate_triangle(const PUPPY::Vec2f &pos, float radius, PUPPY::Vec2f &p1, PUPPY::Vec2f &p2, PUPPY::Vec2f &p3) {
    float a1 = rand();
    float a2 = rand();

    p1 = pos;
 
    p2 = {(float) (pos.x + cos(a1) * radius), (float) (pos.y + sin(a2) * radius)};
    p3 = {(float) (pos.x + cos(a2) * radius), (float) (pos.y + sin(a1) * radius)};
}

void MyPluginInterface::draw(const PUPPY::Vec2f &pos) const {
    float size = APPI->get_size();
    PUPPY::RGBA color = APPI->get_color();

    if (r_settings.window) {
        auto buffer = r_settings.field->get_text().begin();
        auto ms = read_next_long_long(&buffer);
        size = ms * r_settings.slider->get_fraction();
        color = r_settings.picker->get_color();
    }

    PUPPY::Vec2f p0, p1, p2;
    generate_triangle(pos, size, p0, p1, p2);

    auto target = APPI->get_target();
    
    target->render_triangle(p0, p1, p2, color, PUPPY::COPY);
    
    delete target;
}
