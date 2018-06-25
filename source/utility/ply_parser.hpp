#ifndef __PLY_PARSER_H__
#define __PLY_PARSER_H__

#include "pair.hpp"
#include "../geometry/headers/structs.hpp"
#include <vector>
#include <string>

// vertexData, meshVertexIndices, texCoordData(if applicable)
Triple< std::vector<Vec3f>, std::vector<Vec3i>, std::vector<Vec2f> >
parsePly(const std::string& fileName);

#endif