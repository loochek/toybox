#ifndef SFML_APP_H
#define SFML_APP_H

#include <SFML/Graphics.h>
#include "lstatus.h"
#include "plot.h"

/**
 * SFML app framework
 */

typedef struct 
{
    sfRenderWindow *window;

    plot_t plot1;
    plot_t plot2;
} sfml_app_t;

/**
 * Initializes a SFML app instance
 * 
 * \param app App instance
 */
lstatus_e app_init(sfml_app_t *app);

/**
 * Runs SFML app until exit
 * 
 * \param app App instance
 */
void app_run(sfml_app_t *app);

/**
 * Deinitializes SFML app instance
 * 
 * \param app App instance
 */
void app_deinit(sfml_app_t *app);

#endif