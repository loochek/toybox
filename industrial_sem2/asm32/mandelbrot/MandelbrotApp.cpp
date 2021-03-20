#include "MandelbrotApp.h"
#include <cstdio>
#include <functional>
#include <x86intrin.h>

MandelbrotApp::MandelbrotApp(const int   canvas_width,
                             const int   canvas_height,
                             const int   step_limit,
                             const float scale,
                             const float origin_x,
                             const float origin_y,
                             const float radius):
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

        sf::Time elapsed = clock.restart();
        printf("FPS: %f\n", 1000.f / elapsed.asMilliseconds());

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            m_scale -= 0.00001f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            m_scale += 0.00001f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            m_origin_x -= m_scale * 5;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            m_origin_x += m_scale * 5;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            m_origin_y -= m_scale * 5;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            m_origin_y += m_scale * 5;

        // sf::Thread* thread[16];
        // int thread_cnt = 0;
        
        // for (int i = 0; i < 4; i++)
        // {
        //     for (int j = 0; j < 4; j++)
        //     {
        //         thread[thread_cnt] = new sf::Thread(std::bind(&MandelbrotApp::recalc_canvas,
        //                                                       this, 200 * i, 150 * j, 200, 150));
        //         thread[thread_cnt]->launch();
        //         thread_cnt++;                
        //     }
        // }

        // for (int i = 0; i < 16; i++)
        //     thread[i]->wait();

        // for (int i = 0; i < 16; i++)
        //     delete thread[i];

        recalc_canvas_sse(0, 0, m_canvas_width, m_canvas_height);
        
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
    float curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
    float curr_plane_y = m_origin_y - (m_canvas_height / 2 - raster_offs_y) * m_scale;

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
            float zx = curr_plane_x, zy = curr_plane_y;

            for (step_cnt = 0; step_cnt < m_step_limit; step_cnt++)
            {
                float zx2 = zx * zx;
                float zy2 = zy * zy;

                // if z_i outside of the circle
                if (zx2 + zy2 > m_radius_sq)
                    break;

                // z_i = z_(i-1)^2 + z_0
                float new_zx = zx2 - zy2   + curr_plane_x;
                float new_zy = 2 * zx * zy + curr_plane_y;
                zx = new_zx;
                zy = new_zy;
            }

            canvas.setPixel(raster_x, raster_y, sf::Color(step_cnt, step_cnt, step_cnt));

            curr_plane_x += m_scale;
        }
        
        curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
        curr_plane_y += m_scale;
    }
}

void MandelbrotApp::recalc_canvas_sse(int raster_offs_x, int raster_offs_y, int raster_width, int raster_height)
{
    // raster canvas covers the complex plane depending on the scale

    // plane coords of (raster_offs_x, raster_offs_y) raster pixel
    float curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
    float curr_plane_y = m_origin_y - (m_canvas_height / 2 - raster_offs_y) * m_scale;

    __m128  const_3210  = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);
    __m128  scale_p     = _mm_set_ps1(m_scale);
    __m128  radius_sq_p = _mm_set_ps1(m_radius_sq);
    __m128i one_pi      = _mm_set1_epi32(1);
    __m128  one_ps      = _mm_set_ps1(1.0f);

    // for each point corresponding to a pixel of the canvas
    // we calculate the first member of { z_i } sequence which outside of defined circle
    // the number of a member of the sequence affects the color of the pixel through a certain function

    for (int raster_y = raster_offs_y; raster_y < raster_offs_y + raster_height; raster_y++)
    {
        for (int raster_x = raster_offs_x; raster_x < raster_offs_x + raster_width; raster_x += 4)
        {
            // member number for each point
            __m128i step_cnt = _mm_setzero_si128();

            __m128 curr_plane_x_p = _mm_set_ps1(curr_plane_x);
            __m128 curr_plane_y_p = _mm_set_ps1(curr_plane_y);

            // z_0 = current point
            __m128 zx = _mm_mul_ps(scale_p, const_3210);
            __m128 zy = curr_plane_y_p;
            zx = _mm_add_ps(zx, curr_plane_x_p);

            for (int step = 0; step < m_step_limit; step++)
            {

                __m128 zx2 = _mm_mul_ps(zx, zx);
                __m128 zy2 = _mm_mul_ps(zy, zy);

                __m128 cmp = _mm_add_ps(zx2, zy2);
                cmp = _mm_cmple_ps(cmp, radius_sq_p);
                
                //if (!_mm_movemask_ps(cmp))
                //    break;

                __m128i cmpi = _mm_castps_si128(_mm_and_ps(cmp, _mm_castsi128_ps(one_pi)));
                cmp = _mm_and_ps(cmp, one_ps);

                step_cnt = _mm_add_epi32(step_cnt, cmpi);

                // z_i = z_(i-1)^2 + z_0

                __m128 xy = _mm_mul_ps(zx, zy);

                zx = _mm_add_ps(_mm_sub_ps(zx2, zy2), curr_plane_x_p);
                zy = _mm_add_ps(_mm_add_ps(xy, xy)  , curr_plane_y_p);
            }


            int st_cnt1 = _mm_extract_epi32(step_cnt, 0);
            int st_cnt2 = _mm_extract_epi32(step_cnt, 1);
            int st_cnt3 = _mm_extract_epi32(step_cnt, 2);
            int st_cnt4 = _mm_extract_epi32(step_cnt, 3);
            canvas.setPixel(raster_x    , raster_y, sf::Color(st_cnt1, st_cnt1, st_cnt1));
            canvas.setPixel(raster_x + 1, raster_y, sf::Color(st_cnt2, st_cnt2, st_cnt2));
            canvas.setPixel(raster_x + 2, raster_y, sf::Color(st_cnt3, st_cnt3, st_cnt3));
            canvas.setPixel(raster_x + 3, raster_y, sf::Color(st_cnt4, st_cnt4, st_cnt4));
            
            curr_plane_x += m_scale * 4;
        }
        
        curr_plane_x = m_origin_x - (m_canvas_width  / 2 - raster_offs_x) * m_scale;
        curr_plane_y += m_scale;
    }
}