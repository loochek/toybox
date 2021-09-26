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
     * Entity will be responsible for its destruction
     * 
     * \param drawable Drawable component
     */
    void addDrawableComponent(Drawable *drawable);

    /**
     * Assigns a physical component to the entity
     * Entity will be responsible for its destruction
     * 
     * \param physicalObject Physical component
     */
    void addPhysicalComponent(PhysicalObject *physicalObject);

public:
    bool drawableComponentPresent;
    Drawable *drawableComponent;

    bool physicalComponentPresent;
    PhysicalObject *physicalComponent;
};

#endif