#include "../headers/light.hpp"
#include "../headers/arealight.hpp"
#include "../headers/pointlight.hpp"
#include "../headers/structs.hpp"

IncidentLight AreaLight::getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const
{
    PointLight pointLight = this->getPointLight(position);

    return pointLight.getIncidentLight(scene, hitInfo, time);
}