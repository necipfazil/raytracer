#include "../headers/sphere.hpp"
#include <cmath>

float Sphere::discriminant(const Ray & ray) const
{
    const Vector3 & direction = ray.getDirection();
    const Position3 & rayOrigin = ray.getOrigin();
    
    const Vector3 common = center.to(rayOrigin);
    // see textbook p77
    float A = direction ^ common;
    float B = direction ^ direction ;
    float C = common ^ common;
    
    return (A * A) - B * (C - (radius * radius));
}

bool Sphere::isIntersecting(const Ray & ray) const
{
    return discriminant(ray) >= 0.0;
}

bool Sphere::hit(const Ray & originalRay, HitInfo & hitInfo, bool backfaceCulling) const
{
    // set time of hit
    hitInfo.time = originalRay.getTimeCreated();
    
    Ray ray = transformRayForIntersection(originalRay);

    float disc = discriminant(ray);
    
    float A = ( ray.getOrigin() - center ) ^ ( ray.getDirection() * (-1) );
    float B = ray.getDirection() ^ ray.getDirection(); 
    
    // we have two intersection points
    if(disc > 0.0f) 
    {
        float t1 = ( A + sqrt(disc) ) / B ;
        float t2 = ( A - sqrt(disc) ) / B ;

        // the hits occured in inverse direction are not taken into account
        if(t1 < 0 && t2 < 0)
            return false;

        // take the smallest t
        hitInfo.t = t1 > t2 ? t2 : t1;

        // if one of them is negative, take the other one since we are looking for the smallest positive t
        if(t2 < 0)
            hitInfo.t = t1;
        else if(t1 < 0)
            hitInfo.t = t2;
        
        // fill hitinfo
        hitInfo.normal = (this->getCenter().to(ray.getPoint(hitInfo.t))).normalize();
        hitInfo.hitPosition = ray.getPoint(hitInfo.t);
        if(this->hasMaterial)
            hitInfo.material = this->material;

        // texture info
        hitInfo.textureInfo.hasTexture = this->hasImageTexture || this->hasPerlinTexture;

        if(this->hasImageTexture)
        {
            hitInfo.textureInfo.decalMode = imageTexture.getDecalMode();

            Vector3 centerToHitPosition = center.to(hitInfo.hitPosition);

            // compute theta and fi angles
            float theta = acos(centerToHitPosition.getY() / radius);
            float fi    = atan2(centerToHitPosition.getZ(), centerToHitPosition.getX());

            // compute u and v
            float u = (-fi + M_PI) / (2 * M_PI);
            float v = theta / M_PI;

            // assign color
            hitInfo.textureInfo.textureColor = imageTexture.getInterpolatedColor(u, v);

            // check decal mode
            if(imageTexture.getDecalMode() == DecalMode::REPLACE_KD)
            {
                hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
            }
            else if(imageTexture.getDecalMode() == DecalMode::BLEND_KD)
            {
                hitInfo.material.setDiffuse(
                    ((hitInfo.textureInfo.textureColor.getVector3()) + hitInfo.material.getDiffuse()) / 2.f
                );
            }

            if(imageTexture.isBump())
            {
                // compute dpdu and dpdv
                float dxdu = 2 * M_PI * centerToHitPosition.getZ();
                float dydu = 0;
                float dzdu = -2 * M_PI * centerToHitPosition.getX();

                float dxdv = M_PI * centerToHitPosition.getY() * cos(fi);
                float dydv = -1 * M_PI * radius * sin(theta);
                float dzdv = M_PI * centerToHitPosition.getY() * sin(fi);

                Vector3 dpdu = Vector3(dxdu, dydu, dzdu);
                Vector3 dpdv = Vector3(dxdv, dydv, dzdv);

                Vec2f grd = imageTexture.getGradient(u, v);

                Vector3 dpPrimedu = dpdu + (hitInfo.normal * grd.u);
                Vector3 dpPrimedv = dpdv + (hitInfo.normal * grd.v);

                // update normal
                hitInfo.normal = (dpPrimedv * dpPrimedu).normalize();
            }
            
        }
        else if(this->hasPerlinTexture)
        {
            hitInfo.textureInfo.decalMode = perlinTexture.getDecalMode();
            hitInfo.textureInfo.textureColor = perlinTexture.getPerlinColor(hitInfo.hitPosition);

            // check decal mode
            if(perlinTexture.getDecalMode() == DecalMode::REPLACE_KD)
            {
                hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
            }
            else if(perlinTexture.getDecalMode() == DecalMode::BLEND_KD)
            {
                hitInfo.material.setDiffuse(
                    ((hitInfo.textureInfo.textureColor.getVector3()) + hitInfo.material.getDiffuse()) / 2.f
                );
            }

            if(perlinTexture.isBump())
            {
                Vector3 gradient = perlinTexture.getPerlinColor(hitInfo.hitPosition).getVector3();

                Vector3 gParallel = hitInfo.normal * (gradient ^ hitInfo.normal);
                Vector3 gOrth = gradient - gParallel;

                // update normal
                hitInfo.normal = (hitInfo.normal - gOrth).normalize();
            }
        }

        // apply transformation to hitInfo if required
        transformHitInfoAfterIntersection(originalRay, hitInfo);

        return true;    
    }
    // the ray grazes
    else if (disc == 0.0f) 
    {
        hitInfo.t = A / B;
        
        if(hitInfo.t > 0.0f)
        {
            // fill hitinfo
            hitInfo.normal = (this->getCenter().to(ray.getPoint(hitInfo.t))).normalize();
            hitInfo.hitPosition = ray.getPoint(hitInfo.t);
            if(this->hasMaterial)
                hitInfo.material = this->material;

            // TODO: Write clearer
            // texture info
            hitInfo.textureInfo.hasTexture = this->hasImageTexture || this->hasPerlinTexture;

            if(this->hasImageTexture)
            {
                hitInfo.textureInfo.decalMode = imageTexture.getDecalMode();

                Vector3 centerToHitPosition = center.to(hitInfo.hitPosition);

                // compute theta and fi angles
                float theta = acos(centerToHitPosition.getY() / radius);
                float fi    = atan2(centerToHitPosition.getZ(), centerToHitPosition.getX());

                // compute u and v
                float u = (-fi + M_PI) / (2 * M_PI);
                float v = theta / M_PI;

                // assign color
                hitInfo.textureInfo.textureColor = imageTexture.getInterpolatedColor(u, v);

                // check decal mode
                if(imageTexture.getDecalMode() == DecalMode::REPLACE_KD)
                {
                    hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
                }
                else if(imageTexture.getDecalMode() == DecalMode::BLEND_KD)
                {
                    hitInfo.material.setDiffuse(
                        (hitInfo.textureInfo.textureColor.getVector3() + hitInfo.material.getDiffuse()) / 2.f
                    );
                }
            }
            else if(this->hasPerlinTexture)
            {
                hitInfo.textureInfo.decalMode = perlinTexture.getDecalMode();
                hitInfo.textureInfo.textureColor = perlinTexture.getPerlinColor(hitInfo.hitPosition);

                // check decal mode
                if(perlinTexture.getDecalMode() == DecalMode::REPLACE_KD)
                {
                    hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
                }
                else if(perlinTexture.getDecalMode() == DecalMode::BLEND_KD)
                {
                    hitInfo.material.setDiffuse(
                        ((hitInfo.textureInfo.textureColor.getVector3()) + hitInfo.material.getDiffuse()) / 2.f
                    );
                }

                if(perlinTexture.isBump())
                {
                    Vector3 gradient = perlinTexture.getPerlinColor(hitInfo.hitPosition).getVector3();

                    Vector3 gParallel = hitInfo.normal * (gradient ^ hitInfo.normal);
                    Vector3 gOrth = gradient - gParallel;

                    // update normal
                    hitInfo.normal = (hitInfo.normal - gOrth).normalize();
                }
            }

            // apply transformation to hitInfo if required
            transformHitInfoAfterIntersection(originalRay, hitInfo);

            return true;
        }
        else
        {
            return false;
        }
    }
    // no intersection
    else
    {
        return false;
    }
}