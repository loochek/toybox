#include <cassert>
#include "PhysicalObject.hpp"
#include "PhysicalCircle.hpp"
#include "PhysicalCircle2.hpp"
#include "PhysicalBound.hpp"

void PhysicalObject::update(float elapsedTime)
{
    position += velocity * elapsedTime;
}


// The logic of the intersection and collision of physical objects of different types

static bool dummyIntersectFunc       (const PhysicalObject *obj1, const PhysicalObject *obj2);
static bool intersectFuncCircleCircle(const PhysicalObject *obj1, const PhysicalObject *obj2);
static bool intersectFuncCircleBound (const PhysicalObject *obj1, const PhysicalObject *obj2);
static bool intersectFuncBoundCircle (const PhysicalObject *obj1, const PhysicalObject *obj2);

static void dummyCollideFunc       (PhysicalObject *obj1, PhysicalObject *obj2);
static void collideFuncCircleCircle(PhysicalObject *obj1, PhysicalObject *obj2);
static void collideFuncCircleBound (PhysicalObject *obj1, PhysicalObject *obj2);
static void collideFuncBoundCircle (PhysicalObject *obj1, PhysicalObject *obj2);

static bool genericIntersectFuncCircleBound(const PhysicalCircle *circle, const PhysicalBound *bound);
static void genericCollideFuncCircleBound  (PhysicalCircle *circle, PhysicalBound *bound);


const IntersectFunc PhysicalObject::intersectTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT] =
{
    // Invalid            Bound                     Circle1                    Circle2 
    { dummyIntersectFunc, dummyIntersectFunc      , dummyIntersectFunc       , dummyIntersectFunc },
    { dummyIntersectFunc, dummyIntersectFunc      , intersectFuncBoundCircle , intersectFuncBoundCircle },
    { dummyIntersectFunc, intersectFuncCircleBound, intersectFuncCircleCircle, intersectFuncCircleCircle },
    { dummyIntersectFunc, intersectFuncCircleBound, intersectFuncCircleCircle, intersectFuncCircleCircle }
};

const CollideFunc PhysicalObject::collideTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT] =
{
    // Invalid            Bound                 Circle1                  Circle2 
    { dummyCollideFunc, dummyCollideFunc      , dummyCollideFunc       , dummyCollideFunc },
    { dummyCollideFunc, dummyCollideFunc      , collideFuncBoundCircle , collideFuncBoundCircle },
    { dummyCollideFunc, collideFuncCircleBound, collideFuncCircleCircle, collideFuncCircleCircle },
    { dummyCollideFunc, collideFuncCircleBound, collideFuncCircleCircle, collideFuncCircleCircle }
};


static bool dummyIntersectFunc(const PhysicalObject *obj1, const PhysicalObject *obj2)
{
    // Does nothing
    return false;
}

static void dummyCollideFunc(PhysicalObject *obj1, PhysicalObject *obj2)
{
    // Does nothing
}

static bool intersectFuncCircleCircle(const PhysicalObject *obj1, const PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    const PhysicalCircle *circle1 = (const PhysicalCircle*)obj1;
    const PhysicalCircle *circle2 = (const PhysicalCircle*)obj2;

    Vec2f lengthVector = circle2->position - circle1->position;
    float intersectLength = circle1->radius + circle2->radius;

    if ((lengthVector ^ lengthVector) > intersectLength * intersectLength)
        return false;

    return true;
}

static bool intersectFuncCircleBound(const PhysicalObject *obj1, const PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    const PhysicalCircle *circle = (const PhysicalCircle*)obj1;
    const PhysicalBound  *bound  = (const PhysicalBound* )obj2;

    return genericIntersectFuncCircleBound(circle, bound);
}

static bool intersectFuncBoundCircle(const PhysicalObject *obj1, const PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    const PhysicalBound  *bound  = (const PhysicalBound* )obj1;
    const PhysicalCircle *circle = (const PhysicalCircle*)obj2;

    return genericIntersectFuncCircleBound(circle, bound);
}

static void collideFuncCircleCircle(PhysicalObject *obj1, PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    const PhysicalCircle *circle1 = (const PhysicalCircle*)obj1;
    const PhysicalCircle *circle2 = (const PhysicalCircle*)obj2;

    Vec2f normal  = (obj1->position - obj2->position).normalized();
    Vec2f tangent = Vec2f(-normal.y, normal.x);

    Vec2f obj1VelNormProj = normal * ((obj1->velocity) ^ normal);
    Vec2f obj1VelTangProj = obj1->velocity - obj1VelNormProj;
    Vec2f obj2VelNormProj = normal * ((obj2->velocity) ^ normal);
    Vec2f obj2VelTangProj = obj2->velocity - obj2VelNormProj;

    obj1->velocity = (obj2VelNormProj + obj1VelTangProj);
    obj2->velocity = (obj1VelNormProj + obj2VelTangProj);
}

static void collideFuncCircleBound(PhysicalObject *obj1, PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    PhysicalCircle *circle = (PhysicalCircle*)obj1;
    PhysicalBound  *bound  = (PhysicalBound* )obj2;

    genericCollideFuncCircleBound(circle, bound);
}

static void collideFuncBoundCircle(PhysicalObject *obj1, PhysicalObject *obj2)
{
    assert(obj1 != nullptr);
    assert(obj2 != nullptr);

    PhysicalBound  *bound  = (PhysicalBound* )obj1;
    PhysicalCircle *circle = (PhysicalCircle*)obj2;

    genericCollideFuncCircleBound(circle, bound);
}

static bool genericIntersectFuncCircleBound(const PhysicalCircle *circle, const PhysicalBound *bound)
{
    assert(circle != nullptr);
    assert(bound != nullptr);

    if (bound->boundType == PhysicalBoundType::Horizontal)
    {
        if (fabs(circle->position.x - bound->position.x) > circle->radius)
            return false;

        return true;
    }
    else
    {
        if (fabs(circle->position.y - bound->position.y) > circle->radius)
            return false;

        return true;
    }
}

static void genericCollideFuncCircleBound(PhysicalCircle *circle, PhysicalBound *bound)
{
    assert(circle != nullptr);
    assert(bound != nullptr);

    if (bound->boundType == PhysicalBoundType::Horizontal)
    {
        if (circle->position.x - bound->position.x > 0)
        {
            // Wall on the left
            // Push the circle right

            float pushDistance = bound->position.x - (circle->position.x - circle->radius);
            circle->position.x += pushDistance;
        }
        else
        {
            // Wall on the right
            // Push the circle left

            float pushDistance = (circle->position.x + circle->radius) - bound->position.x;
            circle->position.x -= pushDistance;
        }

        circle->velocity.x = -circle->velocity.x;
    }
    else
    {
        if (circle->position.y - bound->position.y > 0)
        {
            // Wall on the bottom
            // Push the circle up

            float pushDistance = bound->position.y - (circle->position.y - circle->radius);
            circle->position.y += pushDistance;
        }
        else
        {
            // Wall on the top
            // Push the circle down

            float pushDistance = (circle->position.y + circle->radius) - bound->position.y;
            circle->position.y -= pushDistance;
        }

        circle->velocity.y = -circle->velocity.y;
    }
}