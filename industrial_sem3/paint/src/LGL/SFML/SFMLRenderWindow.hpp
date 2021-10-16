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
        RenderWindow(const Vec2i &resolution);
        ~RenderWindow();

        /**
         * Displays drawn primitives in the window
         */
        void display();

        /**
         * Checks for a window pending close from a graphics library
         * \return True if window should close
         */
        bool shouldClose();

        /**
         * Resets timer
         * 
         * \return Time in seconds from the last reset
         */
        float timerReset();

        /**
         * \return exactly what you think
         */
        bool isLeftMouseButtonPressed();

        /**
         * \return exactly what you think
         */
        bool isRightMouseButtonPressed();

        /**
         * \return Mouse position relative to the window
         */
        Vec2f getWindowMousePosition();

    private:
        sf::RenderWindow mWindow;
        sf::Clock        mClock;
    };
};

#endif