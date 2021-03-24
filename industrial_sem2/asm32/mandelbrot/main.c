#include <stdio.h>

#include "sfml_app.h"

int main()
{
    app_state_t app = {0};
    
    mandelbrot_cfg_t cfg = {
                                800,   // width
                                600,   // height
                                256,   // step_limit
                                100.f, // radius_sq
                               -1.f,   // origin_x
                                0.f,   // origin_y
                                0.001f // scale
                           };

    if (app_init(&app, &cfg) != 0)
    {
        printf("SFML initialization failed!\n");
        app_destroy(&app);
        return -1;
    }

    app_run    (&app);
    app_destroy(&app);

    return 0;
}