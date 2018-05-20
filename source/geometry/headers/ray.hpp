#ifndef __RAY_H__
#define __RAY_H__

#include "structs.hpp"
#include "position3.hpp"
#include "vector3.hpp"
//#include "surface.hpp"

class Ray
{
    private:
        Position3 origin;
        Vector3 direction;
        float weight = 1.f;
        float timeCreated;
        
    public:
        Ray() {
            this->origin = Position3();
            this->direction = Vector3();
        }
        
        Ray(Position3 origin, Vector3 direction)
            : origin(origin), direction(direction) { this->direction.normalize(); };
            
            
        Position3 getOrigin() const { return this->origin; }
        Vector3 getDirection() const { return this->direction; }
        float getWeight() const { return this->weight; }
        float getTimeCreated() const { return this->timeCreated; }
        
        void setOrigin(Position3 position) { this->origin = position; }
        void setDirection(Vector3 direction) { this->direction = direction.normalize(); }
        void setWeight(float weight) { this->weight = weight; }
        void setTimeCreated(float timeCreated) { this->timeCreated = timeCreated; }
        
        Ray createReflectionRay(const HitInfo &) const;
        Position3 getPoint(const float & t) const;

        // given a float value (an epsilon value is likely to be used here), moves the origin of the
        // .. ray in its direction by this float value
        Ray & translateRayOrigin(const float val);
        
        // used only for debugging
        /*bool getClosestHit(
            HitInfo & hitInfo,  // return the hit info
            const std::vector<Surface*> surfaces,   // feed surfaces
            float epsilon);*/
                           
        float getTValue(const Position3 & hitPosition) const;
       
};

#endif