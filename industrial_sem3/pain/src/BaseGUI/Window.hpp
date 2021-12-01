#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Widget.hpp"

class Label;
class BaseButton;
class ButtonDelegate;
class DragArea;

/**
 * Base class for windows
 */
class Window : public Widget
{
public:
    /**
     * Sets window header title
     * 
     * \param title Title text
     */
    void setTitle(const char *title);

    /**
     * Resize which depends on content size
     * 
     * \param newContentSize New content size
     */
    virtual void resize(const Vec2i &newContentSize) override;
    
protected:
    Window() = delete;
    Window(const IntRect &contentRect, Widget *parent = nullptr);
    
    virtual ~Window() {};

    virtual void onRedrawThis() override;
    virtual void onResize() override;

private:
    void getTextures();

public:
    static const int HEADER_HEIGHT;
    static const int SIDE_BORDER_SIZE;
    static const int BOTTOM_BORDER_SIZE;

protected:
    Label      *mTitle;
    BaseButton *mCloseButton;
    DragArea   *mDragArea;

    const LGL::Texture *mCorner1Texture;
    const LGL::Texture *mCorner2Texture;
    const LGL::Texture *mCorner3Texture;
    const LGL::Texture *mCorner4Texture;
    const LGL::Texture *mEdgeTopTexture;
    const LGL::Texture *mEdgeLeftTexture;
    const LGL::Texture *mEdgeRightTexture;
    const LGL::Texture *mEdgeBottomTexture;

    const LGL::Texture *mCloseButtonIdleTexture;
    const LGL::Texture *mCloseButtonHoveredTexture;
};

#endif