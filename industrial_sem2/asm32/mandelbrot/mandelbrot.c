#include "mandelbrot.h"

const sfColor color_map[] = {
                                { 66 , 30 , 15 , 255 }, 
                                { 25 , 7  , 26 , 255 }, 
                                { 9  , 1  , 47 , 255 }, 
                                { 4  , 4  , 73 , 255 }, 
                                { 0  , 7  , 100, 255 }, 
                                { 12 , 44 , 138, 255 }, 
                                { 24 , 82 , 177, 255 }, 
                                { 57 , 125, 209, 255 }, 
                                { 134, 181, 229, 255 }, 
                                { 211, 236, 248, 255 }, 
                                { 241, 233, 191, 255 }, 
                                { 248, 201, 95 , 255 }, 
                                { 255, 170, 0  , 255 }, 
                                { 204, 128, 0  , 255 }, 
                                { 153, 87 , 0  , 255 }, 
                                { 106, 52 , 3  , 255 }
                            };

sfColor color_function(mandelbrot_cfg_t *cfg, int iteration)
{
    if (iteration == cfg->step_limit)
        return sfBlack;

    return color_map[iteration % 16];
}

void raster(mandelbrot_cfg_t *cfg, sfImage *canvas,
                   int raster_offs_x, int raster_offs_y, int raster_width, int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    double curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
    double curr_plane_y = cfg->origin_y - (cfg->canvas_height / 2 - raster_offs_y) * cfg->scale;

    // for each point corresponding to a pixel of the canvas
    // we calculate the first member of { z_i } sequence which outside of defined circle
    // the number of a member of the sequence affects the color of the pixel through a certain function

    for (int raster_y = raster_offs_y; raster_y < raster_offs_y + raster_height; raster_y++)
    {
        for (int raster_x = raster_offs_x; raster_x < raster_offs_x + raster_width; raster_x++)
        {
            // member number
            int step_cnt = 0;
            // z_0 = current point
            double zx = curr_plane_x, zy = curr_plane_y;

            for (step_cnt = 0; step_cnt < cfg->step_limit; step_cnt++)
            {
                double zx2 = zx * zx;
                double zy2 = zy * zy;

                // if z_i outside of the circle
                if (zx2 + zy2 > cfg->radius_sq)
                    break;

                // z_i = z_(i-1)^2 + z_0

                double xy = zx * zy;

                zx = zx2 - zy2   + curr_plane_x;
                zy = 2 * xy + curr_plane_y;
            }

            sfImage_setPixel(canvas, raster_x, raster_y, color_function(cfg, step_cnt));

            curr_plane_x += cfg->scale;
        }
        
        curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
        curr_plane_y += cfg->scale;
    }
}