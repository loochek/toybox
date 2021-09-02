#ifndef PLOT_H
#define PLOT_H

#include <SFML/Graphics.h>
#include "lstatus.h"

typedef double (*plot_func_t)(double x);

typedef struct
{
    plot_func_t func;

    /// Viewport width
    double viewport_width;

    /// Viewport height
    double viewport_height;

    /// Width of a plot fragment visible through viewport
    double plot_width;

    /// Height of a plot fragment visible through viewport
    double plot_height;

    /// Viewport left up corner coord
    double plot_offset_x;

    /// Viewport left up corner coord
    double plot_offset_y;

    sfRectangleShape *background;
    sfCircleShape    *dot_drawer;
    sfConvexShape    *line_drawer;
} plot_t;

/**
 * Initializes a plot structure
 * 
 * \param plot Plot instanse
 */
lstatus_e plot_init(plot_t *plot);

/**
 * Sets viewport offset in plot coordinates
 * 
 * \param plot Plot instanse
 * \param x_offs X offset
 * \param y_offs Y offset
 */
void plot_set_viewport_offset(plot_t *plot, double x_offs, double y_offs);

/**
 * Sets size of a plot fragment visible through viewport
 * 
 * \param plot Plot instanse
 * \param width Plot fragment width in plot coords
 * \param height Plot fragment height in plot coords
 */
void plot_set_viewport_plot_size(plot_t *plot, double width, double height);

/**
 * Sets viewport size for a plot
 * 
 * \param plot Plot instanse
 * \param width Viewport width
 * \param height Viewport height
 */
void plot_set_viewport_size(plot_t *plot, double width, double height);

/**
 * Sets function to draw
 * 
 * \param plot Plot instanse
 * \param func Function
 */
void plot_set_func(plot_t *plot, plot_func_t func);

/**
 * Moves viewport
 * 
 * \param plot Plot instanse
 * \param x_offs X offset in plot coords
 * \param y_offs Y offset in plot coords
 */
void plot_move_viewport(plot_t *plot, double x_offs, double y_offs);

/**
 * Scale viewport relative to offset
 * 
 * \param plot Plot instanse
 * \param scale Scale factor
 */
void plot_scale(plot_t *plot, double scale);

/**
 * Draws the plot
 * 
 * \param plot Plot instanse
 * \param canvas SFML render window
 * \param viewport_pos Viewport position
 */
void plot_draw(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_pos);

/**
 * Deinitializes a plot structure
 * 
 * \param plot Plot instanse
 */
void plot_deinit(plot_t *plot);

#endif