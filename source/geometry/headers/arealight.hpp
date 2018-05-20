#ifndef __AREA_LIGHT_H__
#define __AREA_LIGHT_H__

#include "light.hpp"
#include "pointlight.hpp"
#include "../../utility/random_number_generator.hpp"

class AreaLight : public Light
{
    private:
        Vector3 edgeVectors[2];
        Vector3 normal; // should be used in reverse direction, too

        void computeAndSetNormal()
        {
            this->normal = this->edgeVectors[0] * this->edgeVectors[1];
            this->normal.normalize();
        }
    
    public:
        AreaLight(const Position3 & position, const Vector3 & intensity, const Vector3 edgeVectors[2])
            : Light(position, intensity)
            {
                this->edgeVectors[0] = edgeVectors[0];
                this->edgeVectors[1] = edgeVectors[1];
                computeAndSetNormal();
            }

        void setEdgeVector(Vector3 edgeVector, int index);
        Vector3 getNormal() const { return this->normal; }

        PointLight getPointLight(const Position3 & surfacePosition) const
        {
            Position3 randomPositionInsideArea = 
                this->position + 
                (edgeVectors[0] * getRandomBtw01()) +
                (edgeVectors[1] * getRandomBtw01());

            Vector3 fromLightToSource = randomPositionInsideArea.to(surfacePosition);

            float attenuationByNormal = fromLightToSource.normalize() ^ this->normal;

            // normal is bidirectional
            if(attenuationByNormal < 0.f)
                attenuationByNormal *= -1;

            // get attenuated intensity because of the angle made with normal
            Vector3 attenuatedIntensity = this->intensity * attenuationByNormal;

            // return the new point light
            return PointLight(randomPositionInsideArea, attenuatedIntensity);
        }
};

#endif