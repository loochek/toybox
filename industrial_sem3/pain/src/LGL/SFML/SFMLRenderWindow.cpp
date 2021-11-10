#include "SFMLRenderWindow.hpp"

namespace LGL
{
    RenderWindow::RenderWindow(const Vec2i &resolution, const char *title) : RenderTarget(&mWindow)
    {
        mWindow.create(sf::VideoMode(resolution.x, resolution.y), title);
        //mWindow.setFramerateLimit(60);
    }

    RenderWindow::~RenderWindow()
    {
        mWindow.close();
    }

    void RenderWindow::display()
    {
        mWindow.display();
    }

    bool RenderWindow::pollEvent(Event &event)
    {
        sf::Event sfmlEvent;
        if (!mWindow.pollEvent(sfmlEvent))
            return false;

        // SFML event is present

        switch (sfmlEvent.type)
        {
        case sf::Event::Closed:
            event.type = Event::Closed;
            break;

        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
            event.key.code    = (KeyboardKey)sfmlEvent.key.code;
            event.key.alt     = sfmlEvent.key.alt;
            event.key.control = sfmlEvent.key.control;
            event.key.shift   = sfmlEvent.key.shift;

            if (sfmlEvent.type == sf::Event::KeyPressed)
                event.type = Event::KeyPressed;
            else
                event.type = Event::KeyReleased;

            break;

        case sf::Event::MouseWheelScrolled:
            if (sfmlEvent.mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)
            {
                event.type = Event::Dummy;
                break;
            }

            event.mouseWheelScroll.delta = sfmlEvent.mouseWheelScroll.delta;
            event.mouseWheelScroll.x     = sfmlEvent.mouseWheelScroll.x;
            event.mouseWheelScroll.y     = sfmlEvent.mouseWheelScroll.y;
            break;

        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
            if (sfmlEvent.mouseButton.button == sf::Mouse::Button::Left)
                event.mouseButton.button = MouseButton::Left;
            else if (sfmlEvent.mouseButton.button == sf::Mouse::Button::Right)
                event.mouseButton.button = MouseButton::Right;
            else
            {
                event.type = Event::Dummy;
                break;
            }
                
            event.mouseButton.x = sfmlEvent.mouseButton.x;
            event.mouseButton.y = sfmlEvent.mouseButton.y;
            
            if (sfmlEvent.type == sf::Event::MouseButtonPressed)
                event.type = Event::MouseButtonPressed;
            else
                event.type = Event::MouseButtonReleased;
        
            break;

        case sf::Event::MouseMoved:
            event.type = Event::MouseMoved;
            event.mouseMove.x = sfmlEvent.mouseMove.x;
            event.mouseMove.y = sfmlEvent.mouseMove.y;
            break;

        default:
            event.type = Event::Dummy;
            break;
        }

        return true;
    }

    float RenderWindow::timerReset()
    {
        return mClock.restart().asSeconds();
    }
};