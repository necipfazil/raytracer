#include "../config.h"
#include "../scene.hpp"
#include "../geometry/headers/geometry.hpp"
#include "../image/color.hpp"
#include "../image/image.hpp"
#include "../utility/concurrent_bag.hpp"
#include "../utility/pixel_mission_generator.hpp"
#include "../geometry/headers/directional_light.hpp"
#include <forward_list>
#include <cmath>

// point light
Color Scene::getDiffuseColor(const Material & material, const HitInfo & hitInfo, const PointLight & pointLight)
{ 
    // hit to light
    Vector3 hit2light = pointLight.getPosition() - hitInfo.hitPosition;
                        
    // distanceSquare
    float distanceSq = hit2light ^ hit2light;

    // intensity
    Vector3 intensity = pointLight.getIntensity() / distanceSq;

    hit2light.normalize();
    
    float normalDotLight = (hitInfo.normal ^ hit2light);
    
    if(normalDotLight < 0.0f)
        normalDotLight = 0.0f;
        
    Color diffuseColor = Color(material.getDiffuse().intensify(intensity) * normalDotLight);

    return diffuseColor;
}

// spot light
Color Scene::getDiffuseColor(const Material & material, const HitInfo & hitInfo, const SpotLight & spotLight)
{
    // intensity
    Vector3 intensity = spotLight.getIntensity(hitInfo.hitPosition);
    
    Vector3 hit2light = hitInfo.hitPosition.to(spotLight.getPosition()).normalize();

    float normalDotLight = (hitInfo.normal ^ hit2light);
    
    if(normalDotLight < 0.0f)
        normalDotLight = 0.0f;
        
    Color diffuseColor = Color(material.getDiffuse().intensify(intensity) * normalDotLight);

    return diffuseColor;
}

// directional light
Color Scene::getDiffuseColor(const Material & material, const HitInfo & hitInfo, const DirectionalLight & dirLight)
{   
    float normalDotLight = hitInfo.normal ^ dirLight.getReverseDirection();
    
    if(normalDotLight < 0.0f)
        normalDotLight = 0.0f;
    
    Color diffuseColor = Color(material.getDiffuse().intensify(dirLight.getRadiance()) * normalDotLight);

    return diffuseColor;
}

// directional light
Color Scene::getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const DirectionalLight & dirLight)
{
    Vector3 l = dirLight.getReverseDirection();
    Vector3 intensity = dirLight.getRadiance();
    
    Vector3 h = (( -ray.getDirection()) + l ).normalize();
    
    float max = hitInfo.normal ^ h;
    max = max < 0.0f ? 0.0f : max;
    max = pow(max, material.getPhongExponent());
    
    Vector3 colorVector = material.getSpecular().intensify(intensity) * max;
    
    return Color(colorVector);
}

// point light
Color Scene::getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const PointLight & pointLight)
{
    Vector3 hit2light = hitInfo.hitPosition.to(pointLight.getPosition());
    float distanceSq = hit2light ^ hit2light;
    
    Vector3 l = hit2light;
    l.normalize();
    
    Vector3 intensity = pointLight.getIntensity() / distanceSq;
    
    Vector3 h = (( -ray.getDirection()) + l ).normalize();
    
    float max = hitInfo.normal ^ h;
    max = max < 0.0f ? 0.0f : max;
    max = pow(max, material.getPhongExponent());
    
    Vector3 colorVector = material.getSpecular().intensify(intensity) * max;
    
    return Color(colorVector);
}

// spot light
Color Scene::getSpecular(const Ray & ray, const Material & material, const HitInfo & hitInfo, const SpotLight & spotLight)
{
    Vector3 l = hitInfo.hitPosition.to(spotLight.getPosition()).normalize();
    
    Vector3 intensity = spotLight.getIntensity(hitInfo.hitPosition);
    
    Vector3 h = (( -ray.getDirection()) + l ).normalize();
    
    float max = hitInfo.normal ^ h;
    max = max < 0.0f ? 0.0f : max;
    max = pow(max, material.getPhongExponent());
    
    Vector3 colorVector = material.getSpecular().intensify(intensity) * max;
    
    return Color(colorVector);
}

Color Scene::getAmbientColor(const Material & material, const Vector3 & ambientLight)
{
    return Color(ambientLight.intensify(material.getAmbient()));
}

