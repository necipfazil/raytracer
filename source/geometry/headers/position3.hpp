#ifndef __POSITION3_H__
#define __POSITION3_H__

#include "vector3.hpp"

class Position3
{
    protected:
        float x, y, z;
        
    public:
        Position3(float x, float y, float z)
            : x(x), y(y), z(z) {}

        Position3()
            : x(0.f), y(0.f), z(0.f) {}

        Position3(float xyz)
            : x(xyz), y(xyz), z(xyz) {}
        
        float getX() const;
        float getY() const;
        float getZ() const;
        
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        
        // substraction of two points yields a vector
        Vector3 operator-(const Position3 & rhs) const;

        // addition of Position3 with a Vector3 yields displacement and returns a new Position3
        Position3 operator+(const Vector3 & rhs) const;
        
        // a vector from this to rhs
        Vector3 to(const Position3 & rhs) const;
        
        float distanceSquare(const Position3 & rhs) const;

        static Position3 generateMinPosition(const Position3 & p1, const Position3 & p2);
        static Position3 generateMaxPosition(const Position3 & p1, const Position3 & p2);

        static bool compareLTX(const Position3 & lhs, const Position3 & rhs);
        static bool compareLTY(const Position3 & lhs, const Position3 & rhs);
        static bool compareLTZ(const Position3 & lhs, const Position3 & rhs);
};

#endif