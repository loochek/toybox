#ifndef LGL_SFML_RENDER_TARGET_HPP
#define LGL_SFML_RENDER_TARGET_HPP

#include <SFML/Graphics.hpp>
#include "SFMLCommon.hpp"

namespace LGL
{
    class RenderTexture;
    
    /**
     * Abstract surface for drawing
     */
    class RenderTarget
    {
    public:
        RenderTarget() = delete;

        /**
         * Draws a triangle by points
         * 
         * \param p1 Point 1
         * \param p2 Point 2
         * \param p3 Point 3
         * \param color Triangle color
         */
        void drawTriangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3,
                        const Color &color = Color(1.0f, 1.0f, 1.0f));
        
        /**
         * Draws a quad by points
         * 
         * \param p1 Point 1
         * \param p2 Point 2
         * \param p3 Point 3
         * \param p4 Point 4
         * \param color Quad color
         */
        void drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4,
                    const Color &color = Color(1.0f, 1.0f, 1.0f));

        /**
         * Draws a rect
         * 
         * \param rect Rect
         * \param color Rect color
         */
        void drawRect(const FloatRect &rect, const Color &color = Color(1.0f, 1.0f, 1.0f));

        /**
         * Draws a circle
         * 
         * \param position Circle position
         * \param radius Circle radius
         * \param color Circle color
         */
        void drawCircle(const Vec2f &position, float radius = 1.0f,
                        const Color &color = Color(1.0f, 1.0f, 1.0f));

        /**
         * Draws a line segment specified by two points
         * 
         * \param p1 First point
         * \param p2 Second point
         * \param thickness Thickness
         * \param color Color
         */
        void drawLine(const Vec2f &p1, const Vec2f &p2, float thickness = 1.0f,
                    const Color &color = Color(1.0f, 1.0f, 1.0f));

        /**
         * Draws an arrow specified by two points
         * 
         * \param start Start point
         * \param end End point
         * \param thickness Thickness
         * \param color Color
         */
        void drawArrow(const Vec2f &start, const Vec2f &end, float thickness = 1.0f,
                    const Color &color = Color(1.0f, 1.0f, 1.0f));

        /**
         * Draws the render texture with specified viewport position. 
         * Viewport size is always same with render texture resolution to avoid blurriness
         * 
         * \param texture Texture to draw
         * \param position Position of left top corner of the part
         * \param viewportPosition Position of the viewport to be drawn
         */
        void drawRenderTexture(RenderTexture &texture, const Vec2f &position, const Vec2f &viewportPosition);

        // /**
        //  * Draws a text
        //  * 
        //  * \param start Start point
        //  * \param end End point
        //  * \param thickness Thickness
        //  * \param color Color
        //  */
        // void drawText(const Vec2f &position, const char *text, TextOrigin textOrigin = TextOrigin::Default,
        //               const Color &color = Color(), int size = 16);

        /**
         * Clears the render target
         * 
         * \param clearColor Clear color
         */
        void clear(const Color &clearColor = Color());

    protected:
        RenderTarget(sf::RenderTarget *renderTarget);

    private:
        static sf::CircleShape sCircleDrawer;
        static sf::ConvexShape sPolygonDrawer;
        static sf::Sprite      sRenderTextureDrawer;

        sf::RenderTarget *mRenderTarget;
    };
};

#endif