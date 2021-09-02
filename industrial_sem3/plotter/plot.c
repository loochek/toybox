#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "plot.h"

static const float ARROW_WIDTH  = 7.0;
static const float ARROW_HEIGHT = 10.0;

static void draw_line(plot_t *plot, sfRenderWindow *window,
                      sfVector2f p1, sfVector2f p2, float thickness, sfColor color);

static void draw_arrow(plot_t *plot, sfRenderWindow *window,
                       sfVector2f p1, sfVector2f p2, float thickness, sfColor color);

static double default_plot_func(double x)
{
    return x * x;
}

lstatus_e plot_init(plot_t *plot)
{
    assert(plot != NULL);

    lstatus_e status = LSTATUS_OK;

    plot->background = sfRectangleShape_create();
    if (plot->background == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler0;
    }

    plot->dot_drawer = sfCircleShape_create();
    if (plot->dot_drawer == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler1;
    }

    plot->line_drawer = sfConvexShape_create();
    if (plot->line_drawer == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler2;
    }

    // Drawers setup

    sfCircleShape_setRadius(plot->dot_drawer, 2.0f);
    sfCircleShape_setOrigin(plot->dot_drawer, (sfVector2f){ 2.f, 2.f });
    
    sfCircleShape_setFillColor(plot->dot_drawer, sfBlack);
    sfCircleShape_setPointCount(plot->dot_drawer, 10);

    // Default params

    plot->func = default_plot_func;

    plot_set_viewport_size(plot, 200, 200);
    plot_set_viewport_plot_size(plot, 100.0, 100.0);
    plot_set_viewport_offset(plot, -50.0, 50.0);

    return LSTATUS_OK;

error_handler2:
    sfCircleShape_destroy(plot->dot_drawer);

error_handler1:
    sfRectangleShape_destroy(plot->background);

error_handler0:
    return status;
}

void plot_set_viewport_offset(plot_t *plot, double x_offs, double y_offs)
{
    assert(plot != NULL);

    plot->plot_offset_x = x_offs;
    plot->plot_offset_y = y_offs;
}

void plot_set_viewport_plot_size(plot_t *plot, double width, double height)
{
    assert(plot != NULL);

    plot->plot_width  = width;
    plot->plot_height = height;
}

void plot_set_viewport_size(plot_t *plot, double width, double height)
{
    assert(plot != NULL);

    plot->viewport_width  = width;
    plot->viewport_height = height;

    sfRectangleShape_setSize(plot->background, (sfVector2f){ plot->viewport_width, plot->viewport_height });
}

void plot_set_func(plot_t *plot, plot_func_t func)
{
    assert(plot != NULL);

    plot->func = func;
}

void plot_move_viewport(plot_t *plot, double x_offs, double y_offs)
{
    assert(plot != NULL);

    plot->plot_offset_x += x_offs;
    plot->plot_offset_y += y_offs;
}

void plot_scale(plot_t *plot, double scale)
{
    assert(plot != NULL);

    plot->plot_height *= scale;
    plot->plot_width  *= scale;
}

void plot_draw(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position)
{
    assert(plot != NULL);
    assert(canvas != NULL);

    // Background drawing

    sfRectangleShape_setPosition(plot->background, viewport_position);
    sfRenderWindow_drawRectangleShape(canvas, plot->background, NULL);

    // Axis drawing

    double viewport_x_axis_offs = (double)plot->viewport_height / plot->plot_height * plot->plot_offset_y;
    
    if (viewport_x_axis_offs > 0 && viewport_x_axis_offs < plot->viewport_height)
    {
        sfVector2f x_axis_begin = { viewport_position.x,
                                    viewport_position.y + viewport_x_axis_offs };
        sfVector2f x_axis_end   = { viewport_position.x + plot->viewport_width,
                                    viewport_position.y + viewport_x_axis_offs };
        draw_arrow(plot, canvas, x_axis_begin, x_axis_end, 1.0f, sfBlack);
    }

    double viewport_y_axis_offs = (double)plot->viewport_width / plot->plot_width * (-plot->plot_offset_x);
    
    if (viewport_y_axis_offs > 0 && viewport_y_axis_offs < plot->viewport_width)
    {
        sfVector2f y_axis_begin = { viewport_position.x + viewport_y_axis_offs,
                                    viewport_position.y + plot->viewport_height};
        sfVector2f y_axis_end   = { viewport_position.x + viewport_y_axis_offs,
                                    viewport_position.y };
        draw_arrow(plot, canvas, y_axis_begin, y_axis_end, 1.0f, sfBlack);
    }

    // Plot drawing

    double prev_viewport_x_offs = -1.0, prev_viewport_y_offs = -1.0;
    bool prev_outside = false;

    for (double x_offs = 0; x_offs < plot->plot_width; x_offs += 0.2)
    {
        double x = plot->plot_offset_x + x_offs;
        double y = plot->func(x);

        double y_offs = plot->plot_offset_y - y;

        double viewport_x_offs = (double)plot->viewport_width  / plot->plot_width  * x_offs;
        double viewport_y_offs = (double)plot->viewport_height / plot->plot_height * y_offs;

        sfVector2f point_pos = { viewport_position.x + viewport_x_offs,
                                 viewport_position.y + viewport_y_offs };
        sfVector2f prev_point_pos = { viewport_position.x + prev_viewport_x_offs,
                                      viewport_position.y + prev_viewport_y_offs };

        if (viewport_y_offs < 0 || viewport_y_offs > plot->viewport_height)
        {
            // current point is outside the viewport

            if (!prev_outside && prev_viewport_x_offs > 0)
            {
                // if previous point is inside the viewport, line must be clipped by viewport border
                
                sfVector2f line_vec = { point_pos.x - prev_point_pos.x, point_pos.y - prev_point_pos.y };
                double clip_factor = 1.0;

                if (viewport_y_offs < 0)
                {
                    // clipped at the top
                    clip_factor = prev_viewport_y_offs / (prev_viewport_y_offs - viewport_y_offs);
                }
                else if (viewport_y_offs > 0)
                {
                    // clipped at the bottom
                    clip_factor = (plot->viewport_height - prev_viewport_y_offs) /
                                  (viewport_y_offs - prev_viewport_y_offs);
                }

                line_vec.x *= clip_factor;
                line_vec.y *= clip_factor;

                point_pos = (sfVector2f){ prev_point_pos.x + line_vec.x, prev_point_pos.y + line_vec.y };

                draw_line(plot, canvas, prev_point_pos, point_pos, 2.0f, sfBlack);
            }
            
            prev_viewport_x_offs = viewport_x_offs;
            prev_viewport_y_offs = viewport_y_offs;
            prev_outside = true;
            continue;
        }

        // current point is inside the viewport

        if (prev_outside)
        {
            // if previous point is outside the viewport, line must be clipped by viewport border

            sfVector2f line_vec = { prev_point_pos.x - point_pos.x, prev_point_pos.y - point_pos.y };
            double clip_factor = 1.0f;

            if (prev_viewport_y_offs < 0)
            {
                // clipped at the top
                clip_factor = viewport_y_offs / (viewport_y_offs - prev_viewport_y_offs);
            }
            else if (prev_viewport_y_offs > 0)
            {
                // clipped at the bottom
                clip_factor = (plot->viewport_height - viewport_y_offs) /
                              (prev_viewport_y_offs - viewport_y_offs);
            }

            line_vec.x *= clip_factor;
            line_vec.y *= clip_factor;

            prev_point_pos = (sfVector2f){ point_pos.x + line_vec.x, point_pos.y + line_vec.y };

            draw_line(plot, canvas, prev_point_pos, point_pos, 2.0f, sfBlack);
        }

        sfCircleShape_setPosition(plot->dot_drawer, point_pos);
        sfRenderWindow_drawCircleShape(canvas, plot->dot_drawer, NULL);

        if (prev_viewport_x_offs > 0 && !prev_outside)
            draw_line(plot, canvas, prev_point_pos, point_pos, 2.0f, sfBlack);

        prev_viewport_x_offs = viewport_x_offs;
        prev_viewport_y_offs = viewport_y_offs;
        prev_outside = false;
    }
}

