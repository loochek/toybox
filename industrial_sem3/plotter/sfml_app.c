#include <math.h>
#include "sfml_app.h"

static double plot_func(double x);

lstatus_e app_init(sfml_app_t *app)
{
    lstatus_e status = LSTATUS_OK;

    sfVideoMode video_mode = { 1280, 720, 32 };
    app->window = sfRenderWindow_create(video_mode, "plotter", sfClose, NULL);
    if (app->window == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler0;
    }

    app->font = sfFont_createFromFile("Roboto-Light.ttf");
    if (app->font == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler1;
    }

    sfRenderWindow_setFramerateLimit(app->window, 60);

    status = plot_init(&app->plot1, app->font);
    if (status != LSTATUS_OK)
    {
        goto error_handler2;
    }

    status = plot_init(&app->plot2, app->font);
    if (status != LSTATUS_OK)
    {
        goto error_handler3;
    }

    plot_set_viewport_size(&app->plot1, 200, 200);
    plot_set_scale_factor(&app->plot1, 1 / 20.0);
    plot_set_viewport_origin(&app->plot1, 0.0, 3.0);

    plot_set_func(&app->plot2, plot_func);
    plot_set_viewport_size(&app->plot2, 500, 500);

    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 0.0 }, (sfVector2f){ 0.0, 1.0 } });
    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 1.0 }, (sfVector2f){ 0.0, 1.0 } });
    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 1.0 }, (sfVector2f){ -0.5, -1.0 } });
    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 1.0 }, (sfVector2f){  0.5, -1.0 } });
    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 2.0 }, (sfVector2f){ -0.5, -1.0 } });
    plot_add_vector(&app->plot2, (applied_vector_t){ (sfVector2f){ 0.0, 2.0 }, (sfVector2f){  0.5, -1.0 } });

    return LSTATUS_OK;

error_handler3:
    plot_deinit(&app->plot1);

error_handler2:
    sfFont_destroy(app->font);

error_handler1:
    sfRenderWindow_destroy(app->window);

error_handler0:
    return status;
}

void app_run(sfml_app_t *app)
{
    while (sfRenderWindow_isOpen(app->window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(app->window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(app->window);
        }

        if (sfKeyboard_isKeyPressed(sfKeyUp))
            plot_move_viewport(&app->plot2, 0, 1.0);

        if (sfKeyboard_isKeyPressed(sfKeyDown))
            plot_move_viewport(&app->plot2, 0, -1.0);

        if (sfKeyboard_isKeyPressed(sfKeyRight))
            plot_move_viewport(&app->plot2, 1.0, 0);

        if (sfKeyboard_isKeyPressed(sfKeyLeft))
            plot_move_viewport(&app->plot2, -1.0, 0);

        if (sfKeyboard_isKeyPressed(sfKeyZ))
            plot_scale(&app->plot2, 1.01);

        if (sfKeyboard_isKeyPressed(sfKeyX))
            plot_scale(&app->plot2, 1 / 1.01);
        
        sfRenderWindow_clear(app->window, sfRed);
        plot_draw(&app->plot1, app->window, (sfVector2f){ 100, 100 });
        plot_draw(&app->plot2, app->window, (sfVector2f){ 400, 100 });
        sfRenderWindow_display(app->window);
    }
}

void app_deinit(sfml_app_t *app)
{
    plot_deinit(&app->plot2);
    plot_deinit(&app->plot1);

    sfFont_destroy(app->font);
    sfRenderWindow_destroy(app->window);
}

static double plot_func(double x)
{
    return x * sin(x);
}