#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vector3.hpp"
#include "position3.hpp"

class Light
{
    protected:
        Position3 position;
        Vector3 intensity;
    public:
        Light(const Position3& position, const Vector3& intensity)
            : position(position), intensity(intensity) {}
            
        virtual Position3 getPosition() const { return this->position; }
        virtual Vector3 getIntensity() const { return this->intensity; }

        virtual void setPosition(const Position3& position) { this->position = position; }
        virtual void setIntensity(const Vector3& intensity) { this->intensity = intensity; }
};

#endif