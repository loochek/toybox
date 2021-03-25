#include <SFML/Graphics.h>

#include "image_blend.h"

/**
 * SFML pplication state
 */
typedef struct
{
    blender_input_t input;
    
    sfRenderWindow  *window;
    sfImage         *canvas;
    sfTexture       *canvas_texture;
    sfSprite        *canvas_sprite;
    sfClock         *clock;
} app_state_t;

/**
 * Initializes SFML application
 * if exit code is not 0, SFML initialization failed
 */
int app_init(app_state_t *app);

/**
 * Application main loop
 */
void app_run(app_state_t *app);

/**
 * Destroys SFML application
 */
void app_destroy(app_state_t *app);