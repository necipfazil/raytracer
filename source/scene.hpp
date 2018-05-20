#ifndef __SCENE_H__
#define __SCENE_H__

#include "geometry/headers/geometry.hpp"
#include "image/image.hpp"
#include "image/color.hpp"
#include "utility/concurrent_bag.hpp"
#include "utility/pixel_mission_generator.hpp"
#include "filemanip/tinyxml2.h"
#include "geometry/headers/transformation.hpp"
#include "geometry/headers/directional_light.hpp"
#include "geometry/headers/spot_light.hpp"
#include <string>

class Scene
{
    private:

        Color backgroundColor;
        Vector3 ambientLight;

        float shadowRayEpsilon;
        int maxRecursionDepth;
        
        std::vector<Camera> cameras;
        std::vector<PointLight> pointLights;
        std::vector<AreaLight> areaLights;
        std::vector<DirectionalLight> directionalLights;
        std::vector<SpotLight> spotLights;
        std::vector<Material> materials;
        std::vector<Vertex> vertexData;

        Shape* BVH;

        // the reason why getRayColor(), getReflectionColor(), isLyingInShadow()
        // .. methods are non-static is that they are dependent on the Shape's included in the scene
        // therefore, they require to access the self's bounding volume hiearchy
        Color getRayColor(const Ray & ray, int recursionDepth, bool backfaceCulling) const;
        Color getReflectionColor(const Ray & ray, const HitInfo & hitInfo, int recursionDepth) const;
        Color getRefractionColor(const Ray & hittingRay, const HitInfo & hitInfo, int recursionDepth) const;

        Ray createRefractionRay(const Ray & hittingRay, const HitInfo & hitInfo) const;

        bool isLyingInShadow(const Position3 & hitPosition, const Position3& lightPosition, float time) const;
        bool isLyingInShadow(const Position3 & hitPosition, const DirectionalLight & dirLight, float time) const;

        // point light color computations
        static Color getDiffuseColor(const Material & material, const HitInfo & hitInfo, const PointLight & pointLight);
        static Color getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const PointLight & pointLight);
        static Color getAmbientColor(const Material & material, const Vector3 & ambientLight);

        // directional light color computations
        static Color getDiffuseColor(const Material & material, const HitInfo & hitInfo, const DirectionalLight & dirLight);
        static Color getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const DirectionalLight & dirLight);
        
        // spot light color computations
        static Color getDiffuseColor(const Material & material, const HitInfo & hitInfo, const SpotLight & spotLight);
        static Color getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const SpotLight & spotLight);
        
        #ifdef __CONCURRENT_BAG_TASK_DIST__
        static void imageFiller(Camera * camera, Image * image, Scene * scene, ConcurrentBag<Vec2i> * missionsBag);
        #else
        static void imageFiller(Camera * camera, Image * image, Scene * scene, PixelMissionGenerator * pixelMissionGenerator);
        #endif
    public:
        
        ~Scene()
        {
            if(BVH)
            {
                delete BVH;
                BVH = NULL;
            }
        }
        
        void loadFromXml(const std::string& filepath);
        void generateImages(unsigned short numberOfThreads);
};

#endif
