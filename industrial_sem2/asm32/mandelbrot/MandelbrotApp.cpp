#include "MandelbrotApp.h"
#include <cstdio>
#include <functional>
#include <x86intrin.h>

const sf::Color color_mapping[] = {
                                    sf::Color(66, 30, 15),
                                    sf::Color(25, 7, 26),
                                    sf::Color(9, 1, 47),
                                    sf::Color(4, 4, 73),
                                    sf::Color(0, 7, 100),
                                    sf::Color(12, 44, 138),
                                    sf::Color(24, 82, 177),
                                    sf::Color(57, 125, 209),
                                    sf::Color(134, 181, 229),
                                    sf::Color(211, 236, 248),
                                    sf::Color(241, 233, 191),
                                    sf::Color(248, 201, 95),
                                    sf::Color(255, 170, 0),
                                    sf::Color(204, 128, 0),
                                    sf::Color(153, 87, 0),
                                    sf::Color(106, 52, 3)
                                  };

sf::Color MandelbrotApp::color_function(int iteration)
{
    if (iteration == m_step_limit)
        return sf::Color::Black;

    return color_mapping[iteration % 16];
}

MandelbrotApp::MandelbrotApp(const int    canvas_width,
                             const int    canvas_height,
                             const int    step_limit,
                             const double scale,
                             const double origin_x,
                             const double origin_y,
                             const double radius):
                m_canvas_width(canvas_width),
                m_canvas_height(canvas_height),
                m_step_limit(step_limit),
                m_scale(scale),
                m_origin_x(origin_x),
                m_origin_y(origin_y),
                m_radius_sq(radius * radius)
{
    render_window.create(sf::VideoMode(canvas_width, canvas_height), "Mandelbrot");
    canvas.create(canvas_width, canvas_height);
    canvas_texture.create(canvas_width, canvas_height);
    canvas_sprite.setPosition(0, 0);
    canvas_sprite.setTexture(canvas_texture);
}

void MandelbrotApp::run()
{
    sf::Clock clock;
    while (render_window.isOpen())
    {
        sf::Event event;
        while (render_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                render_window.close();
        }

        float elapsed = clock.restart().asSeconds();
        printf("FPS: %f\n", 1.0f / elapsed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            m_scale /= 1.01f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            m_scale *= 1.01f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            m_origin_x -= m_scale * 50 * elapsed;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            m_origin_x += m_scale * 50 * elapsed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            m_origin_y -= m_scale * 50 * elapsed;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            m_origin_y += m_scale * 50 * elapsed;

        sf::Thread* thread[16];
        int thread_cnt = 0;
        
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                thread[thread_cnt] = new sf::Thread(std::bind(&MandelbrotApp::recalc_canvas_avx_prec,
                                                              this, m_canvas_width  / 4 * i,
                                                                    m_canvas_height / 4 * j,
                                                                    m_canvas_width  / 4,
                                                                    m_canvas_height / 4));
                thread[thread_cnt]->launch();
                thread_cnt++;                
            }
        }

        for (int i = 0; i < 16; i++)
            thread[i]->wait();

        for (int i = 0; i < 16; i++)
            delete thread[i];

        //recalc_canvas_avx(0, 0, m_canvas_width, m_canvas_height);
        
        canvas_texture.update(canvas);

        render_window.clear();
        render_window.draw(canvas_sprite);
        render_window.display();
    }
}

void MandelbrotApp::recalc_canvas(int raster_offs_x, int raster_offs_y, int raster_width, int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    double curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
    double curr_plane_y = m_origin_y - (m_canvas_height / 2 - raster_offs_y) * m_scale;

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

            for (step_cnt = 0; step_cnt < m_step_limit; step_cnt++)
            {
                double zx2 = zx * zx;
                double zy2 = zy * zy;

                // if z_i outside of the circle
                if (zx2 + zy2 > m_radius_sq)
                    break;

                // z_i = z_(i-1)^2 + z_0

                double xy = zx * zy;

                zx = zx2 - zy2   + curr_plane_x;
                zy = 2 * xy + curr_plane_y;
            }

            canvas.setPixel(raster_x, raster_y, color_function(step_cnt));

            curr_plane_x += m_scale;
        }
        
        curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
        curr_plane_y += m_scale;
    }
}

void MandelbrotApp::recalc_canvas_avx_perf(int raster_offs_x,
                                           int raster_offs_y,
                                           int raster_width,
                                           int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    float curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
    float curr_plane_y = m_origin_y - (m_canvas_height / 2 - raster_offs_y) * m_scale;

    __m256  const_0123  = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
    __m256  scale_p     = _mm256_set1_ps(m_scale);
    __m256  radius_sq_p = _mm256_set1_ps(m_radius_sq);
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

            for (int step = 0; step < m_step_limit; step++)
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
                canvas.setPixel(raster_x + 8 - i, raster_y, color_function(step_cnt_p[i]));

            curr_plane_x += m_scale * 8;
        }
        
        curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
        curr_plane_y += m_scale;
    }
}

void MandelbrotApp::recalc_canvas_avx_prec(int raster_offs_x,
                                           int raster_offs_y,
                                           int raster_width,
                                           int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    double curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
    double curr_plane_y = m_origin_y - (m_canvas_height / 2 - raster_offs_y) * m_scale;

    __m256d  const_0123  = _mm256_set_pd(0.0f, 1.0f, 2.0f, 3.0f);
    __m256d  scale_p     = _mm256_set1_pd(m_scale);
    __m256d  radius_sq_p = _mm256_set1_pd(m_radius_sq);
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

            for (int step = 0; step < m_step_limit; step++)
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
                canvas.setPixel(raster_x + 4 - i, raster_y, color_function(step_cnt_p[i]));

            curr_plane_x += m_scale * 4;
        }
        
        curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
        curr_plane_y += m_scale;
    }
}