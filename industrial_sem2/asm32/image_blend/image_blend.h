#include <SFML/Graphics.h>

typedef struct
{
    sfImage *background;
    sfImage *foreground;

    int foreground_x_offset;
    int foreground_y_offset;

} blender_input_t;

void blend(sfImage *output_canvas, blender_input_t *input);
void blend_sse(sfImage *output_canvas, blender_input_t *input);