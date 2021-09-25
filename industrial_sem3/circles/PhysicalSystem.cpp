#include "PhysicalSystem.hpp"

void PhysicalSystem::update(float elapsedTime)
{
    for (int i = 0; i < objectsCount; i++)
    {
        PhysicalObject &object = *objects[i];

        object.update(elapsedTime);
        //checkBounds(object);
    }

    for (int i = 0; i < objectsCount; i++)
    {
        PhysicalObject &obj1 = *objects[i];

        for (int j = i + 1; j < objectsCount; j++)
        {
            PhysicalObject &obj2 = *objects[j];

            IntersectFunc intersectFunc = PhysicalObject::intersectTable[(int)obj1.type][(int)obj2.type];
            CollideFunc   collideFunc   = PhysicalObject::collideTable  [(int)obj1.type][(int)obj2.type];

            if (!intersectFunc(&obj1, &obj2))
                continue;

            collideFunc(&obj1, &obj2);
        }
    }
}