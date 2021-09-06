#include <stdexcept>
#include <math.h>
#include "SFMLApp.hpp"

static const double MOVEMENT_SPEED = 50.0;
static const double SCALE_SPEED    = 0.5;

static double plotFunc(double x);

SFMLApp::SFMLApp()
{
    window.create(sf::VideoMode(1280, 720), "plotter");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("Roboto-Light.ttf"))
        throw std::runtime_error("Unable to load font");

    plot1.setFont(font);
    plot1.setViewportSize(sf::Vector2f(200, 200));
    plot1.setViewportOrigin(Vec2(0.0, 3.0));
    plot1.setScaleFactor(1 / 20.0);

    plot2.setFont(font);
    plot2.setFunction(plotFunc);
    plot2.setViewportSize(sf::Vector2f(500, 500));
    plot2.setScaleFactor(0.015);

    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 0.0), sf::Vector2f(0.0, 1.0)));
    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 1.0), sf::Vector2f(0.0, 1.0)));
    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 1.0), sf::Vector2f(-0.5, -1.0)));
    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 1.0), sf::Vector2f( 0.5, -1.0)));
    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 2.0), sf::Vector2f(-0.5, -1.0)));
    plot2.addVector(AppliedVector(sf::Vector2f(0.0, 2.0), sf::Vector2f( 0.5, -1.0)));
}

void SFMLApp::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float elapsed = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            plot2.moveViewport(Vec2(0, MOVEMENT_SPEED) * elapsed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            plot2.moveViewport(Vec2(0, -MOVEMENT_SPEED) * elapsed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            plot2.moveViewport(Vec2(MOVEMENT_SPEED, 0) * elapsed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            plot2.moveViewport(Vec2(-MOVEMENT_SPEED, 0) * elapsed);

        double scaleFactor = 1.0 + SCALE_SPEED * elapsed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
            plot2.scale(scaleFactor);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
            plot2.scale(1 / scaleFactor);
        
        window.clear(sf::Color::Red);
        plot1.draw(window, sf::Vector2f(100, 100));
        plot2.draw(window, sf::Vector2f(400, 100));
        window.display();
    }
}

static double plotFunc(double x)
{
    return x * sin(x);
}