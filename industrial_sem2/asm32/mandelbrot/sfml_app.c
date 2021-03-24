#include <stdio.h>

#include "sfml_app.h"

const int    INITIAL_MOVEMENT_SPEED = 100.0f;
const double INITIAL_SCALE_SPEED    = 0.3f;

int app_init(app_state_t *app, mandelbrot_cfg_t *cfg)
{
    app->mandelbrot_cfg = *cfg;
    app->movement_speed = INITIAL_MOVEMENT_SPEED;
    app->scale_speed    = INITIAL_SCALE_SPEED;

    app->window = sfRenderWindow_create((sfVideoMode){ cfg->canvas_width, cfg->canvas_height, 32 },
                                        "Mandelbrot",
                                        sfDefaultStyle,
                                        NULL);

    app->canvas = sfImage_create(cfg->canvas_width, cfg->canvas_height);

    app->canvas_texture = sfTexture_create(cfg->canvas_width, cfg->canvas_height);
    if (!app->canvas_texture)
        return -1;

    app->canvas_sprite = sfSprite_create();
    if (!app->canvas_sprite)
        return -1;

    sfSprite_setPosition(app->canvas_sprite, (sfVector2f){ 0, 0 });
    sfSprite_setTexture(app->canvas_sprite, app->canvas_texture, 0);

    app->clock = sfClock_create();

    return 0;
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

        // Application loop
        // 0: handle time
        float elapsed = sfTime_asSeconds(sfClock_restart(app->clock));
        printf("FPS: %f\n", 1.0f / elapsed);

        // 1: handle input
        double scale_factor = 1.0f + app->scale_speed * elapsed;

        if (sfKeyboard_isKeyPressed(sfKeyZ))
            app->mandelbrot_cfg.scale /= scale_factor;

        if (sfKeyboard_isKeyPressed(sfKeyX))
            app->mandelbrot_cfg.scale *= scale_factor;

        double movement_offset = app->mandelbrot_cfg.scale * app->movement_speed * elapsed;

        if (sfKeyboard_isKeyPressed(sfKeyLeft))
            app->mandelbrot_cfg.origin_x -= movement_offset;

        if (sfKeyboard_isKeyPressed(sfKeyLeft))
            app->mandelbrot_cfg.origin_x -= movement_offset;

        if (sfKeyboard_isKeyPressed(sfKeyRight))
            app->mandelbrot_cfg.origin_x += movement_offset;

        if (sfKeyboard_isKeyPressed(sfKeyUp))
            app->mandelbrot_cfg.origin_y -= movement_offset;

        if (sfKeyboard_isKeyPressed(sfKeyDown))
            app->mandelbrot_cfg.origin_y += movement_offset;

        if (sfKeyboard_isKeyPressed(sfKeyNum1))
            app->rasterizer_in_use = RASTER_DEFAULT;

        if (sfKeyboard_isKeyPressed(sfKeyNum2))
            app->rasterizer_in_use = RASTER_AVX_PREC;
        
        if (sfKeyboard_isKeyPressed(sfKeyNum3))
            app->rasterizer_in_use = RASTER_AVX_PERF;

        // 2: redraw canvas with updated config

        raster_func_t raster_func = NULL;

        switch (app->rasterizer_in_use)
        {
        case RASTER_AVX_PERF:
            raster_func = raster_avx_perf;
            break;
            
        case RASTER_AVX_PREC:
            raster_func = raster_avx_prec;
            break;

        default:
            raster_func = raster;
            break;
        }

        raster_func(&app->mandelbrot_cfg,
                    app->canvas,
                    0,
                    0,
                    app->mandelbrot_cfg.canvas_width,
                    app->mandelbrot_cfg.canvas_height);
        sfTexture_updateFromImage(app->canvas_texture, app->canvas, 0, 0);

        // 3: display
        sfRenderWindow_clear(app->window, sfBlack);
        sfRenderWindow_drawSprite(app->window, app->canvas_sprite, NULL);
        sfRenderWindow_display(app->window);
    }
}

void app_destroy(app_state_t *app)
{
    sfClock_destroy       (app->clock);
    sfSprite_destroy      (app->canvas_sprite);
    sfTexture_destroy     (app->canvas_texture);
    sfImage_destroy       (app->canvas);
    sfRenderWindow_destroy(app->window);
}