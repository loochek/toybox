#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

static PPluginStatus init(const PAppInterface* appInterface);
static PPluginStatus deinit();

static void dump();
static void onUpdate(double elapsedTime);

static const PPluginInfo  *getInfo();
static PPreviewLayerPolicy getFlushPolicy();

static void apply();

static bool enableExtension(const char *name);
static void *getExtensionFunc(const char *extension, const char *func);


static PRGBA *applyKernel(PRGBA *pixels, int width, int height, const float kernel[]);


static const float GAUSS_BLUR[3][3] =
{
    { 1.f / 16, 2.f / 16, 1.f / 16 },
    { 2.f / 16, 4.f / 16, 2.f / 16 },
    { 1.f / 16, 2.f / 16, 1.f / 16 }
};

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
    nullptr,
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

    "Gauss blur",
    "1.0",
    "loochek",
    "Simple blur effect",
    
    PPT_EFFECT
};


const PAppInterface *gAppInterface = nullptr;


extern "C" const PPluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

static PPluginStatus init(const PAppInterface* appInterface)
{
    gAppInterface = appInterface;
    appInterface->general.log("Blur: succesful initialization!");
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

static PPreviewLayerPolicy getFlushPolicy()
{
    return PPLP_BLEND;
}

static void apply()
{
    PRenderMode render_mode = { PPBM_COPY, PPDP_ACTIVE, nullptr };

    size_t layerWidth = 0, layerHeight = 0;
    gAppInterface->target.get_size(&layerWidth, &layerHeight);

    PRGBA *pixels = gAppInterface->target.get_pixels();
    
    PRGBA *dstPixels = applyKernel(pixels, layerWidth, layerHeight, (float*)GAUSS_BLUR);

    gAppInterface->render.pixels(PVec2f(0, 0), dstPixels, layerWidth, layerHeight, &render_mode);
    gAppInterface->general.release_pixels(pixels);
    delete[] dstPixels;
}

static bool enableExtension(const char *name)
{
    return false;
}

static void *getExtensionFunc(const char *extension, const char *func)
{
    return nullptr;
}

static PRGBA *applyKernel(PRGBA *pixels, int width, int height, const float kernel[])
{
    PRGBA *dstPixels = new PRGBA[width * height];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float r = 0, g = 0, b = 0;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    int ky = (y + i - 1), kx = (x + j - 1);

                    if (kx < 0)
                        kx = 0;

                    if (ky < 0)
                        ky = 0;

                    if (kx >= width)
                        kx = width - 1;

                    if (ky >= height)
                        ky = height - 1;

                    PRGBA color = pixels[width * ky + kx];

                    r += color.r * kernel[i * 3 + j];
                    g += color.g * kernel[i * 3 + j];
                    b += color.b * kernel[i * 3 + j];
                }
            }

            dstPixels[width * y + x] = PRGBA(r, g, b, pixels[width * y + x].a);
        }
    }

    return dstPixels;
}