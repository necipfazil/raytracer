#ifndef __SPOT_LIGHT_H__
#define __SPOT_LIGHT_H__

#include "position3.hpp"
#include "vector3.hpp"

#include <cmath>

#include <iostream>
using namespace std;

class SpotLight
{
    private:
        Position3 position;
        Vector3 direction;
        Vector3 intensity;
        float coverageAngle;
        float falloffAngle;

        float cosCovAngle, cosFalloffAngle;
        float cosDiff; // cos(alfa/2) - cos(beta/2)
    public:
        SpotLight(const Position3& position, const Vector3& direction, const Vector3& intensity, float coverageAngle, float falloffAngle)
            : position(position), direction(direction), intensity(intensity), coverageAngle(coverageAngle), falloffAngle(falloffAngle)
            {
                this->direction.normalize();

                cosCovAngle = cos((coverageAngle / 2.f) * (M_PI / 180.f));
                cosFalloffAngle = cos((falloffAngle / 2.f) * (M_PI / 180.f));

                cosDiff = cosFalloffAngle - cosCovAngle;
            }

        Position3 getPosition() const {
            return this->position;
        }

        Vector3 getDirection() const {
            return this->direction;
        }
        
        Vector3 getIntensity(const Position3& q) const {
            Vector3 hit2light = q - this->position;

            float distanceSquare = hit2light ^ hit2light;

            float cosTheta = Vector3(hit2light).normalize() ^ this->direction;

            //cout << "CosTheta: " << cosTheta << ", cosFalloffAngle: " << cosFalloffAngle << ", cosCovAngle: " << cosCovAngle << endl;

            if(cosTheta > cosFalloffAngle)
            {
                // full illumunation
                return this->intensity / distanceSquare;
            }
            else if(cosTheta < cosCovAngle)
            {
                // no illumunation
                return Vector3(0.f);
            }
            else
            {
                // in fall-off zone
                float falloffFactor = (cosTheta - cosCovAngle) / cosDiff;

                // avoid linear drop
                falloffFactor = pow(falloffFactor, 4);

                return (this->intensity * falloffFactor) / distanceSquare;
            }
        }

        float getCoverageAngle() const {
            return this->coverageAngle;
        }

        float getFalloffAngle() const {
            return this->falloffAngle;
        }
};

#endif