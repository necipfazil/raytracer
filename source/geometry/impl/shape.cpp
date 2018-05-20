#include "../headers/shape.hpp"
#include <limits>
#include <vector>
#include <iostream>

bool Shape::compareLTX(const Shape * lhs, const Shape * rhs)
{
    return Position3::compareLTX(lhs->getMinPosition(), rhs->getMinPosition());
}

bool Shape::compareLTY(const Shape * lhs, const Shape * rhs)
{
    return Position3::compareLTY(lhs->getMinPosition(), rhs->getMinPosition());
}

bool Shape::compareLTZ(const Shape * lhs, const Shape * rhs)
{
    return Position3::compareLTZ(lhs->getMinPosition(), rhs->getMinPosition());
}

// helper for the method Shape::liangbarskyHit
inline void updateTEntering(const float & t, float & tEntering)
{
    // looking for largest entering
    // therefore, if t > tEntering, update
    if(t > tEntering)
        tEntering = t;
}

// helper for the method Shape::liangbarskyHit
inline void updateTExitting(const float & t, float & tExitting)
{
    // looking for smallest exitting
    // therefore, if t < tExitting, update
    if(t < tExitting)
        tExitting = t;
}

bool Shape::liangbarskyHit(const Ray & ray) const
{
    // looking for:
    //      largest entering,
    //      smallest exitting
    // therefore, init entering as with min and exitting with max
    // .. to allow updates
    float tEntering = std::numeric_limits<float>::lowest();
    float tExitting = std::numeric_limits<float>::max();

    const Vector3 rayDirection = ray.getDirection();
    const Position3 rayOrigin = ray.getOrigin();

    const Vector3 minDiv = (this->getMinPosition() - rayOrigin) / rayDirection;
    const Vector3 maxDiv = (this->getMaxPosition() - rayOrigin) / rayDirection;

    float t = 0.0f;

    // check for X
    // check only if ray could move in X direction
    if(rayDirection.getX() != 0.0f)
    {
        // find the direction of ray in X, which changes entering-exitting situtations
        if(rayDirection.getX() > 0.0f)
        {
            // entering for xMin
            t = minDiv.getX();
            updateTEntering(t, tEntering);

            // exitting for xMax
            t = maxDiv.getX();
            updateTExitting(t, tExitting);
        }
        else
        {
            // exitting for xMin
            t = minDiv.getX();
            updateTExitting(t, tExitting);

            // entering for xMax
            t = maxDiv.getX();
            updateTEntering(t, tEntering);

        }
    }

    // shortcut
    if(!(tEntering <= tExitting))
        return false;

    // check for Y
    // check only if ray could move in Y direction
    if(rayDirection.getY() != 0.0f)
    {
        // find the direction of ray in Y, which changes entering-exitting situtations
        if(rayDirection.getY() > 0.0f)
        {
            // entering for yMin
            t = minDiv.getY();
            updateTEntering(t, tEntering);

            // exitting for yMax
            t = maxDiv.getY();
            updateTExitting(t, tExitting);
        }
        else
        {
            // exitting for yMin
            t = minDiv.getY();
            updateTExitting(t, tExitting);

            // entering for yMax
            t = maxDiv.getY();
            updateTEntering(t, tEntering);
        }
    }

    // shortcut
    if(!(tEntering <= tExitting))
        return false;

    // check for Z
    // check only if ray could move in Z direction
    if(rayDirection.getZ() != 0.0f)
    {
        // find the direction of ray in Z, which changes entering-exitting situtations
        if(rayDirection.getZ() > 0.0f)
        {
            // entering for zMin
            t = minDiv.getZ();
            updateTEntering(t, tEntering);

            // exitting for zMax
            t = maxDiv.getZ();
            updateTExitting(t, tExitting);
        }
        else
        {
            // exitting for zMin
            t = minDiv.getZ();
            updateTExitting(t, tExitting);

            // entering for zMax
            t = maxDiv.getZ();
            updateTEntering(t, tEntering);
        }
    }

    // we desire to have tEntering < tExitting
    return tEntering <= tExitting;
}