// for point light
bool Scene::isLyingInShadow(const Position3 & hitPosition, const Position3 & lightPosition, float time) const
{
    // first, create the shadow ray
    Ray shadowRay(hitPosition, hitPosition.to(lightPosition));

    // set time for ray creation
    shadowRay.setTimeCreated(time);
    
    // move ray's origin with epsilon
    shadowRay.translateRayOrigin(this->shadowRayEpsilon);

    HitInfo shadowRayHitInfo;
    // epsilon
    if( this->BVH->hit(shadowRay, shadowRayHitInfo, true) )
    {   
        float hitPointToLightT = shadowRay.getTValue(lightPosition);
        
        if(hitPointToLightT < shadowRayHitInfo.t)
        {
            // cheers! point is closer to us
            return false;
        }
        else
        {
            // in shadow..
            return true;
        }
    }
    else
    {
        return false;
    }
}

// for directional light
bool Scene::isLyingInShadow(const Position3 & hitPosition, const DirectionalLight & dirLight, float time) const
{
    // create shadow ray
    Ray shadowRay(hitPosition, dirLight.getReverseDirection());

    // set time for ray creation
    shadowRay.setTimeCreated(time);
    
    // move ray's origin with epsilon
    shadowRay.translateRayOrigin(this->shadowRayEpsilon);

    HitInfo shadowRayHitInfo;

    return this->BVH->hit(shadowRay, shadowRayHitInfo, true);
}

Color Scene::getReflectionColor(const Ray & ray, const HitInfo & hitInfo, int recursionDepth) const
{
    if(recursionDepth == 0)
    {
        return Color::Black();
    }

    Ray reflectionRay = ray.createReflectionRay(hitInfo);

    // set time
    reflectionRay.setTimeCreated(ray.getTimeCreated());

    return getRayColor(reflectionRay, recursionDepth - 1, true);
}

bool isRefracted(float refractionIndexRatio, float cosTheta)
{
    float delta = 1 -  pow(refractionIndexRatio, 2) * (1 - pow(cosTheta, 2));

    return delta > 0.f;
}


Color Scene::getRefractionColor(const Ray & hittingRay, const HitInfo & hitInfo, int recursionDepth) const
{
    if(recursionDepth == 0)
    {
        return Color::Black();
    }

    Color color = Color::Black();

    bool isEntering;
    float refractionIndexRatio;
    Vector3 normal;
    HitInfo reflectionRayHitInfo = hitInfo; // its normal will be manipulated  
    
    // check if the ray is entering to or was already inside the refractive object
    // .. compute refractionIndexRatio and normal according to entering/exitting case
    float cosTheta = (-hittingRay.getDirection()) ^ hitInfo.normal;

    if(cosTheta >= 0.f) // entering
    {
        isEntering = true;
        refractionIndexRatio = AIR_REFRACTION_INDEX / hitInfo.material.getRefractionIndex();
        normal = hitInfo.normal;
    }
    else // was already inside the refractive object
    {
        isEntering = false;
        refractionIndexRatio = hitInfo.material.getRefractionIndex() / AIR_REFRACTION_INDEX;
        normal = -hitInfo.normal;
        reflectionRayHitInfo.normal = - reflectionRayHitInfo.normal;
    }

    float theta = acos(cosTheta);
    float sinTheta = sin(theta);
    float sinPhi = refractionIndexRatio * sinTheta;
    float phi = asin(sinPhi);
    float cosPhi = cos(phi);
    
    // take abs of cosPhi and cosTheta since we are looking for the acute angle
    cosPhi = cosPhi < 0.f ? -cosPhi : cosPhi;
    cosTheta = cosTheta < 0.f ? -cosTheta : cosTheta;

    // compute R_0
    float R_0 = pow(
        (hitInfo.material.getRefractionIndex() - AIR_REFRACTION_INDEX)
        / (hitInfo.material.getRefractionIndex() + AIR_REFRACTION_INDEX)
    , 2);

    // compute R_theta
    float R_theta = R_0 + (1 - R_0) * pow(1 - cosTheta, 5);
    if(!std::isnan(cosPhi) && cosPhi < cosTheta)
    {
        R_theta = R_0 + (1 - R_0) * pow(1 - cosPhi, 5);
    }

    // init refraction and reflection color as black
    Color refractionColor = Color::Black();
    Color reflectionColor = Color::Black();

    // create reflection ray
    Ray reflectionRay = hittingRay.createReflectionRay(reflectionRayHitInfo);

    // set time
    reflectionRay.setTimeCreated(hittingRay.getTimeCreated());

    // get reflected ray's color
    reflectionColor = getRayColor(reflectionRay, recursionDepth - 1, false);

    if(isRefracted(refractionIndexRatio, cosTheta))
    {
        // compute t, which is the direction of the refracted ray
        Vector3 t =
            ((hittingRay.getDirection() + (normal * cosTheta)) *  refractionIndexRatio) - (normal * cosPhi);
        
        // create the refracted ray, origined at hit position(+epsilon) and directed as t
        Ray refractedRay = Ray(hitInfo.hitPosition, t).translateRayOrigin(shadowRayEpsilon);

        // set time
        refractedRay.setTimeCreated(hittingRay.getTimeCreated());

        // set refracted ray's color
        refractionColor = getRayColor(refractedRay, recursionDepth - 1, false)
            .intensify(Vector3(1 - R_theta));

        // intensify reflection color
        reflectionColor.intensify(Vector3(R_theta));
    }

    if(!isEntering)
    {
        // if the ray was inside, attenuation should be applied to the color
        Vector3 attenuation = hitInfo.material.getTransparency().power(hitInfo.t);

        refractionColor.intensify(attenuation);
        reflectionColor.intensify(attenuation);
    }

    color += refractionColor;
    color += reflectionColor;

    return color;
}

