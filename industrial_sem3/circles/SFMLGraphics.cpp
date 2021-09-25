#include "SFMLGraphics.hpp"

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

Graphics::Graphics(const Vec2i &resolution)
{
    window.create(sf::VideoMode(resolution.x, resolution.y), "SFML graphics abstraction");
}

Graphics::~Graphics()
{
    window.close();
}

void Graphics::drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, const Color &color)
{
    polygonDrawer.setFillColor(toSFMLColor(color));

    polygonDrawer.setPointCount(4);
    polygonDrawer.setPoint(0, toSFMLVector(p1));
    polygonDrawer.setPoint(1, toSFMLVector(p2));
    polygonDrawer.setPoint(2, toSFMLVector(p3));
    polygonDrawer.setPoint(3, toSFMLVector(p4));

    window.draw(polygonDrawer);
}

void Graphics::drawCircle(const Vec2f &position, float radius, const Color &color)
{
    circleDrawer.setPosition(toSFMLVector(position));
    circleDrawer.setRadius(radius);
    circleDrawer.setOrigin(sf::Vector2f(radius, radius));
    circleDrawer.setFillColor(toSFMLColor(color));

    window.draw(circleDrawer);
}

void Graphics::drawLine(const Vec2f &p1, const Vec2f &p2, float thickness, const Color &color)
{
    Vec2f line_vec = p2 - p1;
    Vec2f normal = Vec2f(-line_vec.y, line_vec.x) / line_vec.length();

    Vec2f vert1 = p1 - normal * thickness;
    Vec2f vert2 = p1 + normal * thickness;
    Vec2f vert3 = p2 + normal * thickness;
    Vec2f vert4 = p2 - normal * thickness;
    
    polygonDrawer.setFillColor(toSFMLColor(color));

    polygonDrawer.setPointCount(4);
    polygonDrawer.setPoint(0, toSFMLVector(vert1));
    polygonDrawer.setPoint(1, toSFMLVector(vert2));
    polygonDrawer.setPoint(2, toSFMLVector(vert3));
    polygonDrawer.setPoint(3, toSFMLVector(vert4));

    window.draw(polygonDrawer);
}

void Graphics::drawArrow(const Vec2f &start, const Vec2f &end, float thickness, const Color &color)
{
    drawLine(start, end, thickness, color);

    Vec2f line_vec_norm = (end - start).normalized();
    Vec2f normal = Vec2f(-line_vec_norm.y, line_vec_norm.x);

    Vec2f vert1 = end - line_vec_norm * ARROW_HEIGHT - normal * ARROW_WIDTH;
    Vec2f vert2 = end - line_vec_norm * ARROW_HEIGHT + normal * ARROW_WIDTH;

    polygonDrawer.setFillColor(toSFMLColor(color));

    polygonDrawer.setPointCount(3);
    polygonDrawer.setPoint(0, toSFMLVector(end));
    polygonDrawer.setPoint(1, toSFMLVector(vert1));
    polygonDrawer.setPoint(2, toSFMLVector(vert2));

    window.draw(polygonDrawer);
}

void Graphics::clear(const Color &clearColor)
{
    window.clear(toSFMLColor(clearColor));
}

void Graphics::display()
{
    window.display();
}

sf::Vector2f Graphics::toSFMLVector(const Vec2f &vector)
{
    return sf::Vector2f(vector.x, vector.y);
}

sf::Color Graphics::toSFMLColor(const Color &color)
{
    return sf::Color(color.x * 255, color.y * 255, color.z * 255);
}

bool Graphics::shouldClose()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return true;
    }

    return false;
}

float Graphics::timerReset()
{
    return clock.restart().asSeconds();
}