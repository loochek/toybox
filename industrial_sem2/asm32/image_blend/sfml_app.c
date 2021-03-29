#include <stdio.h>

#include "sfml_app.h"

const int CANVAS_WIDTH  = 800;
const int CANVAS_HEIGHT = 600;

int app_init(app_state_t *app)
{
    app->window = sfRenderWindow_create((sfVideoMode){ CANVAS_WIDTH, CANVAS_HEIGHT, 32 },
                                        "Image blend",
                                        sfDefaultStyle,
                                        NULL);

    app->canvas = sfImage_create(CANVAS_WIDTH, CANVAS_HEIGHT);

    app->canvas_texture = sfTexture_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!app->canvas_texture)
        return -1;

    app->canvas_sprite = sfSprite_create();
    if (!app->canvas_sprite)
        return -1;

    sfSprite_setPosition(app->canvas_sprite, (sfVector2f){ 0, 0 });
    sfSprite_setTexture(app->canvas_sprite, app->canvas_texture, 0);

    app->clock = sfClock_create();

    app->input.background = sfImage_createFromFile("Table.bmp");
    app->input.foreground = sfImage_createFromFile("AskhatCat.bmp");
    app->input.foreground_x_offset = 251;
    app->input.foreground_y_offset = 211;

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

        sfClock_restart(app->clock);

        for (int i = 0; i < 100; i++)
            blend_avx(app->canvas, &app->input);

        float elapsed = sfTime_asSeconds(sfClock_restart(app->clock));
        printf("Rasterizer FPS: %f\n", 1.0f / elapsed);

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