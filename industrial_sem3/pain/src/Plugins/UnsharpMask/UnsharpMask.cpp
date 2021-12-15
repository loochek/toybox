#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/Color.hpp"

class Unsharp : public PUPPY::PluginInterface
{
public:
    Unsharp() : PUPPY::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface* appInterface) const override;
    virtual PUPPY::Status deinit() const override;
    virtual void dump() const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_release(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

    virtual void show_settings() const override;

private:
    PUPPY::RGBA *unsharp(PUPPY::RGBA *pixels, const PUPPY::Vec2s &size, float amount, int kernelSize,
                             const float kernel[]) const;
    void calcGaussianKernel(int kernelRadius, float sigma, float kernel[]) const;
    float gaussian(float x, float y, float sigma) const;
    void getSettings(float &power, float &amount) const;
    LGL::Color toLGLColor(const PUPPY::RGBA &color) const;
    PUPPY::RGBA toPluginColor(const LGL::Color &color) const;
};

//===================================================================

const PUPPY::AppInterface* gAppInterface = nullptr;

struct {
    PUPPY::Window    *window;
    PUPPY::TextField *powerTextBox;
    PUPPY::TextField *amountTextBox;
} gWidgets;

const PUPPY::WBody WINDOW_BODY(PUPPY::Vec2f(400, 400), PUPPY::Vec2f(200, 50));
const PUPPY::WBody POWER_TEXT_BOX_BODY (PUPPY::Vec2f(40, 10), PUPPY::Vec2f(30, 20));
const PUPPY::WBody AMOUNT_TEXT_BOX_BODY(PUPPY::Vec2f(40, 40), PUPPY::Vec2f(30, 20));
const PUPPY::Vec2f POWER_LABEL_POS(10, 10);
const PUPPY::Vec2f AMOUNT_LABEL_POS(10, 40);

const float DEFAULT_POWER  = 1.3;
const float DEFAULT_AMOUNT = 0.8;

//===================================================================

const Unsharp gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,     // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Unsharp mask",
    "2.0",
    "loochek",
    "Simple unsharp masking effect",

    nullptr,                // icon
    
    PUPPY::PluginType::EFFECT
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

//===================================================================

bool Unsharp::ext_enable(const char *name) const
{
    return false;
}

void *Unsharp::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Unsharp::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *Unsharp::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Unsharp::init(const PUPPY::AppInterface* appInterface) const
{
    gAppInterface = appInterface;

    if (appInterface->factory.widget)
    {
        const PUPPY::WidgetFactory *factory = appInterface->factory.widget;

        gWidgets.window        = factory->window("Unsharp mask", WINDOW_BODY);
        gWidgets.powerTextBox  = factory->text_field(POWER_TEXT_BOX_BODY , gWidgets.window);
        gWidgets.amountTextBox = factory->text_field(AMOUNT_TEXT_BOX_BODY, gWidgets.window);

        factory->label(POWER_LABEL_POS, "Power:", gWidgets.window);
        factory->label(AMOUNT_LABEL_POS, "Amount:", gWidgets.window);

        gWidgets.powerTextBox->set_text("1.3");
        gWidgets.amountTextBox->set_text("0.8");
    }

    appInterface->log("Unsharp: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Unsharp::deinit() const
{
    if (gWidgets.window) {
        gWidgets.window->set_to_delete();
    }

    return PUPPY::Status::OK;
}

void Unsharp::dump() const
{
}

void Unsharp::on_tick(double dt) const
{
}

void Unsharp::effect_apply() const
{
    PUPPY::RenderTarget *activeLayer = gAppInterface->get_target();
    PUPPY::Vec2s layerSize(activeLayer->get_size());

    PUPPY::RGBA *pixels = activeLayer->get_pixels();

    float power = 0;
    float amount = 0;
    getSettings(power, amount);
    int kernelRadius = ceil(2 * power);

    int kernelSize = 1 + 2 * kernelRadius;
    float *kernel = new float[kernelSize * kernelSize];
    calcGaussianKernel(kernelRadius, power, kernel);
    
    PUPPY::RGBA *dstPixels = unsharp(pixels, layerSize, amount, kernelSize, kernel);

    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);
    activeLayer->render_pixels(PUPPY::Vec2f(0, 0), layerSize, dstPixels, mode);
    
    delete[] pixels;
    delete[] dstPixels;
    delete[] kernel;
    delete activeLayer;
}

void Unsharp::tool_on_press(const PUPPY::Vec2f &position) const
{
}

void Unsharp::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Unsharp::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void Unsharp::show_settings() const
{
}

PUPPY::RGBA *Unsharp::unsharp(PUPPY::RGBA *pixels, const PUPPY::Vec2s &size, float amount, int kernelSize,
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

            LGL::Color originalColor = toLGLColor(pixels[size.x * y + x]);
            LGL::Color blurredColor  = sumColor.applyAlpha((float)pixels[size.x * y + x].a / EXTERNAL_RGB_BASE);

            LGL::Color finalColor = (originalColor - (blurredColor - originalColor) * amount).norm();

            dstPixels[size.x * y + x] = toPluginColor(finalColor);
        }
    }

    return dstPixels;
}

void Unsharp::calcGaussianKernel(int kernelRadius, float sigma, float kernel[]) const
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

float Unsharp::gaussian(float x, float y, float sigma) const
{
    float pow = -(x * x + y * y) / (2.0f * sigma * sigma);
    return expf(pow) / (2.0f * M_PI * sigma * sigma);
}

void Unsharp::getSettings(float &power, float &amount) const
{
    if (gWidgets.powerTextBox != nullptr)
    {
        power = atof(gWidgets.powerTextBox->get_text().data());
        if (power < 0.1f)
            power = DEFAULT_POWER;
    }
    else
        power = DEFAULT_POWER;

    if (gWidgets.amountTextBox != nullptr)
    {
        amount = atof(gWidgets.amountTextBox->get_text().data());
        if (amount < 0.1f)
            amount = DEFAULT_AMOUNT;
    }
    else
        amount = DEFAULT_AMOUNT;
}

LGL::Color Unsharp::toLGLColor(const PUPPY::RGBA &color) const
{
    return LGL::Color((float)color.r / EXTERNAL_RGB_BASE,
                      (float)color.g / EXTERNAL_RGB_BASE,
                      (float)color.b / EXTERNAL_RGB_BASE,
                      (float)color.a / EXTERNAL_RGB_BASE);
}

PUPPY::RGBA Unsharp::toPluginColor(const LGL::Color &color) const
{
    return PUPPY::RGBA(color.r * EXTERNAL_RGB_BASE,
                       color.g * EXTERNAL_RGB_BASE,
                       color.b * EXTERNAL_RGB_BASE,
                       color.a * EXTERNAL_RGB_BASE);
}