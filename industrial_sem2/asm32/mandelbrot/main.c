#include "sfml_app.h"

int main()
{
    app_state_t app = {0};
    
    mandelbrot_cfg_t cfg = {
                                800,
                                600,
                                256,
                                100.f,
                               -1.f,
                                0.f,
                                0.001f
                           };

    app_init   (&app, &cfg);
    app_run    (&app);
    app_destroy(&app);

    return 0;
}