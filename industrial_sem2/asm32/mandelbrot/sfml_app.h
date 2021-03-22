#include <SFML/Graphics.h>

#include "mandelbrot.h"

typedef struct
{
    /**
     * Rasterization config
     */
    mandelbrot_cfg_t mandelbrot_cfg;

    sfRenderWindow  *window;
    sfImage         *canvas;
    sfTexture       *canvas_texture;
    sfSprite        *canvas_sprite;

} app_state_t;

void app_init   (app_state_t *app, mandelbrot_cfg_t *cfg);
void app_run    (app_state_t *app);
void app_destroy(app_state_t *app);