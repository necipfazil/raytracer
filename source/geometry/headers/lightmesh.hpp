#ifndef __LIGHT_MESH_H__
#define __LIGHT_MESH_H__

#include "shape.hpp"
#include "boundingvolume.hpp"
#include <iostream>

class LightMesh: public Light, public Shape
{
    private:
        Vector3 radiance;
        BoundingVolume* mesh = nullptr;
    public:
        LightMesh(BoundingVolume* mesh, Vector3 radiance)
            : radiance(radiance)
        {
            if(mesh)
            {
                this->mesh = BoundingVolume::makeInstanceOf(mesh);

                this->minPosition = this->mesh->getMinPosition();
                this->maxPosition = this->mesh->getMaxPosition();
            }
        }
        
        LightMesh(const LightMesh& rhs)
            : radiance(rhs.radiance), mesh(rhs.mesh)
        {
            if(mesh)
            {
                mesh = BoundingVolume::makeInstanceOf(rhs.mesh);

                this->minPosition = this->mesh->getMinPosition();
                this->maxPosition = this->mesh->getMaxPosition();
            }
                
        }

        LightMesh& operator=(const LightMesh& rhs)
        {
            // destruct
            if(this->mesh)
            {
                delete this->mesh;
                this->mesh = nullptr;
            }

            // copy
            this->radiance = rhs.radiance;
            
            if(rhs.mesh)
            {
                this->mesh = BoundingVolume::makeInstanceOf(rhs.mesh);

                this->minPosition = this->mesh->getMinPosition();
                this->maxPosition = this->mesh->getMaxPosition();
            }
                
        }

        ~LightMesh() { if(mesh) delete mesh; }

        virtual IncidentLight getIncidentLight(const Scene& scene, const HitInfo& hitInfo, float time) const;

        // Decorator
        virtual bool hit(const Ray& ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const;

        void setRadiance(const Vector3& radiance) { this->radiance = radiance; }
        void setMesh(BoundingVolume* mesh) { this->mesh = mesh; }

        virtual Position3 getUniformPoint() const;
};

#endif