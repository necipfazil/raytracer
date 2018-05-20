#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "surface.hpp"
#include "vertex.hpp"
#include "vector3.hpp"
#include "enums.hpp"
#include "position3.hpp"
#include "vector3.hpp"
#include "ray.hpp"
#include "structs.hpp"


class Triangle : public Surface
{
    private:
        const Vertex vertex[3];
        const Vector3 normal;

        ShadingMode shadingMode;

        Vec2i texCoord[3];

        Position3 computeMinPosition() const;
        Position3 computeMaxPosition() const;
        
        void fillLookUpTable();
        
        struct LookUpTable
        {
            // A: vertex[0], B: vertex[1], C: vertex[2]
            float Ax_Bx;
            float Ay_By;
            float Az_Bz;
            float Ax_Cx;
            float Ay_Cy;
            float Az_Cz;
            Vector3 A_C;
            Vector3 A_B;
        };
        
        LookUpTable lookUpTable;
        
    public:
        // constructor with material
        Triangle(
            const Material & material,
            const Vertex & vertex0,
            const Vertex & vertex1,
            const Vertex & vertex2,
            const ShadingMode & shadingMode = DEFAULT_SHADING_MODE
        );

        // constructor without material
        Triangle(
            const Vertex & vertex0,
            const Vertex & vertex1,
            const Vertex & vertex2,
            const ShadingMode & shadingMode = DEFAULT_SHADING_MODE
        );
        
        Vertex getVertex(int vertexId) const;
        Vector3 getNormal() const;
    
        static Vector3 computeNormal(const Position3 & vertex0,
                                     const Position3 & vertex1,
                                     const Position3 & vertex2 );
        
        bool hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling) const;

        ShadingMode getShadingMode() const { return this->shadingMode; }
        void setShadingMode(ShadingMode shadingMode) { this->shadingMode = shadingMode; }

        void setTexCoord(const Vec2i& c0, const Vec2i& c1, const Vec2i& c2)
        {
            
            texCoord[0] = c0;
            texCoord[1] = c1;
            texCoord[2] = c2;
/*
            std::cout << "TexCoord 0: " << c0.x << ", " << c0.y << std::endl;
            std::cout << "TexCoord 1: " << c1.x << ", " << c1.y << std::endl;
            std::cout << "TexCoord 2: " << c2.x << ", " << c2.y << std::endl;*/
        }
};

#endif