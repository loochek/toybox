#ifndef LGL_SFML_RENDER_WINDOW_HPP
#define LGL_SFML_RENDER_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include "SFMLRenderTarget.hpp"

namespace LGL
{
    /**
     * A graphical window that is the render target and can track the mouse and keyboard
     */
    class RenderWindow : public RenderTarget
    {
    public:
        /**
         * \param resolution Window resolution
         */
        RenderWindow(const Vec2i &resolution, const char *title = "LGL (SFML backend)", bool fullscreen = false);
        ~RenderWindow();

        /**
         * Displays drawn primitives in the window
         */
        void display();

        /**
         * Gets event from the graphics library
         * 
         * \return False if no more events are present, true otherwise
         */
        bool pollEvent(Event &event);

        /**
         * Resets timer
         * 
         * \return Time in seconds from the last reset
         */
        float timerReset();

    private:
        sf::RenderWindow mWindow;
        sf::Clock        mClock;
    };
};

#endif