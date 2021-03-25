#include "image_blend.h"
#include "x86intrin.h"

void blend(sfImage *output_canvas, blender_input_t *input)
{
    const unsigned char *background = sfImage_getPixelsPtr(input->background); // RGBA
    const unsigned char *foreground = sfImage_getPixelsPtr(input->foreground); // RGBA

    sfVector2u canvas_size     = sfImage_getSize(output_canvas);
    sfVector2u background_size = sfImage_getSize(input->background);
    sfVector2u foreground_size = sfImage_getSize(input->foreground);
    
    int canvas_width = canvas_size.x, canvas_height = canvas_size.y;    
    int background_width = background_size.x, background_height = background_size.y;
    int foreground_width = foreground_size.x, foreground_height = foreground_size.y;

    // copy background image to canvas
    for (int y = 0; y < background_height; y++)
    {
        for (int x = 0; x < background_width; x++)
        {
            int offs = (background_width * y + x) * 4;

            int b_r = background[offs];
            int b_g = background[offs + 1];
            int b_b = background[offs + 2];
            int b_a = background[offs + 3];

            sfImage_setPixel(output_canvas, x, y, (sfColor){ b_r, b_g, b_b, 255 });
        }
    }

    // foreground overlay
    for (int y = 0; y < foreground_height; y++)
    {
        for (int x = 0; x < foreground_width; x++)
        {
            int fg_offs = (foreground_width * y + x) * 4;
            int bg_offs = (background_width * (y + input->foreground_y_offset) + x + input->foreground_x_offset) * 4;

            int b_r = background[bg_offs];
            int b_g = background[bg_offs + 1];
            int b_b = background[bg_offs + 2];
            int b_a = background[bg_offs + 3];

            int f_r = foreground[fg_offs];
            int f_g = foreground[fg_offs + 1];
            int f_b = foreground[fg_offs + 2];
            int f_a = foreground[fg_offs + 3];

            int r_r = (f_r * f_a + b_r * (255 - f_a)) >> 8;
            int r_g = (f_g * f_a + b_g * (255 - f_a)) >> 8;
            int r_b = (f_b * f_a + b_b * (255 - f_a)) >> 8;

            sfImage_setPixel(output_canvas, input->foreground_x_offset + x, input->foreground_y_offset + y, (sfColor){ r_r, r_g, r_b, 255 });
        }
    }
}


void blend_sse(sfImage *output_canvas, blender_input_t *input)
{
    const unsigned char *background = sfImage_getPixelsPtr(input->background); // RGBA
    const unsigned char *foreground = sfImage_getPixelsPtr(input->foreground); // RGBA

    sfVector2u canvas_size     = sfImage_getSize(output_canvas);
    sfVector2u background_size = sfImage_getSize(input->background);
    sfVector2u foreground_size = sfImage_getSize(input->foreground);
    
    int canvas_width = canvas_size.x, canvas_height = canvas_size.y;    
    int background_width = background_size.x, background_height = background_size.y;
    int foreground_width = foreground_size.x, foreground_height = foreground_size.y;

    // copy background image to canvas
    for (int y = 0; y < background_height; y++)
    {
        for (int x = 0; x < background_width; x++)
        {
            int offs = (background_width * y + x) * 4;

            int b_r = background[offs];
            int b_g = background[offs + 1];
            int b_b = background[offs + 2];
            int b_a = background[offs + 3];

            sfImage_setPixel(output_canvas, x, y, (sfColor){ b_r, b_g, b_b, 255 });
        }
    }

    __m128i zero = _mm_setzero_si128();
    __m128i c255 = _mm_set1_epi16(255);
    __m128i csdf = _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0);

    int fg_offs = 0;
    int bg_offs = (background_width * input->foreground_y_offset + input->foreground_x_offset) * 4;

    // foreground overlay
    for (int y = 0; y < foreground_height; y++)
    {
        for (int x = 0; x < foreground_width; x += 4)
        {
            int fg_offs = (foreground_width * y + x) * 4;
            int bg_offs = (background_width * (y + input->foreground_y_offset) + x + input->foreground_x_offset) * 4;
      
            // want to load 4 pixels
            // RGBA RGBA RGBA RGBA
            __m128i fg_pixels = _mm_load_si128((__m128i*)&foreground[fg_offs]);
            __m128i bg_pixels = _mm_load_si128((__m128i*)&background[bg_offs]);

            // split
            __m128i fg_high = (__m128i)_mm_movehl_ps((__m128)zero, (__m128)fg_pixels);
            __m128i fg_low  = (__m128i)_mm_movelh_ps((__m128)fg_pixels, (__m128)zero);

            __m128i bg_high = (__m128i)_mm_movehl_ps((__m128)zero, (__m128)bg_pixels);
            __m128i bg_low  = (__m128i)_mm_movelh_ps((__m128)bg_pixels, (__m128)zero);

            // 0R 0G 0B 0A 0R 0G 0B 0A
            fg_high = _mm_cvtepu8_epi16(fg_high);
            fg_low  = _mm_cvtepu8_epi16(fg_low);

            bg_high = _mm_cvtepu8_epi16(bg_high);
            bg_low  = _mm_cvtepu8_epi16(bg_low);

            __m128i alpha_mask = _mm_set_epi8(0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                              0x80, 6, 0x80, 6, 0x80, 6, 0x80, 6);
            
            __m128i alpha_fg_high = _mm_shuffle_epi8(fg_high, alpha_mask);
            __m128i alpha_fg_low  = _mm_shuffle_epi8(fg_low , alpha_mask);

            __m128i alpha_bg_high = _mm_shuffle_epi8(fg_high, alpha_mask);
            __m128i alpha_bg_low  = _mm_shuffle_epi8(fg_low , alpha_mask);

            alpha_bg_high = _mm_sub_epi16(c255, alpha_bg_high);
            alpha_bg_low  = _mm_sub_epi16(c255, alpha_bg_low );

            fg_high = _mm_mullo_epi16(fg_high, alpha_fg_high);
            fg_low  = _mm_mullo_epi16(fg_low , alpha_fg_low );

            bg_high = _mm_mullo_epi16(bg_high, alpha_bg_high);
            bg_low  = _mm_mullo_epi16(bg_low , alpha_bg_low );

            __m128i result_high = _mm_add_epi16(fg_high, bg_high);
            __m128i result_low  = _mm_add_epi16(fg_low , bg_low );

            __m128i pack_mask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                                15,   13,   11,    9,    7,    5,    3,    1);

            result_high = _mm_shuffle_epi8(result_high, pack_mask);
            result_low  = _mm_shuffle_epi8(result_low , pack_mask);

            __m128i result = (__m128i)_mm_movelh_ps((__m128)result_low, (__m128)result_high);

            unsigned char *result_ptr = (unsigned char*)&result;

            for (int i = 0; i < 4; i++)
            {
                unsigned char r = result_ptr[4 * i + 0];
                unsigned char g = result_ptr[4 * i + 1];
                unsigned char b = result_ptr[4 * i + 2];
                unsigned char a = result_ptr[4 * i + 3];

                sfImage_setPixel(output_canvas,
                                    input->foreground_x_offset + x + i,
                                    input->foreground_y_offset + y,
                                    (sfColor) { r, g, b, 255 });
            }
        }
    }
}