void plot_deinit(plot_t *plot)
{
    sfRectangleShape_destroy(plot->background);
    sfCircleShape_destroy(plot->dot_drawer);
    sfConvexShape_destroy(plot->line_drawer);
}

static void draw_line(plot_t *plot, sfRenderWindow *window,
                      sfVector2f p1, sfVector2f p2, float thickness, sfColor color)
{
    sfVector2f line_vec = { p2.x - p1.x, p2.y - p1.y };
    float line_vec_len = sqrtf(line_vec.x * line_vec.x + line_vec.y * line_vec.y);

    sfVector2f normal = { -line_vec.y / line_vec_len, line_vec.x / line_vec_len };

    sfVector2f vert1 = { p1.x - normal.x * thickness, p1.y - normal.y * thickness };
    sfVector2f vert2 = { p1.x + normal.x * thickness, p1.y + normal.y * thickness };
    sfVector2f vert3 = { p2.x + normal.x * thickness, p2.y + normal.y * thickness };
    sfVector2f vert4 = { p2.x - normal.x * thickness, p2.y - normal.y * thickness };
    
    sfConvexShape_setFillColor(plot->line_drawer, color);

    sfConvexShape_setPointCount(plot->line_drawer, 4);
    sfConvexShape_setPoint(plot->line_drawer, 0, vert1);
    sfConvexShape_setPoint(plot->line_drawer, 1, vert2);
    sfConvexShape_setPoint(plot->line_drawer, 2, vert3);
    sfConvexShape_setPoint(plot->line_drawer, 3, vert4);

    sfRenderWindow_drawConvexShape(window, plot->line_drawer, NULL);
}

static void draw_arrow(plot_t *plot, sfRenderWindow *window,
                       sfVector2f p1, sfVector2f p2, float thickness, sfColor color)
{
    draw_line(plot, window, p1, p2, thickness, color);

    sfVector2f line_vec = { p2.x - p1.x, p2.y - p1.y };

    float line_vec_len = sqrtf(line_vec.x * line_vec.x + line_vec.y * line_vec.y);
    sfVector2f line_vec_norm = {line_vec.x / line_vec_len, line_vec.y / line_vec_len };

    sfVector2f normal = { -line_vec_norm.y, line_vec_norm.x };

    sfVector2f vert1 = { p2.x - line_vec_norm.x * ARROW_HEIGHT - normal.x * ARROW_WIDTH,
                         p2.y - line_vec_norm.y * ARROW_HEIGHT - normal.y * ARROW_WIDTH };
                         
    sfVector2f vert2 = { p2.x - line_vec_norm.x * ARROW_HEIGHT + normal.x * ARROW_WIDTH,
                         p2.y - line_vec_norm.y * ARROW_HEIGHT + normal.y * ARROW_WIDTH };

    sfConvexShape_setFillColor(plot->line_drawer, color);

    sfConvexShape_setPointCount(plot->line_drawer, 3);
    sfConvexShape_setPoint(plot->line_drawer, 0, p2);
    sfConvexShape_setPoint(plot->line_drawer, 1, vert1);
    sfConvexShape_setPoint(plot->line_drawer, 2, vert2);

    sfRenderWindow_drawConvexShape(window, plot->line_drawer, NULL);
}