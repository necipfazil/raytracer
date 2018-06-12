#include "../headers/lightmesh.hpp"
#include "../headers/position3.hpp"
#include "../headers/pointlight.hpp"
#include "../headers/triangle.hpp"
#include "../../utility/random_number_generator.hpp"
#include <cmath>

Position3 LightMesh::getUniformPoint() const
{
    Position3 uniformPoint = this->mesh->getUniformPoint();

    if(this->hasTransformation)
        uniformPoint = this->transformation.transform(uniformPoint);

    return uniformPoint;
}

IncidentLight LightMesh::getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const
{
    Position3 uniformPoint = getUniformPoint();

    // treat it as a point light
    PointLight pointLight = PointLight(uniformPoint, this->radiance);

    return pointLight.getIncidentLight(scene, hitInfo, time);
}

// Decorator
bool LightMesh::hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const
{
    if(opaqueSearch || mesh == nullptr)
        return false;
    
    // call included mesh's hit method
    bool result = mesh->hit(ray, hitInfo, false, opaqueSearch);

    hitInfo.isLight = true;
    hitInfo.lightColor = this->radiance;

    return result;
}