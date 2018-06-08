#include "../headers/directional_light.hpp"
#include "../headers/position3.hpp"
#include "../../scene.hpp"

IncidentLight DirectionalLight::getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const
{
    IncidentLight result;

    // check shadow
    // create shadow ray
    Ray shadowRay(hitInfo.hitPosition, this->getReverseDirection());

    // set time for ray creation
    shadowRay.setTimeCreated(time);
    
    // move ray's origin with epsilon
    shadowRay.translateRayOrigin(scene.getShadowRayEpsilon());

    HitInfo shadowRayHitInfo;

    result.inShadow = scene.getBVH()->hit(shadowRay, shadowRayHitInfo, true, true);

    // if in shadow, do not continue computation
    if(result.inShadow)
        return result;
    
    // set intensity
    result.intensity = this->radiance;

    // set direction
    result.hitToLightDirection = this->getReverseDirection();

    return result;
}