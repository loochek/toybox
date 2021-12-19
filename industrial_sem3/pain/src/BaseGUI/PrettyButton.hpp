#ifndef PRETTY_BUTTON_HPP
#define PRETTY_BUTTON_HPP

#include "../LGL/LGL.hpp"
#include "AnimatedButton.hpp"

/**
 * Button with custom textures
 */
class PrettyButton : public AnimatedButton
{
public:
    PrettyButton() = delete;
    PrettyButton(const Vec2i &buttonPos, int buttonSize, Widget *parent = nullptr);

    virtual ~PrettyButton() {};

    /**
     * Sets a label for the button. 
     * No label will drawn if null pointer is passed
     * 
     * \param label Label text
     */
    virtual void setLabel(const char *label);

protected:
    virtual void onRedrawThis() override;

private:
    void getTextures();

public:
    static const int BUTTON_HEIGHT;

protected:
    const LGL::Texture *mLeftIdleTexture;
    const LGL::Texture *mLeftHoveredTexture;
    const LGL::Texture *mLeftPressedTexture;

    const LGL::Texture *mRightIdleTexture;
    const LGL::Texture *mRightHoveredTexture;
    const LGL::Texture *mRightPressedTexture;

    const LGL::Texture *mBodyIdleTexture;
    const LGL::Texture *mBodyHoveredTexture;
    const LGL::Texture *mBodyPressedTexture;
};

#endif