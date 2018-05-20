#ifndef __PLY_PARSER_H__
#define __PLY_PARSER_H__

#include "pair.hpp"
#include "../geometry/headers/structs.hpp"
#include <vector>
#include <string>

Pair< std::vector<Vec3f>, std::vector<Vec3i> >
parsePly(const std::string& fileName);

#endif