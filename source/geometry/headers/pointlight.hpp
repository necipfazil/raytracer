#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "light.hpp"
#include "position3.hpp"
#include "vector3.hpp"

class PointLight : public Light
{
    public:
        PointLight(const Position3& position, const Vector3& intensity)
            : Light(position, intensity) { }
    //public:
        //Position3 getPosition() const { return this->position; }
        //Vector3 getIntensity() const { return this->intensity; }

        //void setPosition(const Position3& position) { this->position = position; }
        //void setIntensity(const Vector3& intensity) { this->intensity = intensity; }
};


#endif