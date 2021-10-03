#include <cassert>
#include "RenderSystem.hpp"
#include "../GameFramework/Entity.hpp"

const int MAX_OBJECTS_COUNT = 100;

RenderSystem::RenderSystem(Graphics &graphics, const Rect &viewport) : graphics(graphics), viewport(viewport)
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
        if (!obj->getEntity()->isActive())
            continue;
            
        obj->draw(graphics, viewport);
    }
}