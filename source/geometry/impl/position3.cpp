#include "../headers/position3.hpp"
#include<iostream>

// Position3
// getX
float Position3::getX() const
{
    return x;
}

// getY
float Position3::getY() const
{
    return y;
}

// getZ
float Position3::getZ() const
{
    return z;
}

// setX
void Position3::setX(float x)
{
    this -> x = x;
}

// setY
void Position3::setY(float y)
{
    this -> y = y;
}

// setZ
void Position3::setZ(float z)
{
    this -> z = z;
}

// substraction of two positions yields a vector
Vector3 Position3::operator-(const Position3 & rhs) const
{
    return Vector3(
        this->x - rhs.x,
        this->y - rhs.y,
        this->z - rhs.z
    );
}

// addition of Position3 with a Vector3 yields displacement and returns a new Position3
Position3 Position3::operator+(const Vector3 & rhs) const
{
    return Position3(
        this->x + rhs.getX(),
        this->y + rhs.getY(),
        this->z + rhs.getZ()
    );
}

// a vector from "this" to "rhs"
Vector3 Position3::to(const Position3 & rhs) const
{
    return rhs.operator-(*this);
}

float Position3::distanceSquare(const Position3 & rhs) const
{
    return ( x - rhs.x ) * ( x - rhs.x ) + ( y - rhs.y ) * ( y - rhs.y ) + ( z - rhs.z ) * ( z - rhs.z ) ;
}


Position3 Position3::generateMinPosition(const Position3 & p1, const Position3 & p2)
{
    // initialize result with p1
    Position3 result = p1;

    // update if necessary
        // X
    result.x = result.x < p2.x ? result.x : p2.x;
        // Y
    result.y = result.y < p2.y ? result.y : p2.y;
        // Z
    result.z = result.z < p2.z ? result.z : p2.z;

    return result;
}


Position3 Position3::generateMaxPosition(const Position3 & p1, const Position3 & p2)
{
    // initialize result with p1
    Position3 result = p1;

    // update if necessary
        // X
    result.x = result.x > p2.x ? result.x : p2.x;
        // Y
    result.y = result.y > p2.y ? result.y : p2.y;
        // Z
    result.z = result.z > p2.z ? result.z : p2.z;

    return result;
}

bool Position3::compareLTX(const Position3 & lhs, const Position3 & rhs)
{
    return lhs.getX() < rhs.getX();
}

bool Position3::compareLTY(const Position3 & lhs, const Position3 & rhs)
{
    return lhs.getY() < rhs.getY();
}

bool Position3::compareLTZ(const Position3 & lhs, const Position3 & rhs)
{
    return lhs.getZ() < rhs.getZ();
}

std::ostream &operator<<(std::ostream &output, const Position3 & position)
{
    output << "P( " << position.getX() << ", " << position.getY() << ", " << position.getZ() << " )";
    return output;
}