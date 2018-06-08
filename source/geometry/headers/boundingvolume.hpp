#ifndef __BOUNDINGVOLUME_H__
#define __BOUNDINGVOLUME_H__

#include "shape.hpp"
#include "position3.hpp"
#include "structs.hpp"
#include "enums.hpp"
#include "ray.hpp"
#include "transformation.hpp"

#include <vector>

// bounding volume
class BoundingVolume : public Shape
{
    private:
        Shape *leftNode, *rightNode;

        // to be improved by making use of shared_ptr
        bool ownsChildren = false;
        
        static Shape* createBoundingVolumeHiearchy(
            std::vector<Shape*> &shapes,  // surfaces
            int firstInd, int numOfSurfaces,  // indices of first and last, used to determine the surfaces to consider
            Axis divisionAxis
        );

        static Axis nextDivisionAxis(Axis currentAxis);

        static int partitionShapesIntoTwo(
            std::vector<Shape*> &shapes,
            int firstInd, int numOfShapes,
            Axis partitionAxis
        );
        
        BoundingVolume() : leftNode(nullptr), rightNode(nullptr) { }

        bool isEnclosing(const Position3 & point) const;

    public:
        // it is not known if the return Shape* points to a BoundingVolume or a Surface
        // therefore, it is not safe to resolve it to a concrete object
        // also, since not intented to be used, copy constructor of the bounding volume class
        // is not implemented, which could result in undefined behaviour upon resolving the
        // returned pointer of the method and, somehow, copying it
        static Shape* createBoundingVolumeHiearchy(std::vector<Shape*> &shapes);

        static BoundingVolume* makeInstanceOf(BoundingVolume* toBeInstanced);
        
        bool hit(const Ray & ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const;

        virtual ~BoundingVolume();
        
};

#endif