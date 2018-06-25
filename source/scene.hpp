#ifndef __SCENE_H__
#define __SCENE_H__

#include "geometry/headers/geometry.hpp"
#include "geometry/headers/spherical_env_light.hpp"
#include "image/image.hpp"
#include "image/color.hpp"
#include "utility/concurrent_bag.hpp"
#include "utility/pixel_mission_generator.hpp"
#include "filemanip/tinyxml2.h"
#include "geometry/headers/transformation.hpp"
#include "geometry/headers/light.hpp"
#include "geometry/headers/brdf.hpp"
#include "geometry/headers/enums.hpp"
#include <string>

class Scene
{
    private:

        Integrator integrator = Integrator::DEFAULT;

        Color backgroundColor;
        SphericalEnvLight* sphericalEnvLight = nullptr;

        Vector3 ambientLight;

        float shadowRayEpsilon;
        int maxRecursionDepth;
        
        std::vector<Camera> cameras;
        std::vector<Light*> lights;
        std::vector<Material> materials;
        std::vector<Vertex> vertexData;

        Shape* BVH;

        // the reason why getRayColor(), getReflectionColor(), isLyingInShadow()
        // .. methods are non-static is that they are dependent on the Shape's included in the scene
        // therefore, they require to access the self's bounding volume hiearchy
        Color getRayColor(const Ray & ray, int recursionDepth, bool backfaceCulling, bool onlyOpaque=false) const;
        Color getReflectionColor(const Ray & ray, const HitInfo & hitInfo, int recursionDepth) const;
        Color getRefractionColor(const Ray & hittingRay, const HitInfo & hitInfo, int recursionDepth) const;

        Ray createRefractionRay(const Ray & hittingRay, const HitInfo & hitInfo) const;

        // new methods
        Color getDiffuseColor(const Material & material, const HitInfo & hitInfo, const IncidentLight& incidentLight) const;
        Color getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const IncidentLight& incidentLight) const;

        Color getAmbientColor(const Material & material, const Vector3 & ambientLight) const;

        #ifdef __CONCURRENT_BAG_TASK_DIST__
        static void imageFiller(Camera * camera, Image * image, Scene * scene, ConcurrentBag<Vec2i> * missionsBag);
        #else
        static void imageFiller(Camera * camera, Image * image, Scene * scene, PixelMissionGenerator * pixelMissionGenerator);
        #endif
    public:
        
        ~Scene()
        {
            // BVH
            if(BVH)
            {
                delete BVH;
                BVH = nullptr;
            }

            // lights
            for(int i = 0; i < lights.size(); i++)
            {
                if(lights[i])
                    delete lights[i];

                lights[i] = nullptr;    
            }

            lights.clear();

            // spherical environment light
            if(this->sphericalEnvLight)
            {
                delete this->sphericalEnvLight;
                this->sphericalEnvLight = nullptr;
            }
        }
        
        void loadFromXml(const std::string& filepath);
        void generateImages(unsigned short numberOfThreads);

        float getShadowRayEpsilon() const { return this->shadowRayEpsilon; }
        Shape* getBVH() const { return this->BVH; }
};

#endif
