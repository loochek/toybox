 #include "Entity.hpp"

 Entity::~Entity()
 {
    if (drawableComponentPresent)
        delete drawableComponent;
    
    if (physicalComponentPresent)
        delete physicalComponent;
 }