Color Scene::getRayColor(const Ray & ray, int recursionDepth, bool backfaceCulling) const
{
    HitInfo hitInfo;
    
    if( BVH->hit(ray, hitInfo, backfaceCulling) )
    {
        Material & material = hitInfo.material;

        bool shapeIsFacing = (hitInfo.normal ^ ray.getDirection()) < 0;

        Color color(0.0f, 0.0f, 0.0f);

        // ambient, diffuse, specular and reflection - if shape is facing
        if(!backfaceCulling || shapeIsFacing)
        {
            if(hitInfo.textureInfo.hasTexture && hitInfo.textureInfo.decalMode == DecalMode::REPLACE_ALL)
            {
                return hitInfo.textureInfo.textureColor;
            }

            // ambient
            color += getAmbientColor(material, this->ambientLight);
            
            // traverse point lights
            for(int p = 0; p < this->pointLights.size(); p++ )
            {
                const PointLight & pointLight = this->pointLights[p];
                
                // if light is not seenable, continue
                if(shapeIsFacing && !isLyingInShadow(hitInfo.hitPosition, pointLight.getPosition(), ray.getTimeCreated()) )
                {
                    // diffuse
                    color += getDiffuseColor(material, hitInfo, pointLight);
                    
                    if((hitInfo.normal ^ ray.getDirection()) < 0)
                    // specular
                    color += getSpecular(ray, material, hitInfo, pointLight);
                }
            }

            // traverse area lights
            for(int a = 0; a < this->areaLights.size(); a++ )
            {
                // get an instance of pointlight from arealight
                const PointLight pointLight = this->areaLights[a].getPointLight(hitInfo.hitPosition);
                
                // if light is not seenable, continue
                if(shapeIsFacing && !isLyingInShadow(hitInfo.hitPosition, pointLight.getPosition(), ray.getTimeCreated()) )
                {
                    // diffuse
                    color += getDiffuseColor(material, hitInfo, pointLight);
                    
                    if((hitInfo.normal ^ ray.getDirection()) < 0)
                    // specular
                    color += getSpecular(ray, material, hitInfo, pointLight);
                }
            }

            // traverse directional lights
            for(int d = 0; d < this->directionalLights.size(); d++)
            {
                const DirectionalLight & dirLight = this->directionalLights[d];

                if(shapeIsFacing && !isLyingInShadow(hitInfo.hitPosition, dirLight, ray.getTimeCreated()))
                {
                    // diffuse
                    color += getDiffuseColor(material, hitInfo, dirLight);

                    // specular
                    color += getSpecular(ray, material, hitInfo, dirLight);
                }
            }

            // traverse spot lights
            for(int s = 0; s < this->spotLights.size(); s++)
            {
                const SpotLight& spotLight = this->spotLights[s];

                if(shapeIsFacing && !isLyingInShadow(hitInfo.hitPosition, spotLight.getPosition(), ray.getTimeCreated()))
                {
                    // diffuse
                    color += getDiffuseColor(material, hitInfo, spotLight);

                    // specular
                    color += getSpecular(ray, material, hitInfo, spotLight);
                }
            }
            
            // reflection
                // check if it has mirrorish material 
            if(!material.getMirror().isZeroVector())
            {
                if(recursionDepth)
                    color += getReflectionColor(ray, hitInfo, recursionDepth).intensify(material.getMirror());
            }
        }
        
        // refraction
            // check if the material has refractive character
        if(!material.getTransparency().isZeroVector())
        {
            if(recursionDepth)
            {
                Color refractionColor = this->getRefractionColor(ray, hitInfo, recursionDepth);
                
                color += refractionColor;
            }
            
        }
        
        return color;           
    }
    else
    {
        return this->backgroundColor;
    }
}
