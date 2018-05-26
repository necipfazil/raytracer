#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <vector>

class Vector3
{
    private:
        float x, y, z;
        
    public:
        Vector3()
            : x(0.f), y(0.f), z(0.f) {}

        Vector3(float x, float y, float z)
            : x(x), y(y), z(z) {}

        Vector3(float xyz)
            : x(xyz), y(xyz), z(xyz) {}

        float getX() const;
        float getY() const;
        float getZ() const;
        
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        
        // unary (-) operator
        Vector3 operator-() const;
        
        // substraction
        Vector3 operator-(const Vector3 & rhs) const;
        
        // addition
        Vector3 operator+(const Vector3 & rhs) const;
        
        // dot product
        float operator^(const Vector3 & rhs) const;
        
        // cross product
        Vector3 operator*(const Vector3 & rhs) const;
        
        // normalize: make it unit vector, then, return self
        Vector3 & normalize();
        
        // scalar multiplication
        Vector3 operator*(float rhs) const;
        
        // scalar division
        Vector3 operator/(float rhs) const;

        // scalar division
        Vector3& operator/=(float rhs);
        
        // get norm
        float getNorm() const;
        
        // intensify
        Vector3 intensify(const Vector3 & intensityVector) const;

        static std::vector<Vector3> generateOrthonomalBasis(const Vector3& referenceVector);
        
        // generate a vector with different direction
        Vector3 generateDifferentlyDirectedVector() const;

        // element-wise division
        Vector3 operator/(const Vector3 & rhs) const;

        // element-wise compare
        bool operator!=(const float rhs) const;

        // element-wise product
        Vector3 elementwiseMultiply(const Vector3& rhs) const;

        bool isZeroVector() const;

        // element-wise power
        Vector3 power(float prime) const;
};

#endif