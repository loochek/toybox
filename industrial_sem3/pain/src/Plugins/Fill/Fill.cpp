#include <deque>
#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "../../Utils/Vec2.hpp"
#include "../../Editor/AppInterface/PluginTypesProxy.hpp"

class Fill : public PUPPY::PluginInterface
{
public:
    Fill() : PUPPY::PluginInterface() {};

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
    static bool areColorsEqual(const PUPPY::RGBA &first, const PUPPY::RGBA &second);
};

const PUPPY::AppInterface* gAppInterface = nullptr;

const Fill gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Fill",
    "2.0",
    "loochek",
    "Simple Paint-like fill",

    nullptr,                // icon
    
    PUPPY::PluginType::TOOL
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

const Vec2i DIRS[] = { Vec2i(-1, 0), Vec2i(1, 0), Vec2i(0, -1), Vec2i(0, 1) };

bool Fill::ext_enable(const char *name) const
{
    return false;
}

void *Fill::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Fill::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *Fill::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Fill::init(const PUPPY::AppInterface* appInterface) const
{
    gAppInterface = appInterface;
    appInterface->log("Fill: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Fill::deinit() const
{
    return PUPPY::Status::OK;
}

void Fill::dump() const
{
}

void Fill::on_tick(double dt) const
{
}

void Fill::effect_apply() const
{
}

void Fill::tool_on_press(const PUPPY::Vec2f &position) const
{
    PUPPY::RenderTarget *activeLayer = gAppInterface->get_target();

    Vec2i clickPos(fromPluginVec(position)), layerSize(fromPluginVec(activeLayer->get_size()));;
    if (clickPos.x < 0 || clickPos.x >= layerSize.x || clickPos.y < 0 || clickPos.y >= layerSize.y)
        return;

    PUPPY::RGBA fillColor = gAppInterface->get_color();

    PUPPY::RGBA *pixels = activeLayer->get_pixels();

    std::deque<Vec2i> fillQueue;
    fillQueue.push_back(clickPos);

    while (!fillQueue.empty())
    {
        Vec2i currPixel = fillQueue.front();
        fillQueue.pop_front();

        if (areColorsEqual(pixels[layerSize.x * currPixel.y + currPixel.x], fillColor))
            continue;

        for (Vec2i dir : DIRS)
        {
            if (currPixel.x + dir.x >= 0 && currPixel.x + dir.x < layerSize.x &&
                currPixel.y + dir.y >= 0 && currPixel.y + dir.y < layerSize.y)
            {
                Vec2i sidePixel = currPixel + dir;
                if (areColorsEqual(pixels[layerSize.x * currPixel.y + currPixel.x],
                                   pixels[layerSize.x * sidePixel.y + sidePixel.x]))
                {
                    fillQueue.push_back(sidePixel);
                }
            }
        }

        pixels[layerSize.x * currPixel.y + currPixel.x] = fillColor;
    }

    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);
    activeLayer->render_pixels(PUPPY::Vec2f(0, 0), toPluginVec(Vec2<size_t>(layerSize)), pixels, mode);

    delete[] pixels;
    delete activeLayer;
}

void Fill::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Fill::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void Fill::show_settings() const
{
}

bool Fill::areColorsEqual(const PUPPY::RGBA &first, const PUPPY::RGBA &second)
{
    return first.r == second.r && first.g == second.g && first.b == second.b && first.a == second.a;
}