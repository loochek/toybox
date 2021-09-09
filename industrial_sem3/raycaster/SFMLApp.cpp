#include <stdexcept>
#include "SFMLApp.hpp"
#include "Vec3.hpp"

SFMLApp::SFMLApp() : raycaster(Raycaster(sf::Vector2f(0, 0)))
{
    // sf::ContextSettings context;
    // context.antialiasingLevel = 8;
    // window.create(sf::VideoMode(1280, 720), "raycaster", sf::Style::Default, context);
    window.create(sf::VideoMode(640, 640), "raycaster");
    window.setFramerateLimit(60);
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
        
        window.clear(sf::Color::Green);
        raycaster.draw(window, elapsedTime);
        window.display();
    }
}