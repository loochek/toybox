#include "SFMLRenderTarget.hpp"
#include "SFMLRenderTexture.hpp"

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

namespace LGL
{
    RenderTarget::RenderTarget(sf::RenderTarget *renderTarget) : mRenderTarget(renderTarget)
    {
    }

    void RenderTarget::drawTriangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Color &color)
    {
        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(3);
        mPolygonDrawer.setPoint(0, toSFMLVector(p1));
        mPolygonDrawer.setPoint(1, toSFMLVector(p2));
        mPolygonDrawer.setPoint(2, toSFMLVector(p3));

        mRenderTarget->draw(mPolygonDrawer);
    }

    void RenderTarget::drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, const Color &color)
    {
        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(4);
        mPolygonDrawer.setPoint(0, toSFMLVector(p1));
        mPolygonDrawer.setPoint(1, toSFMLVector(p2));
        mPolygonDrawer.setPoint(2, toSFMLVector(p3));
        mPolygonDrawer.setPoint(3, toSFMLVector(p4));

        mRenderTarget->draw(mPolygonDrawer);
    }

    void RenderTarget::drawRect(const FloatRect &rect, const Color &color)
    {
        Vec2f p1 = rect.position;
        Vec2f p2 = rect.position + Vec2f(rect.size.x, 0);
        Vec2f p3 = rect.position + rect.size;
        Vec2f p4 = rect.position + Vec2f(0, rect.size.y);

        drawQuad(p1, p2, p3, p4, color);
    }

    void RenderTarget::drawCircle(const Vec2f &position, float radius, const Color &color)
    {
        mCircleDrawer.setPosition(toSFMLVector(position));
        mCircleDrawer.setRadius(radius);
        mCircleDrawer.setOrigin(sf::Vector2f(radius, radius));
        mCircleDrawer.setFillColor(toSFMLColor(color));

        mRenderTarget->draw(mCircleDrawer);
    }

    void RenderTarget::drawLine(const Vec2f &p1, const Vec2f &p2, float thickness, const Color &color)
    {
        Vec2f line_vec = p2 - p1;
        Vec2f normal = Vec2f(-line_vec.y, line_vec.x) / line_vec.length();

        Vec2f vert1 = p1 - normal * thickness;
        Vec2f vert2 = p1 + normal * thickness;
        Vec2f vert3 = p2 + normal * thickness;
        Vec2f vert4 = p2 - normal * thickness;
        
        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(4);
        mPolygonDrawer.setPoint(0, toSFMLVector(vert1));
        mPolygonDrawer.setPoint(1, toSFMLVector(vert2));
        mPolygonDrawer.setPoint(2, toSFMLVector(vert3));
        mPolygonDrawer.setPoint(3, toSFMLVector(vert4));

        mRenderTarget->draw(mPolygonDrawer);
    }

    void RenderTarget::drawArrow(const Vec2f &start, const Vec2f &end, float thickness, const Color &color)
    {
        drawLine(start, end, thickness, color);

        Vec2f line_vec_norm = (end - start).normalized();
        Vec2f normal = Vec2f(-line_vec_norm.y, line_vec_norm.x);

        Vec2f vert1 = end - line_vec_norm * ARROW_HEIGHT - normal * ARROW_WIDTH;
        Vec2f vert2 = end - line_vec_norm * ARROW_HEIGHT + normal * ARROW_WIDTH;

        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(3);
        mPolygonDrawer.setPoint(0, toSFMLVector(end));
        mPolygonDrawer.setPoint(1, toSFMLVector(vert1));
        mPolygonDrawer.setPoint(2, toSFMLVector(vert2));

        mRenderTarget->draw(mPolygonDrawer);
    }

    // void RenderTarget::drawText(const Vec2f &position, const char *text, TextOrigin textOrigin, 
    //                         const Color &color, int size)
    // {
    //     assert(text != nullptr);

    //     textDrawer.setPosition(toSFMLVector(position));
    //     textDrawer.setString(text);
    //     textDrawer.setFillColor(toSFMLColor(color));
    //     textDrawer.setCharacterSize(size);
    //     textDrawer.setStyle(sf::Text::Bold);

    //     if (textOrigin == TextOrigin::Centered)
    //     {
    //         sf::FloatRect textRect = textDrawer.getLocalBounds();
    //         textDrawer.setOrigin(textRect.left + textRect.width  / 2.0f,
    //                              textRect.top  + textRect.height / 2.0f);
    //     }
    //     else
    //         textDrawer.setOrigin(sf::Vector2f(0.0f, 0.0f));

    //     mRenderTarget->draw(textDrawer);
    // }

    void RenderTarget::drawRenderTexture(RenderTexture &texture, const Vec2f &position,
                                        const Vec2f &viewportPosition)
    {
        //Vec2f viewportCenter = viewportPosition + fromSFMLVector(texture.mTexture.getSize()) / 2;

        //sf::View view(toSFMLVector(viewportCenter), sf::Vector2f(texture.mTexture.getSize()));

        //texture.mTexture.setView(view);
        texture.mTexture.display();

        mRenderTextureDrawer.setTexture(texture.mTexture.getTexture());
        auto x = texture.mTexture.getTexture().getSize();
        printf("texture size: %dx%d\n", x.x, x.y);
        mRenderTextureDrawer.setPosition(toSFMLVector(position));

        mRenderTarget->draw(mRenderTextureDrawer);
    }

    void RenderTarget::clear(const Color &clearColor)
    {
        mRenderTarget->clear(toSFMLColor(clearColor));
    }
};