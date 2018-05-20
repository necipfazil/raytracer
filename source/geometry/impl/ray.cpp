#include "../../config.h"
#include "../headers/ray.hpp"
#include "../headers/vector3.hpp"
#include "../headers/position3.hpp"
#include "../headers/iomethods.hpp"
#include "../../utility/random_number_generator.hpp"
#include <vector>
#include <iostream>

// given the hit position, returns the parameter t required to use to achieve the given hit position
float Ray::getTValue(const Position3 & hitPosition) const
{
    Vector3 displacement = origin.to(hitPosition);
    
    if(direction.getX() != 0.f)
        return displacement.getX() / direction.getX();
    else if(direction.getY() != 0.f)
        return displacement.getY() / direction.getY();
    else if(direction.getZ() != 0.f)
        return displacement.getZ() / direction.getZ();
    else return -1.0f;
}       

// given a float value (an epsilon value is likely to be used here), moves the origin of the
// .. ray in its direction by this float value
Ray & Ray::translateRayOrigin(const float val)
{
    Vector3 translation = this->direction * val;

    this->origin = this->origin + translation;

    return *this;
}
/*
bool Ray::getClosestHit(
    HitInfo & hitInfo,   // return the hit info
    const std::vector<Surface*> surfaces, // feed surfaces
    float epsilon)
{
    bool hit = false;
    
    for(int i = 0; i < surfaces.size(); i++)
    {
        Surface & surface = *surfaces[i];
        
        HitInfo currentSurfaceHitInfo;
        
        // do I hit the current surface?        
        if(surface.hit(*this, currentSurfaceHitInfo))
        {
            // if I did not hit a surface before, make it the first hit
            if(!hit || (currentSurfaceHitInfo.t < hitInfo.t))
            {
                if(currentSurfaceHitInfo.t > epsilon)
                {
                    hit = true;
                    hitInfo = currentSurfaceHitInfo;
                }
            }
        }
    }
    
    return hit;
}
*/
// given parameter value t, returns the position that this ray achieves
Position3 Ray::getPoint(const float & t) const
{    
    return this->getOrigin() + (this->getDirection() * t);
}

// reflection ray of "this" ray from an hit point
Ray Ray::createReflectionRay(const HitInfo & hitInfo) const
{
	Ray reflectionRay;

    // set time
    reflectionRay.setTimeCreated(this->getTimeCreated());

    // r = d - 2(d_dot_n)n , see textbook pg 87 equation 4.5
	Vector3 reflectionDirection = this->direction - 
	                              (hitInfo.normal * (2.0 * (direction ^ hitInfo.normal)));

    reflectionRay.setDirection(reflectionDirection);
    reflectionRay.setOrigin(hitInfo.hitPosition);
    
    // glossy reflection
    float roughness = hitInfo.material.getRoughness();

    if(roughness != 0.f)
    {
        std::vector<Vector3> orthoBasis = Vector3::generateOrthonomalBasis(reflectionDirection.normalize());

        reflectionDirection =
            orthoBasis[0] +
            (((orthoBasis[1] * getRandom0_5()) + (orthoBasis[2] * getRandom0_5())) * roughness);

        // change direction
        reflectionRay.setDirection(reflectionDirection);
    }

    reflectionRay.translateRayOrigin(RAY_TRANSLATION_EPSILON);

	return reflectionRay;
}
