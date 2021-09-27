#include <cassert>
#include "RenderSystem.hpp"
#include "Entity.hpp"

const int MAX_OBJECTS_COUNT = 100;

RenderSystem::RenderSystem(Graphics &graphics) : graphics(graphics)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::registerComponent(Drawable *component)
{
    assert(component != nullptr);

    components.insert(component);
}

void RenderSystem::unregisterComponent(Drawable *component)
{
    assert(component != nullptr);

    components.erase(component);
}

void RenderSystem::draw()
{
    for (Drawable *obj : components)
    {
        if (!obj->entity->active)
            continue;
            
        obj->draw(graphics);
    }
}