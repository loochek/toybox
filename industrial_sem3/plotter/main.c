#include <math.h>
#include <SFML/Graphics.h>
#include "lstatus.h"
#include "plot.h"

/// Reports status to the user with additional message
#define LSTATUS_ERR_PRINT(msg) fprintf(stderr, "%s ([%s] %s)\n", msg, lstatus_str_repr[status], lstatus_str)

static double plot_func(double x)
{
    return 1/ sin(x);
}

lstatus_e app_run()
{
    lstatus_e status = LSTATUS_OK;

    sfVideoMode video_mode = { 1280, 720, 32 };
    sfRenderWindow *window = sfRenderWindow_create(video_mode, "plotter", sfClose, NULL);
    if (window == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto cleanup0;
    }

    plot_t plot1 = {0};
    status = plot_init(&plot1);
    if (status != LSTATUS_OK)
    {
        goto cleanup1;
    }

    plot_t plot2 = {0};
    status = plot_init(&plot2);
    if (status != LSTATUS_OK)
    {
        goto cleanup2;
    }

    plot_set_viewport_size(&plot1, 200, 200);
    plot_set_scale_factor(&plot1, 1 / 20.0);

    plot_set_func(&plot2, plot_func);
    plot_set_viewport_size(&plot2, 500, 500);

    sfRenderWindow_setFramerateLimit(window, 60);

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }

        if (sfKeyboard_isKeyPressed(sfKeyUp))
            plot_move_viewport(&plot2, 0, 1.0);

        if (sfKeyboard_isKeyPressed(sfKeyDown))
            plot_move_viewport(&plot2, 0, -1.0);

        if (sfKeyboard_isKeyPressed(sfKeyRight))
            plot_move_viewport(&plot2, 1.0, 0);

        if (sfKeyboard_isKeyPressed(sfKeyLeft))
            plot_move_viewport(&plot2, -1.0, 0);

        if (sfKeyboard_isKeyPressed(sfKeyZ))
            plot_scale(&plot2, 1.01);

        if (sfKeyboard_isKeyPressed(sfKeyX))
            plot_scale(&plot2, 1 / 1.01);
        
        sfRenderWindow_clear(window, sfRed);
        plot_draw(&plot1, window, (sfVector2f){ 100, 100 });
        plot_draw(&plot2, window, (sfVector2f){ 400, 100 });
        sfRenderWindow_display(window);
    }

    plot_deinit(&plot2);

cleanup2:
    plot_deinit(&plot1);

cleanup1:
    sfRenderWindow_destroy(window);

cleanup0:
    return status;
}

int main()
{
    lstatus_e status = app_run();

    if (status != LSTATUS_OK)
    {
        LSTATUS_ERR_PRINT("Error!");
        return -1;
    }

    return 0;
}