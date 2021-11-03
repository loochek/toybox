#ifndef KEY_COLOR_CHANGED_DELEGATE_HPP
#define KEY_COLOR_CHANGED_DELEGATE_HPP

#include "../LGL/LGL.hpp"

class KeyColorChangedDelegate
{
public:
    virtual ~KeyColorChangedDelegate() {};

    virtual void onKeyColorChanged(const LGL::Color &color) = 0;
};

#endif