#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "plot.h"

#define MAX_LABEL_LEN 20

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

static const float GRAPH_THICKNESS  = 2.0f;
static const float AXIS_THICKNESS   = 1.0f;
static const float VECTOR_THICKNESS = 1.0f;

static const float LABEL_Y_HORIZ_INDENT     = 7.0f;
static const float LABEL_X_VERT_INDENT      = 1.0f;
static const float LABEL_BOTTOM_VERT_INDENT = -25.0f;
static const float LABEL_LEFT_HORIZ_INDENT  = 7.0f;
static const float LABEL_RIGHT_HORIZ_INDENT = -7.0f;

static const double GRAPH_STEP = 1.0;

static const double DEFAULT_VIEWPORT_SIZE = 200.0;
static const double DEFAULT_SCALE_FACTOR  = 1 / 10.0;

static const int MAX_VEC_COUNT = 128;


static void draw_axis   (plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position);
static void draw_plot   (plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position);
static void draw_vectors(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position);

static void draw_line(plot_t *plot, sfRenderWindow *window,
                      sfVector2f p1, sfVector2f p2, float thickness, sfColor color);

static void draw_arrow(plot_t *plot, sfRenderWindow *window,
                       sfVector2f p1, sfVector2f p2, float thickness, sfColor color);

static double default_plot_func(double x);


lstatus_e plot_init(plot_t *plot, sfFont *font)
{
    assert(plot != NULL);
    assert(font != NULL);

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

    plot->text_drawer = sfText_create();
    if (plot->text_drawer == NULL)
    {
        LSTATUS(LSTATUS_SFML_FAIL, "unable to create SFML object");
        goto error_handler3;
    }

    plot->vectors = calloc(MAX_VEC_COUNT, sizeof(applied_vector_t));
    if (plot->vectors == NULL)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "unable to allocate memory");
        goto error_handler4;
    }

    plot->vec_count = 0;

    // Drawers setup

    sfCircleShape_setRadius(plot->dot_drawer, GRAPH_THICKNESS);
    sfCircleShape_setOrigin(plot->dot_drawer, (sfVector2f){ GRAPH_THICKNESS, GRAPH_THICKNESS });
    
    sfCircleShape_setFillColor(plot->dot_drawer, sfBlack);
    sfCircleShape_setPointCount(plot->dot_drawer, 10);

    sfText_setFont(plot->text_drawer, font);
    sfText_setFillColor(plot->text_drawer, sfBlack);
    sfText_setCharacterSize(plot->text_drawer, 20);

    // Default params

    plot->func = default_plot_func;

    plot_set_viewport_size(plot, DEFAULT_VIEWPORT_SIZE, DEFAULT_VIEWPORT_SIZE);
    plot_set_viewport_origin(plot, 0, 0);
    plot->scale_factor = DEFAULT_SCALE_FACTOR;

    return LSTATUS_OK;

error_handler4:
    sfText_destroy(plot->text_drawer);

error_handler3:
    sfConvexShape_destroy(plot->line_drawer);

error_handler2:
    sfCircleShape_destroy(plot->dot_drawer);

error_handler1:
    sfRectangleShape_destroy(plot->background);

error_handler0:
    return status;
}

void plot_set_viewport_origin(plot_t *plot, double x, double y)
{
    assert(plot != NULL);

    plot->plot_origin_x = x;
    plot->plot_origin_y = y;
}

void plot_set_viewport_size(plot_t *plot, double width, double height)
{
    assert(plot != NULL);

    plot->viewport_width  = width;
    plot->viewport_height = height;

    sfRectangleShape_setSize(plot->background, (sfVector2f){ plot->viewport_width, plot->viewport_height });
}

void plot_set_scale_factor(plot_t *plot, double scale_factor)
{
    assert(plot != NULL);

    plot->scale_factor = scale_factor;
}

void plot_set_func(plot_t *plot, plot_func_t func)
{
    assert(plot != NULL);

    plot->func = func;
}

void plot_move_viewport(plot_t *plot, double x_offs, double y_offs)
{
    assert(plot != NULL);

    plot->plot_origin_x += x_offs * plot->scale_factor;
    plot->plot_origin_y += y_offs * plot->scale_factor;
}

void plot_scale(plot_t *plot, double scale)
{
    assert(plot != NULL);

    plot->scale_factor *= scale;
}

void plot_add_vector(plot_t *plot, applied_vector_t vec)
{
    assert(plot != NULL);

    if (plot->vec_count >= MAX_VEC_COUNT)
        return;

    plot->vectors[plot->vec_count++] = vec;
}

