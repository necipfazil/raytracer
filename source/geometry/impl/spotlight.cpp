#include "../headers/spot_light.hpp"
#include "../headers/light.hpp"
#include "../headers/ray.hpp"
#include "../../scene.hpp"

IncidentLight SpotLight::getIncidentLight(const Scene& scene, const Position3& position, float time) const
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


    // intensity
    result.intensity = this->getIntensity(position);

    // direction
    result.hitToLightDirection = hit2light.normalize();

    return result;
}