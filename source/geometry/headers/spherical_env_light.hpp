#ifndef __SPHERICAL_ENV_LIGHT_H__
#define __SPHERICAL_ENV_LIGHT_H__

#include "../../image/image.hpp"
#include "../../image/color.hpp"
#include "vector3.hpp"
#include "light.hpp"
#include <string>

class Scene;
struct HitInfo;

class SphericalEnvLight : public Light
{
    private:
        Image image;

        Color sampleTexture(float u, float v) const;
    public:
        SphericalEnvLight(std::string imagePath) : image(imagePath) { }

        Color getColor(const Vector3& direction) const;

        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const;
};

#endif