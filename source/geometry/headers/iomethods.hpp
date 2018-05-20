#ifndef __IOMETHODS_H__
#define __IOMETHODS_H__

#include "position3.hpp"
#include "vector3.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream &output, const Position3 & position);
std::ostream &operator<<(std::ostream &output, const Vector3 & vector);

#endif