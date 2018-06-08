#ifndef __DIRECTIONAL_LIGHT_H__
#define __DIRECTIONAL_LIGHT_H__

#include "vector3.hpp"
#include "light.hpp"

class Scene;

class DirectionalLight : public Light
{
    private:
        Vector3 direction;
        Vector3 radiance;
    public:
        DirectionalLight(const Vector3& direction, const Vector3& radiance)
            : direction(direction), radiance(radiance) { this->direction.normalize(); }

        void setDirection(const Vector3& direction) { this->direction = direction; this->direction.normalize(); }
        Vector3 getDirection() const { return this->direction; }
        Vector3 getReverseDirection() const { return -this->direction; }

        void setRadiance(const Vector3& radiance) { this->radiance = radiance; }
        Vector3 getRadiance() const { return this->radiance; }

        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const;
};

#endif