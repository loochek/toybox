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

    bool RenderWindow::shouldClose()
    {
        sf::Event event;
        while (mWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                return true;
        }

        return false;
    }

    float RenderWindow::timerReset()
    {
        return mClock.restart().asSeconds();
    }

    bool RenderWindow::isLeftMouseButtonPressed()
    {
        return sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }

    bool RenderWindow::isRightMouseButtonPressed()
    {
        return sf::Mouse::isButtonPressed(sf::Mouse::Right);
    }

    Vec2f RenderWindow::getWindowMousePosition()
    {
        return fromSFMLVector(mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow)));
    }
};