void plot_draw(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position)
{
    assert(plot != NULL);
    assert(canvas != NULL);

    sfRectangleShape_setPosition(plot->background, viewport_position);
    sfRenderWindow_drawRectangleShape(canvas, plot->background, NULL);

    draw_axis   (plot, canvas, viewport_position);
    draw_plot   (plot, canvas, viewport_position);
    draw_vectors(plot, canvas, viewport_position);
}

void plot_deinit(plot_t *plot)
{
    sfRectangleShape_destroy(plot->background);
    sfCircleShape_destroy(plot->dot_drawer);
    sfConvexShape_destroy(plot->line_drawer);
    sfText_destroy(plot->text_drawer);
    free(plot->vectors);
}


static void draw_axis(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position)
{
    /// Height of a plot fragment visible through viewport
    double plot_height = plot->viewport_height * plot->scale_factor;
    /// Width of a plot fragment visible through viewport
    double plot_width = plot->viewport_width * plot->scale_factor;
    /// Viewport left up corner X coord
    double plot_offset_x = plot->plot_origin_x - plot_width / 2.0;
    /// Viewport left up corner Y coord
    double plot_offset_y = plot->plot_origin_y + plot_height / 2.0;

    char buf[MAX_LABEL_LEN + 1] = {0};

    double viewport_x_axis_offs = (double)plot->viewport_height / plot_height * plot_offset_y;
    
    if (viewport_x_axis_offs > 0 && viewport_x_axis_offs < plot->viewport_height)
    {
        sfVector2f x_axis_begin = { viewport_position.x,
                                    viewport_position.y + viewport_x_axis_offs };
        sfVector2f x_axis_end   = { viewport_position.x + plot->viewport_width,
                                    viewport_position.y + viewport_x_axis_offs };

        draw_arrow(plot, canvas, x_axis_begin, x_axis_end, AXIS_THICKNESS, sfBlack);

        // Right X label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)(plot_offset_x + plot_width));
        sfText_setString(plot->text_drawer, buf);

        sfFloatRect text_bound = sfText_getLocalBounds(plot->text_drawer);

        sfVector2f right_label_pos = {
            viewport_position.x + plot->viewport_width - text_bound.width + LABEL_RIGHT_HORIZ_INDENT,
            viewport_position.y + viewport_x_axis_offs + LABEL_X_VERT_INDENT };

        sfText_setPosition(plot->text_drawer, right_label_pos);
        sfRenderWindow_drawText(canvas, plot->text_drawer, NULL);

        // Left X label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)plot_offset_x);
        sfText_setString(plot->text_drawer, buf);

        sfVector2f left_label_pos = { viewport_position.x + LABEL_LEFT_HORIZ_INDENT,
                                      viewport_position.y + viewport_x_axis_offs + LABEL_X_VERT_INDENT };
        
        sfText_setPosition(plot->text_drawer, left_label_pos);
        sfRenderWindow_drawText(canvas, plot->text_drawer, NULL);
    }

    double viewport_y_axis_offs = (double)plot->viewport_width / plot_width * (-plot_offset_x);
    
    if (viewport_y_axis_offs > 0 && viewport_y_axis_offs < plot->viewport_width)
    {
        sfVector2f y_axis_begin = { viewport_position.x + viewport_y_axis_offs,
                                    viewport_position.y + plot->viewport_height};
        sfVector2f y_axis_end   = { viewport_position.x + viewport_y_axis_offs,
                                    viewport_position.y };

        draw_arrow(plot, canvas, y_axis_begin, y_axis_end, AXIS_THICKNESS, sfBlack);

        char buf[20] = {0};

        // Top Y label

        snprintf(buf, 20, "%d", (int)plot_offset_y);
        sfText_setString(plot->text_drawer, buf);

        sfVector2f top_label_pos = { viewport_position.x + viewport_y_axis_offs + LABEL_Y_HORIZ_INDENT,
                                     viewport_position.y };

        sfText_setPosition(plot->text_drawer, top_label_pos);
        sfRenderWindow_drawText(canvas, plot->text_drawer, NULL);

        // Bottom Y label

        snprintf(buf, 20, "%d", (int)(plot_offset_y - plot_height));
        sfText_setString(plot->text_drawer, buf);

        sfVector2f bottom_label_pos = { viewport_position.x + viewport_y_axis_offs + LABEL_Y_HORIZ_INDENT,
                                        viewport_position.y + plot->viewport_height + LABEL_BOTTOM_VERT_INDENT};

        sfText_setPosition(plot->text_drawer, bottom_label_pos);
        sfRenderWindow_drawText(canvas, plot->text_drawer, NULL);
    }
}

