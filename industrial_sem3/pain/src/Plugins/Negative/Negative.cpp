#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Negative : public PUPPY::PluginInterface
{
public:
    Negative() : PUPPY::PluginInterface() {};

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
};

const PUPPY::AppInterface* gAppInterface = nullptr;

const Negative gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Negative",
    "2.0",
    "loochek",
    "Simple negative effect",

    nullptr,                // icon
    
    PUPPY::PluginType::EFFECT
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
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

const PUPPY::PluginInfo *Negative::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Negative::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;
    appInterface->log("Negative: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Negative::deinit()
{
    return PUPPY::Status::OK;
}

void Negative::dump() const
{
}

void Negative::on_tick(double dt) const
{
}

void Negative::effect_apply() const
{
    PUPPY::RenderTarget *activeLayer = gAppInterface->get_target();
    PUPPY::Vec2s layerSize(activeLayer->get_size());

    PUPPY::RGBA *pixels = activeLayer->get_pixels();

    for (int y = 0; y < layerSize.y; y++)
    {
        for (int x = 0; x < layerSize.x; x++)
        {
            PUPPY::RGBA color = pixels[layerSize.x * y + x];
            color.r = 255 - color.r;
            color.g = 255 - color.g;
            color.b = 255 - color.b;

            pixels[layerSize.x * y + x] = color;
        }
    }

    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);
    activeLayer->render_pixels(PUPPY::Vec2f(0, 0), layerSize, pixels, mode);
    
    delete[] pixels;
    delete activeLayer;
}

void Negative::tool_on_press(const PUPPY::Vec2f &position) const
{
}

void Negative::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Negative::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void Negative::show_settings() const
{
}