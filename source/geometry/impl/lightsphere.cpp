#include "../headers/lightsphere.hpp"
#include "../headers/pointlight.hpp"
#include "../headers/structs.hpp"
#include "../../utility/random_number_generator.hpp"
#include "../../scene.hpp"
#include <cmath>
#include <iostream>

bool LightSphere::hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const
{
    if(opaqueSearch)
        return false;
    
    bool result = Sphere(*this).hit(ray, hitInfo, backfaceCulling, opaqueSearch);

    hitInfo.isLight = true;
    hitInfo.lightColor = this->radiance;

    return result;
}

IncidentLight LightSphere::getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const
{
    Vector3 d = hitInfo.hitPosition.to(this->getCenter());

    float sinThetaMax = this->getRadius() / d.getNorm();
    float cosThetaMax = sqrt(1 - pow(sinThetaMax, 2));
    float thetaMax    = asin(sinThetaMax);

    // generate two random numbers
    float psi1 = getRandomBtw01();
    float psi2 = getRandomBtw01();

    // compute angles for the hemisphere above the hit position
    float fi    = 2 * M_PI * psi1;
    float theta = acos(1 - psi2 + psi2 * cosThetaMax);

    // generate orthonormal basis from normal
    std::vector<Vector3> orthonormalBasis = Vector3::generateOrthonomalBasis(d);

    // to follow the convention, extract u, v, w
    Vector3 w = orthonormalBasis[0], u = orthonormalBasis[1], v = orthonormalBasis[2];

    Vector3 l = w * cos(theta) +
                v * sin(theta) * cos(fi) +
                u * sin(theta) * sin(fi);

    // Now, create a ray to send through the sphere
    Ray w_i = Ray(hitInfo.hitPosition, l);

    HitInfo newHitInfo;
    // Thanks to the last parameter of hit, opaqueOnly, lightsphere would not
    // .. itself since it is not an opaque object
    bool hitCheck = this->hit(w_i, newHitInfo, false, false);

    if(!hitCheck)
    {
        IncidentLight result;
        result.inShadow = true;
        return result;
    }

    float p_w = 0.5f * M_1_PI * ( 1 / (1 - cosThetaMax) );

    // Create a point light
    PointLight pointLight = PointLight(newHitInfo.hitPosition, this->radiance);

    IncidentLight incidentLight = pointLight.getIncidentLight(scene, hitInfo, time);

    incidentLight.intensity /= p_w;

    return incidentLight;
}