static void draw_plot(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position)
{
    /// Height of a plot fragment visible through viewport
    double plot_height = plot->viewport_height * plot->scale_factor;
    /// Width of a plot fragment visible through viewport
    double plot_width = plot->viewport_width * plot->scale_factor;
    /// Viewport left up corner X coord
    double plot_offset_x = plot->plot_origin_x - plot_width / 2.0;
    /// Viewport left up corner Y coord
    double plot_offset_y = plot->plot_origin_y + plot_height / 2.0;

    double prev_viewport_x_offs = -1.0, prev_viewport_y_offs = -1.0;
    bool prev_outside = false;

    bool must_stop = false;

    for (double x_offs = 0; !must_stop; x_offs += GRAPH_STEP * plot->scale_factor)
    {
        if (x_offs >= plot_width)
        {
            x_offs = plot_width;
            must_stop = true;
        }

        double x = plot_offset_x + x_offs;
        double y = plot->func(x);

        double y_offs = plot_offset_y - y;

        double viewport_x_offs = (double)plot->viewport_width  / plot_width  * x_offs;
        double viewport_y_offs = (double)plot->viewport_height / plot_height * y_offs;

        sfVector2f point_pos = { viewport_position.x + viewport_x_offs,
                                 viewport_position.y + viewport_y_offs };
        sfVector2f prev_point_pos = { viewport_position.x + prev_viewport_x_offs,
                                      viewport_position.y + prev_viewport_y_offs };

        if (viewport_y_offs < 0 || viewport_y_offs > plot->viewport_height)
        {
            // current point is outside the viewport

            if (!prev_outside && prev_viewport_x_offs > -0.5)
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

                draw_line(plot, canvas, prev_point_pos, point_pos, GRAPH_THICKNESS, sfBlack);
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

            draw_line(plot, canvas, prev_point_pos, point_pos, GRAPH_THICKNESS, sfBlack);
        }

        sfCircleShape_setPosition(plot->dot_drawer, point_pos);
        sfRenderWindow_drawCircleShape(canvas, plot->dot_drawer, NULL);

        if (prev_viewport_x_offs > -0.5 && !prev_outside)
           draw_line(plot, canvas, prev_point_pos, point_pos, GRAPH_THICKNESS, sfBlack);

        prev_viewport_x_offs = viewport_x_offs;
        prev_viewport_y_offs = viewport_y_offs;
        prev_outside = false;
    }
}

static void draw_vectors(plot_t *plot, sfRenderWindow *canvas, sfVector2f viewport_position)
{
    /// Height of a plot fragment visible through viewport
    double plot_height = plot->viewport_height * plot->scale_factor;
    /// Width of a plot fragment visible through viewport
    double plot_width = plot->viewport_width * plot->scale_factor;
    /// Viewport left up corner X coord
    double plot_offset_x = plot->plot_origin_x - plot_width / 2.0;
    /// Viewport left up corner Y coord
    double plot_offset_y = plot->plot_origin_y + plot_height / 2.0;

    for (int i = 0; i < plot->vec_count; i++)
    {
        applied_vector_t curr_vec = plot->vectors[i];

        sfVector2f vec_start = curr_vec.origin;
        sfVector2f vec_end   = { vec_start.x + curr_vec.vector.x,
                                 vec_start.y + curr_vec.vector.y };

        double start_x_offs = vec_start.x - plot_offset_x;
        double start_y_offs = plot_offset_y - vec_start.y;

        double end_x_offs = vec_end.x - plot_offset_x;
        double end_y_offs = plot_offset_y - vec_end.y;

        double start_viewport_x_offs = (double)plot->viewport_width  / plot_width  * start_x_offs;
        double start_viewport_y_offs = (double)plot->viewport_height / plot_height * start_y_offs;

        double end_viewport_x_offs = (double)plot->viewport_width  / plot_width  * end_x_offs;
        double end_viewport_y_offs = (double)plot->viewport_height / plot_height * end_y_offs;

        if (start_viewport_x_offs < 0 || start_viewport_x_offs > plot->viewport_width ||
            start_viewport_y_offs < 0 || start_viewport_y_offs > plot->viewport_height ||
            end_viewport_x_offs < 0   || end_viewport_x_offs   > plot->viewport_width ||
            end_viewport_y_offs < 0   || end_viewport_y_offs   > plot->viewport_height)
        {
            continue;
        }

        sfVector2f vec_start_viewport = { viewport_position.x + start_viewport_x_offs,
                                          viewport_position.y + start_viewport_y_offs };
        sfVector2f vec_end_viewport   = { viewport_position.x + end_viewport_x_offs,
                                          viewport_position.y + end_viewport_y_offs };

        draw_arrow(plot, canvas, vec_start_viewport, vec_end_viewport, VECTOR_THICKNESS, sfCyan);
    }
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

static double default_plot_func(double x)
{
    return x * x;
}