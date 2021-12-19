#include <cassert>
#include <cstring>
#include "SFMLRenderTarget.hpp"
#include "SFMLRenderTexture.hpp"

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

const char *FONT_FILE_NAME = "resources/Segoe UI.ttf";

namespace LGL
{
    sf::Font RenderTarget::sFont;

    RenderTarget::RenderTarget(sf::RenderTarget *renderTarget) :
        mRenderTarget(renderTarget)
    {
        mTextDrawer.setFont(sFont);
    }

    void RenderTarget::setBlendMode(bool blendEnabled)
    {
        if (blendEnabled)
            mBlendMode = sf::BlendAlpha;
        else
            mBlendMode = sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero, sf::BlendMode::Add,
                                       sf::BlendMode::One, sf::BlendMode::Zero, sf::BlendMode::Add);
    }

    void RenderTarget::drawTriangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Color &color)
    {
        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(3);
        mPolygonDrawer.setPoint(0, toSFMLVector(p1));
        mPolygonDrawer.setPoint(1, toSFMLVector(p2));
        mPolygonDrawer.setPoint(2, toSFMLVector(p3));

        mRenderTarget->draw(mPolygonDrawer, mBlendMode);
    }

    void RenderTarget::drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, const Color &color)
    {
        mPolygonDrawer.setFillColor(toSFMLColor(color));

        mPolygonDrawer.setPointCount(4);
        mPolygonDrawer.setPoint(0, toSFMLVector(p1));
        mPolygonDrawer.setPoint(1, toSFMLVector(p2));
        mPolygonDrawer.setPoint(2, toSFMLVector(p3));
        mPolygonDrawer.setPoint(3, toSFMLVector(p4));

        mRenderTarget->draw(mPolygonDrawer, mBlendMode);
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

        mRenderTarget->draw(mCircleDrawer, mBlendMode);
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

        mRenderTarget->draw(mPolygonDrawer, mBlendMode);
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

        mRenderTarget->draw(mPolygonDrawer, mBlendMode);
    }

    void RenderTarget::drawText(const Vec2f &position, const char *text, TextOrigin textOrigin, 
                                const Color &color, int size)
    {
        assert(text != nullptr);

        mTextDrawer.setPosition(toSFMLVector(position));
        mTextDrawer.setString(text);
        mTextDrawer.setFillColor(toSFMLColor(color));
        mTextDrawer.setCharacterSize(size);

        if (textOrigin == TextOrigin::Centered)
        {
            sf::FloatRect textRect = mTextDrawer.getLocalBounds();
            mTextDrawer.setOrigin(textRect.left + textRect.width  / 2.0f,
                                  textRect.top  + textRect.height / 2.0f);
        }
        else
            mTextDrawer.setOrigin(sf::Vector2f(0.0f, 0.0f));

        mRenderTarget->draw(mTextDrawer, mBlendMode);
    }

    void RenderTarget::drawTexture(const Texture &texture, const Vec2f &position,
                                   const IntRect &textureRect, float opacity)
    {
        sf::Sprite textureDrawer;

        textureDrawer.setTexture(texture.mTexture);
        textureDrawer.setPosition(toSFMLVector(position));

        if (textureRect.size.x >= 0 && textureRect.size.y >= 0)
        {
            textureDrawer.setTextureRect(sf::IntRect(toSFMLVector(textureRect.position),
                                                     toSFMLVector(textureRect.size)));
        }

        textureDrawer.setColor(sf::Color(255, 255, 255, 255 * opacity));

        mRenderTarget->draw(textureDrawer, mBlendMode);
    }

    void RenderTarget::drawRenderTexture(RenderTexture &texture, const Vec2f &position,
                                        const Vec2f &viewportPosition)
    {
        sf::Sprite renderTextureDrawer;

        Vec2f viewportCenter = viewportPosition + fromSFMLVector(texture.mTexture.getSize()) / 2;

        sf::View view(toSFMLVector(viewportCenter), sf::Vector2f(texture.mTexture.getSize()));

        texture.mTexture.setView(view);
        texture.mTexture.display();

        renderTextureDrawer.setTexture(texture.mTexture.getTexture());
        renderTextureDrawer.setPosition(toSFMLVector(position));

        mRenderTarget->draw(renderTextureDrawer, mBlendMode);
    }

    void RenderTarget::clear(const Color &clearColor)
    {
        mRenderTarget->clear(toSFMLColor(clearColor));
    }

    void RenderTarget::loadFont()
    {
        if (!sFont.loadFromFile(FONT_FILE_NAME))
            throw std::runtime_error("Unable to load font");
    }

    Vec2f RenderTarget::calculateTextBounds(const char *text, int size)
    {
        sf::Text textDrawer;

        textDrawer.setFont(sFont);
        textDrawer.setString(text);
        textDrawer.setCharacterSize(size);

        sf::FloatRect bounds = textDrawer.getLocalBounds();

        return Vec2f(bounds.left * 2 + bounds.width, bounds.top * 2 + bounds.height);
    }

    float RenderTarget::calculateTextAxis(const char *text, int size)
    {
        // TODO: избавиться от костылей

        sf::Text textDrawer;

        textDrawer.setFont(sFont);
        textDrawer.setString(text);
        textDrawer.setCharacterSize(size);

        sf::FloatRect localBounds  = textDrawer.getLocalBounds();

        sf::Vector2f textOffset = sf::Vector2f(localBounds.left, localBounds.top);

        return (size + textOffset.y) / 2;
    }

    float RenderTarget::calculateCharacterOffset(const char *text, int index, int size)
    {
        sf::Text textDrawer;

        textDrawer.setFont(sFont);
        textDrawer.setString(text);
        textDrawer.setCharacterSize(size);

        return textDrawer.findCharacterPos(index).x;
    }
};