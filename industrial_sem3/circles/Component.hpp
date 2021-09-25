#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Entity;

/**
 * Base abstract class for components
 */
class Component
{
protected:
    Component() = delete;

    Component(Entity *entity) : entity(entity) {};

protected:
    Entity *entity;
};

#endif