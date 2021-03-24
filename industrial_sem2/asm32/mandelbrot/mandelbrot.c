#include <x86intrin.h>

#include "mandelbrot.h"

sfColor color_function(mandelbrot_cfg_t *cfg, int iteration);

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

void raster_avx_perf(mandelbrot_cfg_t *cfg, sfImage *canvas,
                     int raster_offs_x, int raster_offs_y, int raster_width, int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    float curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
    float curr_plane_y = cfg->origin_y - (cfg->canvas_height / 2 - raster_offs_y) * cfg->scale;

    __m256  const_0123  = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
    __m256  scale_p     = _mm256_set1_ps(cfg->scale);
    __m256  radius_sq_p = _mm256_set1_ps(cfg->radius_sq);
    __m256i one_epi32   = _mm256_set1_epi32(1);
    __m256  one_ps      = _mm256_set1_ps(1.0f);

    // for each point corresponding to a pixel of the canvas
    // we calculate the first member of { z_i } sequence which outside of defined circle
    // the number of a member of the sequence affects the color of the pixel through a certain function

    for (int raster_y = raster_offs_y; raster_y < raster_offs_y + raster_height; raster_y++)
    {
        for (int raster_x = raster_offs_x; raster_x < raster_offs_x + raster_width; raster_x += 8)
        {
            // member number for each point
            __m256i step_cnt = _mm256_setzero_si256();

            __m256 curr_plane_x_p = _mm256_set1_ps(curr_plane_x);
            curr_plane_x_p = _mm256_add_ps(curr_plane_x_p, _mm256_mul_ps(scale_p, const_0123));

            __m256 curr_plane_y_p = _mm256_set1_ps(curr_plane_y);

            // z_0 = current point
            __m256 zx = curr_plane_x_p;
            __m256 zy = curr_plane_y_p;

            for (int step = 0; step < cfg->step_limit; step++)
            {

                __m256 zx2 = _mm256_mul_ps(zx, zx);
                __m256 zy2 = _mm256_mul_ps(zy, zy);

                __m256 cmp = _mm256_add_ps(zx2, zy2);
                cmp = _mm256_cmp_ps(cmp, radius_sq_p, 0x12);
                
                if (!_mm256_movemask_ps(cmp))
                     break;

                __m256i cmpi = _mm256_castps_si256(_mm256_and_ps(cmp, _mm256_castsi256_ps(one_epi32)));
                step_cnt = _mm256_add_epi32(step_cnt, cmpi);
                cmp = _mm256_and_ps(cmp, one_ps);

                // z_i = z_(i-1)^2 + z_0

                __m256 xy = _mm256_mul_ps(zx, zy);

                zx = _mm256_add_ps(_mm256_sub_ps(zx2, zy2), curr_plane_x_p);
                zy = _mm256_add_ps(_mm256_add_ps(xy, xy)  , curr_plane_y_p);
            }

            int *step_cnt_p = (int*)&step_cnt;
            for (int i = 0; i < 8; i++)
                sfImage_setPixel(canvas, raster_x + 8 - i, raster_y, color_function(cfg, step_cnt_p[i]));

            curr_plane_x += cfg->scale * 8;
        }
        
        curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
        curr_plane_y += cfg->scale;
    }
}

void raster_avx_prec(mandelbrot_cfg_t *cfg, sfImage *canvas,
                     int raster_offs_x, int raster_offs_y, int raster_width, int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    double curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
    double curr_plane_y = cfg->origin_y - (cfg->canvas_height / 2 - raster_offs_y) * cfg->scale;

    __m256d  const_0123  = _mm256_set_pd(0.0f, 1.0f, 2.0f, 3.0f);
    __m256d  scale_p     = _mm256_set1_pd(cfg->scale);
    __m256d  radius_sq_p = _mm256_set1_pd(cfg->radius_sq);
    __m256i  one_epi32   = _mm256_set1_epi64x(1);
    __m256d  one_ps      = _mm256_set1_pd(1.0f);

    // for each point corresponding to a pixel of the canvas
    // we calculate the first member of { z_i } sequence which outside of defined circle
    // the number of a member of the sequence affects the color of the pixel through a certain function

    for (int raster_y = raster_offs_y; raster_y < raster_offs_y + raster_height; raster_y++)
    {
        for (int raster_x = raster_offs_x; raster_x < raster_offs_x + raster_width; raster_x += 4)
        {
            // member number for each point
            __m256i step_cnt = _mm256_setzero_si256();

            __m256d curr_plane_x_p = _mm256_set1_pd(curr_plane_x);
            curr_plane_x_p = _mm256_add_pd(curr_plane_x_p, _mm256_mul_pd(scale_p, const_0123));

            __m256d curr_plane_y_p = _mm256_set1_pd(curr_plane_y);

            // z_0 = current point
            __m256d zx = curr_plane_x_p;
            __m256d zy = curr_plane_y_p;

            for (int step = 0; step < cfg->step_limit; step++)
            {

                __m256d zx2 = _mm256_mul_pd(zx, zx);
                __m256d zy2 = _mm256_mul_pd(zy, zy);

                __m256d cmp = _mm256_add_pd(zx2, zy2);
                cmp = _mm256_cmp_pd(cmp, radius_sq_p, 0x12); // less or equal
                
                if (!_mm256_movemask_pd(cmp))
                     break;

                __m256i cmpi = _mm256_castpd_si256(_mm256_and_pd(cmp, _mm256_castsi256_pd(one_epi32)));
                step_cnt = _mm256_add_epi64(step_cnt, cmpi);
                cmp = _mm256_and_pd(cmp, one_ps);

                // z_i = z_(i-1)^2 + z_0

                __m256d xy = _mm256_mul_pd(zx, zy);

                zx = _mm256_add_pd(_mm256_sub_pd(zx2, zy2), curr_plane_x_p);
                zy = _mm256_add_pd(_mm256_add_pd(xy, xy)  , curr_plane_y_p);
            }

            long long *step_cnt_p = (long long*)&step_cnt;
            for (int i = 0; i < 4; i++)
                sfImage_setPixel(canvas, raster_x + 4 - i, raster_y, color_function(cfg, step_cnt_p[i]));

            curr_plane_x += cfg->scale * 4;
        }
        
        curr_plane_x = cfg->origin_x - (cfg->canvas_width  / 2 - raster_offs_x) * cfg->scale;
        curr_plane_y += cfg->scale;
    }
}

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