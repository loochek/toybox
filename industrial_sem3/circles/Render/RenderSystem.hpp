#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <unordered_set>
#include "../Graphics.hpp"
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
     * Excludes drawable component from control
     * 
     * \param component Component
     */
    void unregisterComponent(Drawable *component);

    /**
     * Draws components
     */
    void draw();

private:
    Graphics &graphics;

    std::unordered_set<Drawable*> components;
};

#endif