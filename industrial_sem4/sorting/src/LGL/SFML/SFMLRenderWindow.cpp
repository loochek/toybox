#include "SFMLRenderWindow.hpp"

namespace LGL
{
    RenderWindow::RenderWindow(const Vec2i &resolution, const char *title, bool fullscreen) :
        RenderTarget(&mWindow)
    {
        mWindow.create(sf::VideoMode(resolution.x, resolution.y), title,
                       fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
        mWindow.setVerticalSyncEnabled(true);
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
            event.key.code     = (KeyboardKey)sfmlEvent.key.code;
            event.key.modifier = createModifier(sfmlEvent.key.control, sfmlEvent.key.alt, sfmlEvent.key.shift);

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

            event.mouseWheelScroll.delta    = sfmlEvent.mouseWheelScroll.delta;
            event.mouseWheelScroll.position = Vec2i(sfmlEvent.mouseWheelScroll.x,
                                                    sfmlEvent.mouseWheelScroll.y);
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
                
            event.mouseButton.position = Vec2i(sfmlEvent.mouseButton.x,
                                               sfmlEvent.mouseButton.y);
            
            if (sfmlEvent.type == sf::Event::MouseButtonPressed)
                event.type = Event::MouseButtonPressed;
            else
                event.type = Event::MouseButtonReleased;
        
            break;

        case sf::Event::MouseMoved:
            event.type = Event::MouseMoved;
            event.mouseMove.position = Vec2i(sfmlEvent.mouseMove.x,
                                             sfmlEvent.mouseMove.y);
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