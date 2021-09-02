#include "sfml_app.h"

/// Reports status to the user with additional message
#define LSTATUS_ERR_PRINT(msg) fprintf(stderr, "%s ([%s] %s)\n", msg, lstatus_str_repr[status], lstatus_str)

int main()
{
    lstatus_e status = LSTATUS_OK;

    sfml_app_t app = {0};

    status = app_init(&app);
    if (status != LSTATUS_OK)
    {
        LSTATUS_ERR_PRINT("Error!");
        return -1;
    }

    app_run(&app);
    
    app_deinit(&app);
    return 0;
}