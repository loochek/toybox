#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "../Utils/Vec2.hpp"
#include "Canvas.hpp"

/**
 * Like a tool, but applied to the entire canvas
 */
class Effect
{
public:
    virtual void apply(Canvas &canvas) = 0;
    
protected:
    Effect() {};
};

#endif