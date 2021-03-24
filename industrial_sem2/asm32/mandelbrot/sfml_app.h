#include <SFML/Graphics.h>

#include "mandelbrot.h"

typedef enum
{
    RASTER_DEFAULT,
    RASTER_AVX_PREC,
    RASTER_AVX_PERF
} rasterizer_e;

/**
 * SFML pplication state
 */
typedef struct
{
    mandelbrot_cfg_t mandelbrot_cfg;

    double           scale_speed;
    double           movement_speed;

    rasterizer_e     rasterizer_in_use;

    sfRenderWindow  *window;
    sfImage         *canvas;
    sfTexture       *canvas_texture;
    sfSprite        *canvas_sprite;
    sfClock         *clock;

} app_state_t;

/**
 * Initializes SFML application
 * if exit code is not 0, SFML initialization failed
 */
int app_init(app_state_t *app, mandelbrot_cfg_t *cfg);

/**
 * Application main loop
 */
void app_run(app_state_t *app);

/**
 * Destroys SFML application
 */
void app_destroy(app_state_t *app);