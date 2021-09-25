#include <cassert>
#include "PhysicalObject.hpp"
#include "PhysicalCircle.hpp"
#include "PhysicalCircle2.hpp"

void PhysicalObject::update(float elapsedTime)
{
    position += velocity * elapsedTime;
}

bool dummyIntersectFunc(const PhysicalObject *obj1, const PhysicalObject *obj2, Vec2f *intersectionPoint)
{
    // Does nothing
    return false;
}

void dummyCollideFunc(PhysicalObject *obj1, PhysicalObject *obj2)
{
    // Does nothing
}

bool intersectFuncCircleCircle(const PhysicalObject *obj1, const PhysicalObject *obj2, Vec2f *intersectionPoint)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);
    assert(intersectionPoint != nullptr);

    const PhysicalCircle *circle1 = (const PhysicalCircle*)obj1;
    const PhysicalCircle *circle2 = (const PhysicalCircle*)obj2;

    Vec2f lengthVector = circle2->position - circle1->position;
    float intersectLength = circle1->radius + circle2->radius;

    if ((lengthVector ^ lengthVector) > intersectLength * intersectLength)
        return false;

    *intersectionPoint = circle1->position + lengthVector.normalized() * circle1->radius;
    return true;
}

void collideFuncCircleCircle(PhysicalObject *obj1, PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    const PhysicalCircle *circle1 = (const PhysicalCircle*)obj1;
    const PhysicalCircle *circle2 = (const PhysicalCircle*)obj2;

    Vec2f normal  = (obj1->position - obj2->position).normalized();
    Vec2f tangent = Vec2f(-normal.y, normal.x);

    Vec2f obj1VelNormProj = normal * ((obj1->velocity * obj1->mass) ^ normal);
    Vec2f obj1VelTangProj = obj1->velocity - obj1VelNormProj;
    Vec2f obj2VelNormProj = normal * ((obj2->velocity * obj2->mass) ^ normal);
    Vec2f obj2VelTangProj = obj2->velocity - obj2VelNormProj;

    obj1->velocity = (obj2VelNormProj + obj1VelTangProj) / obj1->mass;
    obj2->velocity = (obj1VelNormProj + obj2VelTangProj) / obj2->mass;
}

const IntersectFunc PhysicalObject::intersectTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT] =
{
    // Invalid            Circle1                   Circle2
    { dummyIntersectFunc, dummyIntersectFunc       , dummyIntersectFunc },
    { dummyIntersectFunc, intersectFuncCircleCircle, intersectFuncCircleCircle },
    { dummyIntersectFunc, intersectFuncCircleCircle, intersectFuncCircleCircle }
};

const CollideFunc PhysicalObject::collideTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT] =
{ 
    { dummyCollideFunc, dummyCollideFunc       , dummyCollideFunc },
    { dummyCollideFunc, collideFuncCircleCircle, dummyCollideFunc },
    { dummyCollideFunc, dummyCollideFunc, collideFuncCircleCircle }
};