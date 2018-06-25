#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "light.hpp"
#include "position3.hpp"
#include "vector3.hpp"

class Scene;

class PointLight : public Light
{
    private:
        Position3 position;
        Vector3 intensity;
    public:
        PointLight(const Position3& position, const Vector3& intensity)
            : position(position), intensity(intensity) { }

        Position3 getPosition() const { return this->position; }
        Vector3 getIntensity() const { return this->intensity; }

        void setPosition(const Position3& position) { this->position = position; }
        void setIntensity(const Vector3& intensity) { this->intensity = intensity; }

        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const;
};


#endif