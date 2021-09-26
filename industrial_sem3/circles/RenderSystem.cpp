#include <cassert>
#include "RenderSystem.hpp"

const int MAX_OBJECTS_COUNT = 100;

RenderSystem::RenderSystem(Graphics &graphics) : graphics(graphics)
{
    components = new Drawable*[MAX_OBJECTS_COUNT];
    componentsCount = 0;
}

RenderSystem::~RenderSystem()
{
    delete[] components;
}

void RenderSystem::registerComponent(Drawable *component)
{
    assert(component != nullptr);

    if (componentsCount >= MAX_OBJECTS_COUNT)
        return;

    components[componentsCount++] = component;
}

void RenderSystem::draw()
{
    for (int i = 0; i < componentsCount; i++)
    {
        Drawable &drawable = *components[i];
        drawable.draw(graphics);
    }
}