std::vector<Position3> Shape::getAllVertices() const
{
    std::vector<Position3> vertices;

    // to make it more readable
    const Position3 & min = this->minPosition;
    const Position3 & max = this->maxPosition;

    vertices.push_back(minPosition);
    vertices.push_back(maxPosition);

    // other than min and max, there are 6 other vertices

    // 1: maxX, minY, minZ
    vertices.push_back(
        Position3(
            max.getX(),
            min.getY(),
            min.getZ()
        )
    );

    // 2: minX, maxY, minZ
    vertices.push_back(
        Position3(
            min.getX(),
            max.getY(),
            min.getZ()
        )
    );

    // 3: minX, minY, maxZ
    vertices.push_back(
        Position3(
            min.getX(),
            min.getY(),
            max.getZ()
        )
    );

    // 4: maxX, maxY, minZ
    vertices.push_back(
        Position3(
            max.getX(),
            max.getY(),
            min.getZ()
        )
    );

    // 5: maxX, minY, maxZ
    vertices.push_back(
        Position3(
            max.getX(),
            min.getY(),
            max.getZ()
        )
    );

    // 6: minX, maxY, maxZ
    vertices.push_back(
        Position3(
            min.getX(),
            max.getY(),
            max.getZ()
        )
    );

    return vertices;
}

void Shape::setMotionBlur(const Vector3& motionBlur)
{
    // has motion blur
    this->hasMotionBlur = true;

    // set motion blur field
    this->motionBlur = motionBlur;

    Translation motionBlurTranslation = Translation(motionBlur.getX(), motionBlur.getY(), motionBlur.getZ());

        // update min/max position
        // create positions for all of the vertices of the box
        // .. so that, all could be transformed and new min/max
        // .. could be determined
    std::vector<Position3> vertices = getAllVertices();

    // transform all of the vertices
    for(int i = 0; i < (int)vertices.size(); i++)
    {
        vertices[i] = motionBlurTranslation.transform<Position3>(vertices[i]);
    }   

    // find new min/max
    Position3 newMin = this->minPosition;
    Position3 newMax = this->maxPosition;

    for(int i = 0; i < (int)vertices.size(); i++)
    {
        // update if necessary
        newMin = Position3::generateMinPosition(newMin, vertices[i]);
        newMax = Position3::generateMaxPosition(newMax, vertices[i]);
    }

    // update minPosition and maxPosition
    this->minPosition = newMin;
    this->maxPosition = newMax;
}

void Shape::transform(const Transformation& transformation)
{
    // has transformation
    this->hasTransformation = true;

    // set transformation field
    this->transformation = transformation;

    // update min/max position
        // create positions for all of the vertices of the box
        // .. so that, all could be transformed and new min/max
        // .. could be determined
    std::vector<Position3> vertices = getAllVertices();
    
    // transform all of the vertices
    for(int i = 0; i < (int)vertices.size(); i++)
    {
        vertices[i] = transformation.transform<Position3>(vertices[i]);
    }

    // find new min/max
    Position3 newMin = Position3(std::numeric_limits<float>::max());
    Position3 newMax = Position3(std::numeric_limits<float>::lowest());

    for(int i = 0; i < (int)vertices.size(); i++)
    {
        // update if necessary
        newMin = Position3::generateMinPosition(newMin, vertices[i]);
        newMax = Position3::generateMaxPosition(newMax, vertices[i]);
    }

    // update minPosition and maxPosition
    this->minPosition = newMin;
    this->maxPosition = newMax;
}


Ray Shape::transformRayForIntersection(const Ray & originalRay) const
{
    // Here you are going to see some simple optimization tricks
    // Therefore, code is not simple as it could have been

    Ray ray = originalRay;

    // both
    if(this->hasTransformation && this->hasMotionBlur)
    {
        Transformation totalTransformation = this->transformation;

        totalTransformation += Translation(this->motionBlur * ray.getTimeCreated());

        ray = totalTransformation.inverseTransform<Ray>(ray);
    }
    else if(this->hasTransformation)
    {
        // only transformation
        ray = this->transformation.inverseTransform<Ray>(ray);
    }
    else if(this->hasMotionBlur)
    {
        // only motion blur
        Translation motionBlurTranslation(this->motionBlur * ray.getTimeCreated());

        ray = motionBlurTranslation.inverseTransform<Ray>(ray);
    }

    return ray;
}

void Shape::transformHitInfoAfterIntersection(const Ray & originalRay, HitInfo & hitInfo) const
{
    // Here you are going to see some simple optimization tricks
    // Therefore, code is not simple as it could have been

    // both
    if(this->hasTransformation && this->hasMotionBlur)
    {
        Transformation totalTransformation = this->transformation;

        totalTransformation += Translation(this->motionBlur * originalRay.getTimeCreated());

        hitInfo = totalTransformation.hitInfoTransform(hitInfo, originalRay);
    }
    else if(this->hasTransformation)
    {
        // only transformation
        hitInfo = this->transformation.hitInfoTransform(hitInfo, originalRay);
    }
    else if(this->hasMotionBlur)
    {
        // only motion blur
        Translation motionBlurTranslation(this->motionBlur * originalRay.getTimeCreated());

        hitInfo = motionBlurTranslation.hitInfoTransform(hitInfo, originalRay);
    }

}