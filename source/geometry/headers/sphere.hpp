#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "surface.hpp"
#include "position3.hpp"
#include "structs.hpp"
#include "ray.hpp"

class Sphere : public Surface
{
    private:
        Position3 center;
        float radius;
        
    public:
        Position3 getCenter() const { return this->center; }
        float getRadius() const { return this->radius; }
        
        Sphere(Position3 center, float radius, const Material & material)
            : Surface(material), center(center), radius(radius) 
        {
            // minPosition
            this->minPosition = Position3(
                this->center.getX() - radius,
                this->center.getY() - radius,
                this->center.getZ() - radius
            );

            // maxPosition
            this->maxPosition = Position3(
                this->center.getX() + radius,
                this->center.getY() + radius,
                this->center.getZ() + radius
            );
        }
        
        bool isIntersecting(const Ray & ray) const;
        
        float discriminant(const Ray & ray) const;
       
        bool hit(const Ray & ray, HitInfo & hitInfo, bool backfaceCulling) const;
};

#endif