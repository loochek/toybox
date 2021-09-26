#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Drawable.hpp"
#include "PhysicalObject.hpp"

class Entity
{
public:
    /// Creates a dummy entity
    Entity();
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

    /**
     * Sends an event which will be received by all components of the entity
     * Use this mechanism for inter-component interaction
     * 
     * \param eventType Event type
     * \param param1 Event parameter
     * \param param2 Another parameter
     */
    void sendEvent(Event eventType, void *param1, void* param2);

public:
    bool drawableComponentPresent;
    Drawable *drawableComponent;

    bool physicalComponentPresent;
    PhysicalObject *physicalComponent;

    bool scheduledForDeletion;
};

#endif