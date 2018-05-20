#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "position3.hpp"
#include "vector3.hpp"

class Vertex : public Position3
{
    private:
        Vector3 normal;

    public:
        Vertex(float x, float y, float z)
            : Position3(x, y, z), normal(0.f) { }

        Vertex(const Position3 & position)
            : Position3(position), normal(0.f) { }

        // normal of a vertex will be the average of the normals
        // of the triangles it is included in. therefore, each triangle should
        // have a mechanism to add its normal to vertices it has
        void addToNormal(const Vector3 & normalOfTriangle)
        {
            this->normal = this->normal + normalOfTriangle;
        }

        void normalizeNormal()
        {
            if(!this->normal.isZeroVector())
            {
                this->normal.normalize();
            }
                
        }

        Vector3 getNormal() const
        {
            return this->normal;
        }
};

#endif