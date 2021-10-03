#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "../Render/Drawable.hpp"
#include "../Physics/PhysicalObject.hpp"

class GameFramework;

class Entity
{
public:
    /// Creates a dummy entity
    Entity(GameFramework *engine);
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

    /**
     * Schedules entity deletion at the end of the logic update loop
     */
    void scheduleForDeletion() { active = false; scheduledForDeletion = true; };

    /**
     * \return Activity status
     */
    bool isActive() { return active; };

    /**
     * Makes entity visible to systems
     */
    void enable() { active = true; };

    /**
     * Makes entity invisible to systems
     */
    void disable() { active = false; };

    /**
     * \return Drawable component of the entity or null pointer
     */
    Drawable *getDrawableComponent() { return drawableComponent; };

    /**
     * \return Drawable component of the entity or null pointer
     */
    PhysicalObject *getPhysicalComponent() { return physicalComponent; };

    /**
     * \return Engine instancce
     */
    GameFramework *engineInstance() { return engine; };

private:
    Drawable       *drawableComponent;
    PhysicalObject *physicalComponent;

    GameFramework * const engine;

    bool active;
    
    friend class GameFramework;
    bool scheduledForDeletion;
};

#endif