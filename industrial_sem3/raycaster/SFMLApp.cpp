#include <stdexcept>
#include "SFMLApp.hpp"
#include "Vec3.hpp"

const int WINDOW_WIDTH  = 1920;
const int WINDOW_HEIGHT = 1080;

const float MOVEMENT_SPEED = 1.0f;

SFMLApp::SFMLApp()
{
    // sf::ContextSettings context;
    // context.antialiasingLevel = 8;
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "raycaster", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    raycaster.setCanvasSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
}

void SFMLApp::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float elapsedTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            raycaster.moveCamera(Vec3f(0.0f, MOVEMENT_SPEED * elapsedTime, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            raycaster.moveCamera(Vec3f(0.0f, -MOVEMENT_SPEED * elapsedTime, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            raycaster.moveCamera(Vec3f(MOVEMENT_SPEED * elapsedTime, 0.0f, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            raycaster.moveCamera(Vec3f(-MOVEMENT_SPEED * elapsedTime, 0.0f, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            raycaster.moveCamera(Vec3f(0.0f, 0.0f, MOVEMENT_SPEED * elapsedTime));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            raycaster.moveCamera(Vec3f(0.0f, 0.0f, -MOVEMENT_SPEED * elapsedTime));

        raycaster.update(elapsedTime);
        
        window.clear(sf::Color::Green);
        raycaster.draw(window);
        window.display();
    }
}