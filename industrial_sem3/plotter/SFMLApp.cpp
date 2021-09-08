#include <stdexcept>
#include <math.h>
#include "SFMLApp.hpp"
#include "Mat3.hpp"

static const double MOVEMENT_SPEED   = 50.0;
static const double SCALE_SPEED      = 0.5;
static const double VEC_ROTATE_SPEED = 2.0;

static double plotFunc(double x);

SFMLApp::SFMLApp()
{
    sf::ContextSettings context;
    context.antialiasingLevel = 8;
    window.create(sf::VideoMode(1280, 720), "plotter", sf::Style::Default, context);
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

    vectorsAngle = 0;
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

        float elapsedTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            plot2.moveViewport(Vec2(0, MOVEMENT_SPEED) * elapsedTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            plot2.moveViewport(Vec2(0, -MOVEMENT_SPEED) * elapsedTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            plot2.moveViewport(Vec2(MOVEMENT_SPEED, 0) * elapsedTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            plot2.moveViewport(Vec2(-MOVEMENT_SPEED, 0) * elapsedTime);

        double scaleFactor = 1.0 + SCALE_SPEED * elapsedTime;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
            plot2.scale(scaleFactor);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
            plot2.scale(1 / scaleFactor);

        handleVectors(elapsedTime);
        
        window.clear(sf::Color::Red);
        plot1.draw(window, sf::Vector2f(100, 100));
        plot2.draw(window, sf::Vector2f(400, 100));
        window.display();
    }
}

void SFMLApp::handleVectors(float elapsedTime)
{
    vectorsAngle += VEC_ROTATE_SPEED * elapsedTime;
    Mat3 transform  = Mat3::rotationMatrix(vectorsAngle);
    Mat3 transform2 = Mat3::translationMatrix(sin(vectorsAngle) * 1.0, sin(vectorsAngle) * 1.0);

    plot2.addVector(AppliedVector(Vec2(0.0, 0.0), transform2 * Vec2(0.0, 0.0)));
    plot2.addVector(AppliedVector(transform2 * Vec2(0.0, 0.0), transform * Vec2(-1.0, -1.0)));
    plot2.addVector(AppliedVector(transform2 * Vec2(0.0, 0.0), transform * Vec2(1.0, -1.0)));
    plot2.addVector(AppliedVector(transform2 * Vec2(0.0, 0.0), transform * Vec2(-1.0, 1.0)));
    plot2.addVector(AppliedVector(transform2 * Vec2(0.0, 0.0), transform * Vec2(1.0, 1.0)));
}

static double plotFunc(double x)
{
    return x * sin(x);
}