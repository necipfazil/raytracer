#ifndef __BINARY_FILE_PARSER_H__
#define __BINARY_FILE_PARSER_H__

#include "../geometry/headers/structs.hpp"
#include "../geometry/headers/vertex.hpp"
#include <vector>
#include <string>

std::vector<Vertex> parseVertexData(std::string binaryFilePath);

std::vector<Vec2f> parseTexCoordData(std::string binaryFilePath);

std::vector<Vec3i> parseMeshFaces(std::string binaryFilePath);

#endif