#include <SFML/Graphics.hpp>

class MandelbrotApp
{
public:
    MandelbrotApp(const int   canvas_width  = 800,
                  const int   canvas_height = 600,
                  const int   step_limit    = 256,
                  const float scale         = 0.001f,
                  const float origin_x      = -1.0f,
                  const float origin_y      = 0.0f,
                  const float radius        = 10.0f);

    void run();

private:
    void recalc_canvas(int raster_offs_x, int raster_offs_y, int width, int height);
    void recalc_canvas_sse(int raster_offs_x, int raster_offs_y, int width, int height);
    void recalc_canvas_mid(int raster_offs_x, int raster_offs_y, int width, int height);

    sf::RenderWindow render_window;
    sf::Image        canvas;
    sf::Texture      canvas_texture;
    sf::Sprite       canvas_sprite;

    const int m_canvas_width;
    const int m_canvas_height;
    const int m_step_limit;

    float m_radius_sq;
    float m_scale;
    float m_origin_x;
    float m_origin_y;
};