#include "../headers/vector3.hpp"
#include <cmath>
#include <vector>
#include <iostream>

// getX
float Vector3::getX() const
{
    return x;
}

// getY
float Vector3::getY() const
{
    return y;
}

// getZ
float Vector3::getZ() const
{
    return z;
}

// setX
void Vector3::setX(float x)
{
    this -> x = x;
}

// setY
void Vector3::setY(float y)
{
    this -> y = y;
}

// setZ
void Vector3::setZ(float z)
{
    this -> z = z;
}

// unary (-) operator
Vector3 Vector3::operator-() const
{
    return *this * (-1);
}

// Vector3 substraction
Vector3 Vector3::operator-(const Vector3 & rhs) const
{
    Vector3 result;
    
    result.setX( this->getX() - rhs.getX() );
    result.setY( this->getY() - rhs.getY() );
    result.setZ( this->getZ() - rhs.getZ() );
    
    return result;
}

// Vector3 addition
Vector3 Vector3::operator+(const Vector3 & rhs) const
{
    Vector3 result;
    
    result.setX( this->getX() + rhs.getX() );
    result.setY( this->getY() + rhs.getY() );
    result.setZ( this->getZ() + rhs.getZ() );
    
    return result;
}

// dot product
float Vector3::operator^(const Vector3 & rhs) const
{
    return this->x * rhs.x
         + this->y * rhs.y
         + this->z * rhs.z;
}

Vector3 Vector3::power(float prime) const
{
    return Vector3(
        pow(this->getX(), prime),
        pow(this->getY(), prime),
        pow(this->getZ(), prime)
    );
}

// cross product
Vector3 Vector3::operator*(const Vector3 & rhs) const
{
    Vector3 result;
    
    result.setX( this->getY() * rhs.getZ() - this->getZ() * rhs.getY() );
    result.setY( this->getZ() * rhs.getX() - this->getX() * rhs.getZ() );
    result.setZ( this->getX() * rhs.getY() - this->getY() * rhs.getX() );
    
    return result;
}

// normalize
Vector3 & Vector3::normalize()
{
    float norm = this->getNorm();
    
    // normalize by dividing the vector by its norm
    *this = *this / norm;
    
    return *this;
}

// scalar multiplication
Vector3 Vector3::operator*(float rhs) const
{
    return Vector3(
        this->x * rhs,
        this->y * rhs,
        this->z * rhs
    );
}

// scalar division
Vector3 Vector3::operator/(float rhs) const
{
    if(rhs == 0.0)
    {
        // division by zero!
        return Vector3(0.0, 0.0, 0.0);
    }

    return Vector3(
        this->x / rhs,
        this->y / rhs,
        this->z / rhs
    );
}

// get norm
float Vector3::getNorm() const
{
    float normSquare = this->x * this->x
                     + this->y * this->y
                     + this->z * this->z;
                     
    return sqrt( normSquare );
}

// element-wise division
Vector3 Vector3::operator/(const Vector3 & rhs) const
{
    return
        Vector3(
            this->x / rhs.x,
            this->y / rhs.y,
            this->z / rhs.z
        );
}

// element-wise compare
bool Vector3::operator!=(const float rhs) const
{
    return 
        this->x != rhs &&
        this->y != rhs &&
        this->z != rhs;
}

bool Vector3::isZeroVector() const
{
    return
        this->x == 0.f &&
        this->y == 0.f &&
        this->z == 0.f;
}

// operator overloadings with different order
Vector3 operator*(float lhs, const Vector3 & rhs)
{
    return rhs * lhs;
}

Vector3 operator/(float lhs, const Vector3 & rhs)
{
    return rhs / lhs;
}

// intensify
Vector3 Vector3::intensify(const Vector3 & intensityVector) const
{

    return Vector3( this->x * intensityVector.x,
                    this->y * intensityVector.y,
                    this->z * intensityVector.z
                    );
}

std::ostream &operator<<(std::ostream &output, const Vector3 & vector)
{
    output << "V( " << vector.getX() << ", " << vector.getY() << ", " << vector.getZ() << " )";
    return output;
}

std::vector<Vector3> Vector3::generateOrthonomalBasis(const Vector3& referenceVector)
{
    Vector3 r = referenceVector;
    r.normalize();

    Vector3 rPrime = r;

    // set the minimum component of rPrime to 1
    float x = rPrime.getX(), y = rPrime.getY(), z = rPrime.getZ();

    if(x <= y && x <= z)
        rPrime.x = 1.f;
    else if(y <= x && y <= z)
        rPrime.y = 1.f;
    else if(z <= x && z <= y)
        rPrime.z = 1.f;

    rPrime.normalize();

    // compute u, v, w(:r)
    Vector3 u = rPrime * r;
    u.normalize();
    Vector3 v = u * r;
    v.normalize();

    std::vector<Vector3> result = { r, u, v };

    return result;
}

Vector3 Vector3::generateDifferentlyDirectedVector() const
{
    float absX = x > 0.0f ? x : -x;
    float absY = y > 0.0f ? y : -y;
    float absZ = z > 0.0f ? z : -z;

    if(absX <= absY && absX <= absZ)
    {
        return Vector3(0, z, -y);
    }
    else if( absY <= absX && absY <= absZ)
    {
        return Vector3(-z, 0, x);
    }
    else if(absZ <= absX && absZ <= absZ)
    {
        return Vector3(-y, x, 0);
    }
}