#include <SFML/Graphics.h>

/**
 * Rasterization config
 */
typedef struct
{
    /**
     * raster canvas width
     */
    int canvas_width;
    /**
     * raster canvas height
     */
    int canvas_height;

    /**
     * step after which the point is considered to be forever enclosed in a circle
     */
    int step_limit; 

    /**
     * the square of the radius of the bounding circle
     */
    double radius_sq;

    /**
     * X coordinate on the complex plane corresponding to the center of the raster
     */
    double origin_x; 

    /**
     * Y coordinate on the complex plane corresponding to the center of the raster
     */
    double origin_y;

    /**
     * raster step on the complex plane
     */
    double scale;
    
} mandelbrot_cfg_t;

/**
 * Basic rasterizer
 */
void raster(mandelbrot_cfg_t *cfg, sfImage *canvas,
            int raster_offs_x, int raster_offs_y, int raster_width, int raster_height);