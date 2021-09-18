#include "SFMLGraphics.hpp"

Graphics::Graphics(const Vec2i &resolution)
{
    window.create(sf::VideoMode(resolution.x, resolution.y), "SFML graphics abstraction");
}

Graphics::~Graphics()
{
    window.close();
}

void Graphics::drawCircle(const Vec2f &position, float radius, const Color &color)
{
    circleDrawer.setPosition(toSFMLVector(position));
    circleDrawer.setRadius(radius);
    circleDrawer.setOrigin(sf::Vector2f(radius, radius));
    circleDrawer.setFillColor(toSFMLColor(color));

    window.draw(circleDrawer);
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