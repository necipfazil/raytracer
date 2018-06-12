#ifndef __LIGHT_SPHERE_H__
#define __LIGHT_SPHERE_H__

#include "light.hpp"
#include "sphere.hpp"
#include "position3.hpp"
#include "material.hpp"
#include "vector3.hpp"

class LightSphere: public Light, public Sphere
{
    private:
        Vector3 radiance;
    public:
        LightSphere(const Sphere& sphere, const Vector3& radiance)
            : Sphere(sphere), radiance(radiance) {}

        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const;

        // Decorator for the method Sphere.hit
        virtual bool hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const;

};

#endif