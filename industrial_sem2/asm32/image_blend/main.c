#include <stdio.h>

#include "sfml_app.h"

int main()
{
    app_state_t app = {0};

    if (app_init(&app) != 0)
    {
        printf("SFML initialization failed!\n");
        app_destroy(&app);
        return -1;
    }

    app_run    (&app);
    app_destroy(&app);

    return 0;
}