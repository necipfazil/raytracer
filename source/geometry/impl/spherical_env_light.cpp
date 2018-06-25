#include "../headers/spherical_env_light.hpp"
#include "../headers/structs.hpp"
#include "../../scene.hpp"
#include "../../config.h"
#include <cmath>

IncidentLight SphericalEnvLight::getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const
{
    Vector3 dir = hitInfo.normal.generateRandomVectorWithinHemisphere(DEFAULT_RANDOM_FACTOR);

    IncidentLight incidentLight;

    #ifdef ENV_MAP_SHADOW_CHECK
    Ray shadowRay = Ray(hitInfo.hitPosition, dir).translateRayOrigin(scene.getShadowRayEpsilon());
    HitInfo dummyHitInfo;

    if(scene.getBVH() && scene.getBVH()->hit(shadowRay, dummyHitInfo, true, true))
    {
        incidentLight.inShadow = true;
        return incidentLight;
    }
    #endif
    
    incidentLight.inShadow = false;
    incidentLight.hitToLightDirection = dir;
    incidentLight.intensity = this->getColor(dir).getVector3().intensify(1 / SPHERE_UNIFORM_SAMPLING_PROP);

    return incidentLight;
}

Color SphericalEnvLight::getColor(const Vector3& dirParam) const
{
    // get normalized direction
    Vector3 direction = dirParam;
    direction.normalize();

    // find theta and psi angles for the sphere
    float theta = acos(direction.getY());
    float psi   = atan2(direction.getZ(), direction.getX());   

    // u and v
    float u = (-psi + M_PI) / (2 * M_PI);
    float v = theta / M_PI;

    // texture lookup
    Color texColor = this->sampleTexture(u, v);

    //return texColor.intensify(p_w);
    return texColor;
}

Color SphericalEnvLight::sampleTexture(float u, float v) const
{
    float i = u * image.getWidth();
    float j = v * image.getHeight();

    // bilinear interpolation

    // floor of coordinates
    int iFloor = i;
    int jFloor = j;

    // distance to floor coord
    float dx = i - iFloor;
    float dy = j - jFloor;

    Color someColor = image.getColor(iFloor    , jFloor    ).intensify((1 - dx) * (1 - dy));
    // there are 4 cells to get interpolated
    Color color =
        image.getColor(iFloor    , jFloor    ).intensify((1 - dx) * (1 - dy)) +
        image.getColor(iFloor + 1, jFloor + 1).intensify(dx       * dy      ) +
        image.getColor(iFloor + 1, jFloor    ).intensify(dx       * (1 - dy)) +
        image.getColor(iFloor    , jFloor + 1).intensify((1 - dx) * dy      );
        
    return color;
}