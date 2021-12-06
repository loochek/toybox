#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/Color.hpp"

const float DEFAULT_POWER  = 0.1;
const float DEFAULT_AMOUNT = 1.5;

static PPluginStatus init(const PAppInterface* appInterface);
static PPluginStatus deinit();

static void dump();
static void onUpdate(double elapsedTime);
static void onSettingsUpdate();

static const PPluginInfo  *getInfo();
static PPreviewLayerPolicy getFlushPolicy();

static void apply();

static bool enableExtension(const char *name);
static void *getExtensionFunc(const char *extension, const char *func);

static void getSettings(float &power, float &amount);
static void calcGaussianKernel(int kernelRadius, float sigma, float kernel[]);
static PRGBA *unsharp(PRGBA *pixels, int width, int height, float amount, int kernelSize, const float kernel[]);
static float gaussian(float x, float y, float sigma);
static LGL::Color toLGLColor(const PRGBA &color);
static PRGBA toPluginColor(const LGL::Color &color);

const PPluginInterface gPluginInterface =
{
    0, // std_version
    0, // reserved
    
    enableExtension,
    getExtensionFunc,

    // general
    getInfo,
    init,
    deinit,
    dump,
    onUpdate,
    onSettingsUpdate,
    getFlushPolicy,

    // effect
    apply,

    // tool
    nullptr,
    nullptr,
    nullptr
};

const PPluginInfo gPluginInfo =
{
    0, // std_version
    0, // reserved

    &gPluginInterface,

    "Unsharp mask",
    "1.0",
    "loochek",
    "Simple unsharp masking effect",
    
    PPT_EFFECT
};


const PAppInterface *gAppInterface = nullptr;
void *gPowerSettingHandle  = nullptr;
void *gAmountSettingHandle = nullptr;


extern "C" const PPluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

static PPluginStatus init(const PAppInterface* appInterface)
{
    gAppInterface = appInterface;

    if (gAppInterface->general.feature_level & PFL_SETTINGS_SUPPORT)
    {
        appInterface->general.log("[Unsharp mask] Settings support is present");
        appInterface->settings.create_surface(&gPluginInterface, 100, 100);
        gPowerSettingHandle  = appInterface->settings.add(&gPluginInterface, PST_TEXT_LINE, "Power");
        gAmountSettingHandle = appInterface->settings.add(&gPluginInterface, PST_TEXT_LINE, "Amount");
    }
    else
        appInterface->general.log("[Unsharp mask] Settings support is NOT present!");

    appInterface->general.log("[Unsharp mask] Succesful initialization!");
    return PPS_OK; 
}

static PPluginStatus deinit()
{
    return PPS_OK;
}

static void dump()
{
}

static const PPluginInfo *getInfo()
{
    return &gPluginInfo;
}

static void onUpdate(double elapsedTime)
{
}

static void onSettingsUpdate()
{
}

static PPreviewLayerPolicy getFlushPolicy()
{
    return PPLP_BLEND;
}

static void apply()
{
    PRenderMode render_mode = { PPBM_COPY, PPDP_ACTIVE, nullptr };

    size_t layerWidth = 0, layerHeight = 0;
    gAppInterface->target.get_size(&layerWidth, &layerHeight);

    float power  = 0;
    float amount = 0;
    getSettings(power, amount);
    int kernelRadius = ceil(2 * power);

    PRGBA *pixels = gAppInterface->target.get_pixels();

    int kernelSize = 1 + 2 * kernelRadius;
    float *kernel = new float[kernelSize * kernelSize];
    calcGaussianKernel(kernelRadius, power, kernel);
    
    PRGBA *dstPixels = unsharp(pixels, layerWidth, layerHeight, amount, kernelSize, kernel);

    gAppInterface->render.pixels(PVec2f(0, 0), dstPixels, layerWidth, layerHeight, &render_mode);
    gAppInterface->general.release_pixels(pixels);
    delete[] dstPixels;
    delete[] kernel;
}

static bool enableExtension(const char *name)
{
    return false;
}

static void *getExtensionFunc(const char *extension, const char *func)
{
    return nullptr;
}

static void getSettings(float &power, float &amount)
{
    PTextFieldSetting settingValue;
    if (gPowerSettingHandle != nullptr)
    {
        gAppInterface->settings.get(&gPluginInterface, gPowerSettingHandle, &settingValue);
        power = atof(settingValue.text);
        if (power < 0.1f)
            power = DEFAULT_POWER;
    }
    else
        power = DEFAULT_POWER;

    if (gAmountSettingHandle != nullptr)
    {
        gAppInterface->settings.get(&gPluginInterface, gAmountSettingHandle, &settingValue);
        amount = atof(settingValue.text);
        if (amount < 0.01f)
            amount = DEFAULT_AMOUNT;
    }
    else
        amount = DEFAULT_AMOUNT;
}

static PRGBA *unsharp(PRGBA *pixels, int width, int height, float amount, int kernelSize, const float kernel[])
{
    PRGBA *dstPixels = new PRGBA[width * height];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
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

                    if (kx >= width)
                        kx = width - 1;

                    if (ky >= height)
                        ky = height - 1;

                    LGL::Color color = toLGLColor(pixels[width * ky + kx]);
                    float coeff = kernel[iKernRow * kernelSize + iKernCol];

                    sumColor += color * coeff;
                }
            }

            LGL::Color originalColor = toLGLColor(pixels[width * y + x]);
            LGL::Color blurredColor  = sumColor.applyAlpha((float)pixels[width * y + x].a / EXTERNAL_RGB_BASE);

            LGL::Color finalColor = (originalColor - (blurredColor - originalColor) * amount).norm();

            dstPixels[width * y + x] = toPluginColor(finalColor);
        }
    }

    return dstPixels;
}

static void calcGaussianKernel(int kernelRadius, float sigma, float kernel[])
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

static float gaussian(float x, float y, float sigma)
{
    float pow = -(x * x + y * y) / (2.0f * sigma * sigma);
    return expf(pow) / (2.0f * M_PI * sigma * sigma);
}

static LGL::Color toLGLColor(const PRGBA &color)
{
    return LGL::Color((float)color.r / EXTERNAL_RGB_BASE,
                      (float)color.g / EXTERNAL_RGB_BASE,
                      (float)color.b / EXTERNAL_RGB_BASE,
                      (float)color.a / EXTERNAL_RGB_BASE);
}

static PRGBA toPluginColor(const LGL::Color &color)
{
    return PRGBA(color.r * EXTERNAL_RGB_BASE,
                 color.g * EXTERNAL_RGB_BASE,
                 color.b * EXTERNAL_RGB_BASE,
                 color.a * EXTERNAL_RGB_BASE);
}