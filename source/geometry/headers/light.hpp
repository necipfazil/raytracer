#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vector3.hpp"
#include "position3.hpp"

class Scene;
struct HitInfo;

struct IncidentLight {
    bool inShadow;
    Vector3 intensity;
    Vector3 hitToLightDirection;
};

class Light
{
    public:
        // Compute light incident to position. Shadow check is also done by considering the scene.
        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const = 0;
};

#endif