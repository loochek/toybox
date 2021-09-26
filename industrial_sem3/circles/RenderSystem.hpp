#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "Graphics.hpp"
#include "Drawable.hpp"

/**
 * Responsible for displaying drawable components
 */
class RenderSystem
{
public:
    /**
     * \param graphics Graphics abstraction to be used for rendering
     */
    RenderSystem(Graphics &graphics);
    ~RenderSystem();

    /**
     * Adds drawable component to be controlled
     * 
     * \param component Component
     */
    void registerComponent(Drawable *component);

    /**
     * Draws components
     */
    void draw();

private:
    Graphics &graphics;

    Drawable **components;
    int componentsCount;
};

#endif