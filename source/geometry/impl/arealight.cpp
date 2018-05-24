#include "../headers/light.hpp"
#include "../headers/arealight.hpp"
#include "../headers/pointlight.hpp"

IncidentLight AreaLight::getIncidentLight(const Scene& scene, const Position3& position, float time) const
{
    PointLight pointLight = this->getPointLight(position);

    return pointLight.getIncidentLight(scene, position, time);
}