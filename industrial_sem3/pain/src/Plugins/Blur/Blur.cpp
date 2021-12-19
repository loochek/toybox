#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/Color.hpp"

class Blur : public PUPPY::PluginInterface
{
public:
    Blur() : PUPPY::PluginInterface() {};

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

private:
    PUPPY::RGBA *applyKernel(PUPPY::RGBA *pixels, const PUPPY::Vec2s &size, int kernelSize,
                             const float kernel[]) const;
    void calcGaussianKernel(int kernelRadius, float sigma, float kernel[]) const;
    float gaussian(float x, float y, float sigma) const;
    void getSettings(float &power) const;
    LGL::Color toLGLColor(const PUPPY::RGBA &color) const;
    PUPPY::RGBA toPluginColor(const LGL::Color &color) const;
};

//===================================================================

const PUPPY::AppInterface* gAppInterface = nullptr;

struct {
    PUPPY::Window    *window;
    PUPPY::TextField *powerTextBox;
} gWidgets;

const PUPPY::WBody WINDOW_BODY(PUPPY::Vec2f(400, 400), PUPPY::Vec2f(160, 40));
const PUPPY::WBody POWER_TEXT_BOX_BODY(PUPPY::Vec2f(120, 10), PUPPY::Vec2f(30, 20));
const PUPPY::Vec2f POWER_LABEL_POS(10, 15);

const float DEFAULT_POWER = 0.7f;

//===================================================================

const Blur gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,     // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Gauss blur",
    "2.0",
    "loochek",
    "Simple blur effect",

    nullptr,                // icon
    
    PUPPY::PluginType::EFFECT
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

//===================================================================

bool Blur::ext_enable(const char *name) const
{
    return false;
}

void *Blur::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Blur::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *Blur::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Blur::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;

    if (appInterface->factory.widget)
    {
        const PUPPY::WidgetFactory *factory = appInterface->factory.widget;

        gWidgets.window       = factory->window("Gaussian blur", WINDOW_BODY);
        gWidgets.powerTextBox = factory->text_field(POWER_TEXT_BOX_BODY, gWidgets.window);

        factory->label(POWER_LABEL_POS, "Power:", gWidgets.window);
        gWidgets.powerTextBox->set_text("1.3");
    }

    appInterface->log("Blur: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Blur::deinit()
{
    if (gWidgets.window) {
        gWidgets.window->set_to_delete();
    }

    return PUPPY::Status::OK;
}

void Blur::dump() const
{
}

void Blur::on_tick(double dt) const
{
}

void Blur::effect_apply() const
{
    PUPPY::RenderTarget *activeLayer = gAppInterface->get_target();
    PUPPY::Vec2s layerSize(activeLayer->get_size());

    PUPPY::RGBA *pixels = activeLayer->get_pixels();

    float power = 0;
    getSettings(power);
    int kernelRadius = ceil(2 * power);

    int kernelSize = 1 + 2 * kernelRadius;
    float *kernel = new float[kernelSize * kernelSize];
    calcGaussianKernel(kernelRadius, power, kernel);
    
    PUPPY::RGBA *dstPixels = applyKernel(pixels, layerSize, kernelSize, kernel);

    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);
    activeLayer->render_pixels(PUPPY::Vec2f(0, 0), layerSize, dstPixels, mode);
    
    delete[] pixels;
    delete[] dstPixels;
    delete[] kernel;
    delete activeLayer;
}

void Blur::tool_on_press(const PUPPY::Vec2f &position) const
{
}

void Blur::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Blur::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void Blur::show_settings() const
{
}

PUPPY::RGBA *Blur::applyKernel(PUPPY::RGBA *pixels, const PUPPY::Vec2s &size, int kernelSize,
                               const float kernel[]) const
{
    PUPPY::RGBA *dstPixels = new PUPPY::RGBA[size.x * size.y];

    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {
            LGL::Color sumColor(0.0f, 0.0f, 0.0f);

            for (int iKernRow = 0; iKernRow < kernelSize; iKernRow++)
            {
                for (int iKernCol = 0; iKernCol < kernelSize; iKernCol++)
                {
                    int kx = x + iKernRow - kernelSize / 2;
                    int ky = y + iKernCol - kernelSize / 2;

                    if (kx < 0)
                        kx = 0;

                    if (ky < 0)
                        ky = 0;

                    if (kx >= size.x)
                        kx = size.x - 1;

                    if (ky >= size.y)
                        ky = size.y - 1;

                    LGL::Color color = toLGLColor(pixels[size.x * ky + kx]);
                    float coeff = kernel[iKernRow * kernelSize + iKernCol];

                    sumColor += color * coeff;
                }
            }

            dstPixels[size.x * y + x] = toPluginColor(sumColor);
            dstPixels[size.x * y + x].a = pixels[size.x * y + x].a;
        }
    }

    return dstPixels;
}

void Blur::calcGaussianKernel(int kernelRadius, float sigma, float kernel[]) const
{
    int kernelSize = 1 + kernelRadius * 2;

    float sum = 0;
    for (int y = -kernelRadius; y <= kernelRadius; y++)
    {
        for (int x = -kernelRadius; x <= kernelRadius; x++)
        {
            float val = gaussian(x, y, sigma);
            kernel[(y + kernelRadius) * kernelSize + (x + kernelRadius)] = val;
            sum += val;
        }
    }

    for (int i = 0; i < kernelSize; i++)
    {
        for (int j = 0; j < kernelSize; j++)
            kernel[i * kernelSize + j] /= sum;
    }
}

float Blur::gaussian(float x, float y, float sigma) const
{
    float pow = -(x * x + y * y) / (2.0f * sigma * sigma);
    return expf(pow) / (2.0f * M_PI * sigma * sigma);
}

void Blur::getSettings(float &power) const
{
    if (gWidgets.powerTextBox != nullptr)
    {
        power = atof(gWidgets.powerTextBox->get_text().data());
        if (power < 0.1f)
            power = DEFAULT_POWER;
    }
    else
        power = DEFAULT_POWER;
}

LGL::Color Blur::toLGLColor(const PUPPY::RGBA &color) const
{
    return LGL::Color((float)color.r / EXTERNAL_RGB_BASE,
                      (float)color.g / EXTERNAL_RGB_BASE,
                      (float)color.b / EXTERNAL_RGB_BASE,
                      (float)color.a / EXTERNAL_RGB_BASE);
}

PUPPY::RGBA Blur::toPluginColor(const LGL::Color &color) const
{
    return PUPPY::RGBA(color.r * EXTERNAL_RGB_BASE,
                       color.g * EXTERNAL_RGB_BASE,
                       color.b * EXTERNAL_RGB_BASE,
                       color.a * EXTERNAL_RGB_BASE);
}