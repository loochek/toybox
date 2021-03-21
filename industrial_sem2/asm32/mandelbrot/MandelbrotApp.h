#include <SFML/Graphics.hpp>

class MandelbrotApp
{
public:
    MandelbrotApp(const int    canvas_width  = 800,
                  const int    canvas_height = 600,
                  const int    step_limit    = 256,
                  const double scale         = 0.001f,
                  const double origin_x      = -1.0f,
                  const double origin_y      = 0.0f,
                  const double radius        = 10.0f);

    void run();

private:
    void recalc_canvas(int raster_offs_x, int raster_offs_y, int width, int height);
    void recalc_canvas_avx_perf(int raster_offs_x, int raster_offs_y, int width, int height);
    void recalc_canvas_avx_prec(int raster_offs_x, int raster_offs_y, int width, int height);

    sf::Color color_function(int iteration);

    sf::RenderWindow render_window;
    sf::Image        canvas;
    sf::Texture      canvas_texture;
    sf::Sprite       canvas_sprite;

    const int m_canvas_width;
    const int m_canvas_height;
    const int m_step_limit;

    double m_radius_sq;
    double m_scale;
    double m_origin_x;
    double m_origin_y;
};