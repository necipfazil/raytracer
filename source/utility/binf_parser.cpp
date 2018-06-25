#include "binf_parser.hpp"
#include "../geometry/headers/structs.hpp"
#include "../geometry/headers/vertex.hpp"
#include "../geometry/headers/position3.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

std::vector<Vec3i> parseMeshFaces(std::string binaryFilePath)
{
    std::vector<Vec3i> faces;

    // create stream from file
    std::ifstream stream (binaryFilePath.data(), std::ifstream::binary);

    // read number of vertices
    int numberOfData = 0;
    stream.read(reinterpret_cast<char*>(&numberOfData), sizeof(int));

    int x, y, z;
    for(int i = 0; i < numberOfData; i++)
    {
        // read
		stream.read(reinterpret_cast<char*>(&x), sizeof(int)); // x
		stream.read(reinterpret_cast<char*>(&y), sizeof(int)); // y
        stream.read(reinterpret_cast<char*>(&z), sizeof(int)); // z

        // push
        faces.push_back(Vec3i(x, y, z));
    }

    stream.close();

    return faces;
}

std::vector<Vec2f> parseTexCoordData(std::string binaryFilePath)
{
    std::vector<Vec2f> texCoordData;

    // create stream from file
    std::ifstream stream (binaryFilePath.data(), std::ifstream::binary);

    // read number of vertices
    int numberOfData = 0;
    stream.read(reinterpret_cast<char*>(&numberOfData), sizeof(int));

    float u, v;
    for(int i = 0; i < numberOfData; i++)
    {
        // read
		stream.read(reinterpret_cast<char*>(&u), sizeof(float)); // u
		stream.read(reinterpret_cast<char*>(&v), sizeof(float)); // v

        // push
        texCoordData.push_back(Vec2f(u, v));
    }

    stream.close();

    return texCoordData;
}

std::vector<Vertex> parseVertexData(std::string binaryFilePath)
{
    std::vector<Vertex> vertexData;

    // create stream from file
    std::ifstream stream (binaryFilePath.data(), std::ifstream::binary);

    // read number of vertices
    int numberOfVertices = 0;
    stream.read(reinterpret_cast<char*>(&numberOfVertices), sizeof(int));

    float x, y, z;
    for(int i = 0; i < numberOfVertices; i++)
    {
        // read
		stream.read(reinterpret_cast<char*>(&x), sizeof(float)); // x
		stream.read(reinterpret_cast<char*>(&y), sizeof(float)); // y
		stream.read(reinterpret_cast<char*>(&z), sizeof(float)); // z

        // push
        vertexData.push_back(Position3(x, y, z));
    }

    stream.close();

    return vertexData;
}
