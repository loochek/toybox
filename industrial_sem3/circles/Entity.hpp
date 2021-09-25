#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Drawable.hpp"
#include "PhysicalObject.hpp"

class Entity
{
public:
    /// Creates a dummy entity
    Entity() : drawableComponentPresent(false), drawableComponent(nullptr),
               physicalComponentPresent(false), physicalComponent(nullptr) {};

    ~Entity();
               
    /**
     * Assigns a drawable component to the entity. 
     * Pass a null pointer to remove drawable component
     * 
     * \param drawable Drawable component
     */
    void assignDrawableComponent(Drawable *drawable);

    /**
     * Assigns a physical component to the entity
     * Pass a null pointer to remove drawable component
     * 
     * \param physicalObject Physical component
     */
    void assignPhysicalComponent(PhysicalObject *physicalObject);

private:
    bool drawableComponentPresent;
    Drawable *drawableComponent;

    bool physicalComponentPresent;
    PhysicalObject *physicalComponent;
};

#endif