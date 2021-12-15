#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "utils.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

// ============================================================================ Info

const auto PTYPE = PUPPY::TOOL;

const char *PNAME    = "Rainbow Stamp";
const char *PVERSION = "2.0";
const char *PAUTHOR  = "KCTF";
const char *PDESCR   = "Cute and NOT harmful";

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

#include "plugin_interface.h"

const MyPluginInterface PINTERFACE {};

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

PUPPY::Status MyPluginInterface::init(const PUPPY::AppInterface *app_interface) const {
    srand(time(NULL));

    APPI = app_interface;

    if (APPI->factory.widget) {
        r_settings.window = APPI->factory.widget->window("STMP", {{100, 100}, {200, 320}});
        r_settings.field = APPI->factory.widget->text_field({{50, 5}, {100, 30}}, r_settings.window);
        r_settings.slider = APPI->factory.widget->slider(PUPPY::Slider::Type::X, {{20, 40}, {160, 20}}, r_settings.window);
        r_settings.picker = APPI->factory.widget->color_picker({{0, 70}, {200, 200}}, r_settings.window);
        
        r_settings.button = APPI->factory.widget->button({{75, 275}, {50, 30}}, r_settings.window);

        auto bl = APPI->factory.widget->button({{10, 275}, {50, 30}}, r_settings.window);
        auto br = APPI->factory.widget->button({{140, 275}, {50, 30}}, r_settings.window);

        auto size = fmax(1, r_settings.button->get_body().size.y - 5);
        r_settings.button->set_caption("KCTF", size);
        r_settings.button->set_handler([](){APPI->log("praise the %s", PAUTHOR);});

        r_settings.field->set_text("30");
        r_settings.slider->set_fraction(0.30);

        br->set_caption("<<<", size);
        bl->set_caption(">>>", size);
    }

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
}

void MyPluginInterface::tool_on_move(const PUPPY::Vec2f &/*from*/, const PUPPY::Vec2f &to) const {
    draw(to);
}

void MyPluginInterface::tool_on_release(const PUPPY::Vec2f &/*pos*/) const {}

void MyPluginInterface::effect_apply() const {}

void MyPluginInterface::show_settings() const {
    r_settings.window->show();
}

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

PUPPY::RGBA negative(PUPPY::RGBA col) {
    return {(unsigned char) (255 - col.r), 
            (unsigned char) (255 - col.g),
            (unsigned char) (255 - col.b),
            col.a};
}

PUPPY::RGBA shift(PUPPY::RGBA col) {
    return {col.g, col.b, col.r, col.a};
}

void MyPluginInterface::draw(const PUPPY::Vec2f &pos) const {
    auto preview = APPI->get_preview();

    float size = APPI->get_size();
    PUPPY::RGBA color = APPI->get_color();

    if (r_settings.window) {
        auto buffer = r_settings.field->get_text().begin();
        auto ms = read_next_long_long(&buffer);
        size = ms * r_settings.slider->get_fraction();
        color = r_settings.picker->get_color();
    }

    int rnd = rand();

    for (int i = 1; i < 50; ++i) {
        PUPPY::RGBA col = color;
        if ((i + (rnd % 2)) % 2) {
            col = negative(col);
        }
        for (int j = 0; j < i % 7 + (rnd % 10); ++j) {
            col = shift(col);
        }

        float frac = (float) i / 10;
        frac = exp(frac);

        preview->render_circle(pos, size / frac, col, PUPPY::COPY);
    }
    
    delete preview;
}
