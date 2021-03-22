#include "sfml_app.h"

void app_init(app_state_t *app, mandelbrot_cfg_t *cfg)
{
    app->mandelbrot_cfg = *cfg;

    app->window = sfRenderWindow_create((sfVideoMode){ cfg->canvas_width, cfg->canvas_height, 32 },
                                        "Mandelbrot",
                                        sfDefaultStyle,
                                        NULL);

    app->canvas         = sfImage_create  (cfg->canvas_width, cfg->canvas_height);
    app->canvas_texture = sfTexture_create(cfg->canvas_width, cfg->canvas_height);
    app->canvas_sprite  = sfSprite_create();
    sfSprite_setPosition(app->canvas_sprite, (sfVector2f){ 0, 0 });
    sfSprite_setTexture(app->canvas_sprite, app->canvas_texture, 0);
}

void app_run(app_state_t *app)
{
    while (sfRenderWindow_isOpen(app->window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(app->window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(app->window);
        }

        raster(&app->mandelbrot_cfg, app->canvas, 0, 0, app->mandelbrot_cfg.canvas_width, app->mandelbrot_cfg.canvas_height);
        sfTexture_updateFromImage(app->canvas_texture, app->canvas, 0, 0);

        sfRenderWindow_clear(app->window, sfBlack);
        sfRenderWindow_drawSprite(app->window, app->canvas_sprite, NULL);
        sfRenderWindow_display(app->window);
    }
}

void app_destroy(app_state_t *app)
{
    sfSprite_destroy      (app->canvas_sprite);
    sfTexture_destroy     (app->canvas_texture);
    sfImage_destroy       (app->canvas);
    sfRenderWindow_destroy(app->window);
}