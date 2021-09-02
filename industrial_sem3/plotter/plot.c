#include <assert.h>
#include <math.h>
#include "plot.h"

static void draw_line(plot_t *plot, sfRenderWindow *window,
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

    sfCircleShape_setRadius(plot->dot_drawer, 1.0f);
    sfCircleShape_setOrigin(plot->dot_drawer, (sfVector2f){ 1.f, 1.f });
    
    sfCircleShape_setFillColor(plot->dot_drawer, sfBlack);
    sfCircleShape_setPointCount(plot->dot_drawer, 10);

    // Default params

    plot->func = default_plot_func;

    plot_set_viewport_size(plot, 200, 200);
    plot_set_viewport_plot_size(plot, 20.0, 20.0);
    plot_set_viewport_offset(plot, -10.0, 16.0);

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

void plot_set_viewport_size(plot_t *plot, int width, int height)
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

void plot_draw(plot_t *plot, sfRenderWindow *canvas, sfVector2f plot_position)
{
    assert(plot != NULL);
    assert(canvas != NULL);

    sfRectangleShape_setPosition(plot->background, plot_position);
    sfRenderWindow_drawRectangleShape(canvas, plot->background, NULL);

    int prev_pixel_x_offs = -1, prev_pixel_y_offs = -1;

    for (double x_offs = 0; x_offs < plot->plot_width; x_offs += 0.01)
    {
        double x = plot->plot_offset_x + x_offs;
        double y = plot->func(x);

        if (y > plot->plot_offset_y || y < plot->plot_offset_y - plot->plot_height)
        {
            prev_pixel_x_offs = -1;
            prev_pixel_y_offs = -1;
            continue;
        }
        
        double y_offs = plot->plot_offset_y - y;

        int pixel_x_offs = (double)plot->viewport_width  / plot->plot_width  * x_offs;
        int pixel_y_offs = (double)plot->viewport_height / plot->plot_height * y_offs;

        sfVector2f point_pos      = { plot_position.x + pixel_x_offs     , plot_position.y + pixel_y_offs      };
        sfVector2f prev_point_pos = { plot_position.x + prev_pixel_x_offs, plot_position.y + prev_pixel_y_offs };

        sfCircleShape_setPosition(plot->dot_drawer, point_pos);
        sfRenderWindow_drawCircleShape(canvas, plot->dot_drawer, NULL);

        if (prev_pixel_x_offs != -1)
            draw_line(plot, canvas, prev_point_pos, point_pos, 1.0f, sfBlack);

        prev_pixel_x_offs = pixel_x_offs;
        prev_pixel_y_offs = pixel_y_offs;
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