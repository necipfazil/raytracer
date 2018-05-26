#include "../headers/light.hpp"
#include "../headers/pointlight.hpp"
#include "../headers/ray.hpp"
#include "../../scene.hpp"

IncidentLight PointLight::getIncidentLight(const Scene& scene, const Position3& position, float time) const
{
    IncidentLight result;

    Vector3 hit2light = position.to(this->position);

    // check if in shadow
        // first, create the shadow ray
    Ray shadowRay(position, hit2light);

    // set time for ray creation
    shadowRay.setTimeCreated(time);
    
    // move ray's origin with epsilon
    shadowRay.translateRayOrigin(scene.getShadowRayEpsilon());

    HitInfo shadowRayHitInfo;

    // TODO: Null check
    if( scene.getBVH()->hit(shadowRay, shadowRayHitInfo, true) )
    {   
        float hitPointToLightT = shadowRay.getTValue(this->position);

        result.inShadow = hitPointToLightT > shadowRayHitInfo.t;
    }
    else
    {
        result.inShadow = false;
    }

    // if in shadow, do not continue computation
    if(result.inShadow)
        return result;
    
    // compute intensity
    float distanceSq = hit2light ^ hit2light;
    
    Vector3 intensity = this->intensity / distanceSq;

    // set intensity
    result.intensity = intensity;

    // set direction
    result.hitToLightDirection = hit2light.normalize();

    return result;
}