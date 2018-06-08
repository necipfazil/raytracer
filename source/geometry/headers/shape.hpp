#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "ray.hpp"
#include "structs.hpp"
#include "position3.hpp"
#include "transformation.hpp"
#include "matrix4.hpp"

#include <vector>

// abstract class for shapes that can be hit by a ray
class Shape
{
    protected:
        // A shape may cover the material - or not. Before delivering its
        // .. material to top, it is better to think if there is valuable
        // .. information inside material!
        bool hasMaterial = false;
        Material material;

        Position3 minPosition, maxPosition;

        bool hasTransformation = false;
        Transformation transformation;

        bool hasMotionBlur = false;
        Vector3 motionBlur;

        std::vector<Position3> getAllVertices() const;

        Ray transformRayForIntersection(const Ray & originalRay) const;
        void transformHitInfoAfterIntersection(const Ray & originalRay, HitInfo & hitInfo) const;

        Shape() {}

        Shape(const Material& material) : material(material), hasMaterial(true) { }
    public:
        void setMaterial(const Material& material)
        {
            this->hasMaterial = true;
            this->material = material;
        }

        virtual bool hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const = 0;

        // destructor
        virtual ~Shape() { };

        // is hit available when min and max position considered
        virtual bool liangbarskyHit(const Ray & ray) const;

        // to be used in BoundingVolume class
        // every Shape should have a min and max position to be used while
        // constructing the bounding volume hiearchy
        Position3 getMinPosition() const { return this->minPosition; }
        Position3 getMaxPosition() const { return this->maxPosition; }

        void transform(const Transformation& transformation);
        void setMotionBlur(const Vector3& motionBlur);

        static bool compareLTX(const Shape * lhs, const Shape * rhs);
        static bool compareLTY(const Shape * lhs, const Shape * rhs);
        static bool compareLTZ(const Shape * lhs, const Shape * rhs);
};

#endif