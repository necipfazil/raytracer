#include "../../config.h"
#include "../headers/boundingvolume.hpp"
#include "../headers/transformation.hpp"
#include "../../utility/random_number_generator.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

// limits
#include <limits>

bool BoundingVolume::isEnclosing(const Position3 & point) const
{
    const Position3 & bvMin = this->minPosition;
    const Position3 & bvMax = this->maxPosition;

    return
        // X check
        point.getX() >= bvMin.getX() && point.getX() <= bvMax.getX() &&
        // Y check
        point.getY() >= bvMin.getY() && point.getY() <= bvMax.getY() &&
        // Z check
        point.getZ() >= bvMin.getZ() && point.getZ() <= bvMax.getZ();
}

Position3 BoundingVolume::getUniformPoint() const
{
    float psi = getRandomBtw01() * area;

    Position3 result;

    if(rightNode && leftNode)
        result = leftNode->getArea() > psi ? leftNode->getUniformPoint() : rightNode->getUniformPoint();
    else if(leftNode)
        result = leftNode->getUniformPoint();
    else if(rightNode)
        result = rightNode->getUniformPoint();

    if(this->hasTransformation)
    {
        result = this->transformation.transform(result);
    }

    return result;

}

bool BoundingVolume::hit(const Ray & originalRay, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const
{
    // set time of hit
    hitInfo.time = originalRay.getTimeCreated();
    
    // does the ray hit the volume or is it enclosed by the volume
    if(liangbarskyHit(originalRay))
    {
        Ray hitCheckRay = transformRayForIntersection(originalRay);

        HitInfo leftHitInfo, rightHitInfo;

        // make hits on both left and right
        bool leftHit = leftNode ? leftNode->hit(hitCheckRay, leftHitInfo, backfaceCulling, opaqueSearch) : false;
        bool rightHit = rightNode ? rightNode->hit(hitCheckRay, rightHitInfo, backfaceCulling, opaqueSearch) : false;

        bool result = leftHit || rightHit;

        // shortcut: if not hit neither left nor right, return false
        if(!result)
            return false;
        
        // update hitInfo
        if(leftHit && rightHit)
            hitInfo = leftHitInfo.t < rightHitInfo.t ? leftHitInfo : rightHitInfo;
        else if(leftHit)
            hitInfo = leftHitInfo;
        else if(rightHit)
            hitInfo = rightHitInfo;

        if(result)
        {
            if(this->hasMaterial)
                hitInfo.material = this->material;

            transformHitInfoAfterIntersection(originalRay, hitInfo);
        }

        return result;
    }
    else
    {
        // current box is not hit by the ray
        return false;
    }
}

// public bounding volume generator method
Shape* BoundingVolume::createBoundingVolumeHiearchy(
    std::vector<Shape*> &shapes
)
{
    return BoundingVolume::createBoundingVolumeHiearchy(
        shapes,           // surfaces
        0,                  // first index
        shapes.size(),    // number of surfaces
        Axis::X             // division axis
    );
}

// private bounding volume generator method
Shape* BoundingVolume::createBoundingVolumeHiearchy(
    std::vector<Shape*> &shapes,  // surfaces
    int firstInd, int numOfShapes,  // indices of first and last, used to determine the surfaces to consider
    Axis divisionAxis
)
{
    if(numOfShapes == 0)
    {
        return nullptr;
    }
    
    // reached to leaf
    if(numOfShapes == 1)
    {
        return shapes[firstInd];
    }

    // group the surfaces into two according to division axis, by comparing..
    int divisionInd = partitionShapesIntoTwo(
        shapes,
        firstInd,
        numOfShapes,
        divisionAxis
    );

    // create a new bounding volume
    BoundingVolume* bvh = new BoundingVolume();

    // create its nodes
        // left node
    bvh->leftNode = createBoundingVolumeHiearchy(
        shapes,
        firstInd,
        divisionInd - firstInd,
        BoundingVolume::nextDivisionAxis(divisionAxis)
    );

        // right node
    bvh->rightNode = createBoundingVolumeHiearchy(
        shapes,
        divisionInd,
        numOfShapes - (divisionInd - firstInd),
        BoundingVolume::nextDivisionAxis(divisionAxis)
    );

    // set minPosition & maxPosition
    if(bvh->leftNode != nullptr && bvh->rightNode != nullptr)
    {
        bvh->minPosition = Position3::generateMinPosition(bvh->leftNode->getMinPosition(), bvh->rightNode->getMinPosition());
        bvh->maxPosition = Position3::generateMaxPosition(bvh->leftNode->getMaxPosition(), bvh->rightNode->getMaxPosition());
    }
    else if(bvh->leftNode != nullptr)
    {
        bvh->minPosition = bvh->leftNode->getMinPosition();
        bvh->maxPosition = bvh->leftNode->getMaxPosition();
    }
    else if(bvh->rightNode != nullptr)
    {
        bvh->minPosition = bvh->rightNode->getMinPosition();
        bvh->maxPosition = bvh->rightNode->getMaxPosition();
    }
    else
    {
        throw "BoundingVolume::createBoundingVolumeHiearchy(), both nodes NULL?";
    }

    bvh->ownsChildren = true;

    // area
    bvh->area += bvh->leftNode  ? bvh->leftNode->getArea()  : 0;
    bvh->area += bvh->rightNode ? bvh->rightNode->getArea() : 0;

    // return newly created bvh
    return bvh;
}

#ifdef __BVH_DIVISION_BY_GEOMETRIC_CENTER__
// returns the index to divide the shapes vector
int BoundingVolume::partitionShapesIntoTwo(
    std::vector<Shape*> &shapes,
    int firstInd, int numOfShapes,
    Axis orderingAxis
)
{
    std::vector<Shape*>::iterator it = shapes.begin();

    // initialize min and max values with values that will be replaced for sure
    float maxPosInCurrentAxis = std::numeric_limits<float>::lowest();
    float minPosInCurrentAxis = std::numeric_limits<float>::max();

    // find min and max values
    for(int i = firstInd; i < firstInd + numOfShapes; i++)
    {
        float currentMax;
        float currentMin;

        // update current item with current item's value at ordering axis
        switch(orderingAxis)
        {
            case X: 
                currentMax = shapes[i]->getMaxPosition().getX(); 
                currentMin = shapes[i]->getMinPosition().getX(); 
                break;
            case Y: 
                currentMax = shapes[i]->getMaxPosition().getY(); 
                currentMin = shapes[i]->getMinPosition().getY(); 
                break;
            case Z: 
                currentMax = shapes[i]->getMaxPosition().getZ(); 
                currentMin = shapes[i]->getMinPosition().getZ(); 
                break;
        }

        // update min and max values if needed
        if(currentMin < minPosInCurrentAxis)
        {
            minPosInCurrentAxis = currentMin;
        }

        if(currentMax > maxPosInCurrentAxis)
        {
            maxPosInCurrentAxis = currentMax;
        }
    }

    // decide the geometric center value at ordering axis to divide the shapes vector into two
    float geometricCenter = (maxPosInCurrentAxis + minPosInCurrentAxis) / 2;

    std::vector<Shape*>::iterator bound;

    bound = std::partition(it + firstInd, it + firstInd + numOfShapes,
        [&](Shape* & s) -> bool
            {
                switch(orderingAxis)
                {
                    case X: 
                        return s->getMaxPosition().getX() < geometricCenter;
                    case Y: 
                        return s->getMaxPosition().getY() < geometricCenter;
                    case Z:
                        return s->getMaxPosition().getZ() < geometricCenter;
                }
            } 
        );
    

    // compute result, which is the index to bound
    int result = bound - shapes.begin();

    // mutate result if it does not partition the list
    if(result == firstInd)
    {
        result++;
    }

    // return result
    return result;
}
#else
// returns the index to divide the shapes vector
int BoundingVolume::partitionShapesIntoTwo(
    std::vector<Shape*> &shapes,
    int firstInd, int numOfShapes,
    Axis orderingAxis
)
{
    std::vector<Shape*>::iterator it = shapes.begin();

    // determine the compare function
    bool (*compareLTptr)(const Shape*, const Shape*);
    switch(orderingAxis)
    {
        case X:
            compareLTptr = &Shape::compareLTX;
            break;
        case Y:
            compareLTptr = &Shape::compareLTY;
            break;
        case Z:
            compareLTptr = &Shape::compareLTZ;
            break;
    }

    // group the elements into two groups such that the left group
    // .. is less than the right group with respect to ordering axis
    std::nth_element(
        it + firstInd,
        it + firstInd + (numOfShapes / 2),
        it + firstInd + numOfShapes,
        compareLTptr
    );

    return firstInd + numOfShapes / 2;
}
#endif

BoundingVolume* BoundingVolume::makeInstanceOf(BoundingVolume* toBeInstanced)
{
    BoundingVolume* instance = new BoundingVolume(*toBeInstanced);
    
    // instance does not own the children
    // TODO: To be improved by making use of shared_ptr
    instance->ownsChildren = false;
}


Axis BoundingVolume::nextDivisionAxis(Axis currentAxis)
{
    switch(currentAxis)
    {
        case X:
            return Axis::Y;
        case Y:
            return Axis::Z;
        case Z:
            return Axis::X;
    }
}

BoundingVolume::~BoundingVolume() 
{
    if(!ownsChildren)
        return;

    if(leftNode)
    {
        delete leftNode;
        leftNode = nullptr;
    }

    if(rightNode)
    {
        delete rightNode;
        rightNode = nullptr;
    }
}