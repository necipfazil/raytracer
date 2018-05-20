#include "ply_parser.hpp"
#include "pair.hpp"
#include "tinyply-2.0/source/tinyply.h"
#include "../geometry/headers/structs.hpp"
#include "../geometry/headers/position3.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>

using namespace tinyply;

Pair< std::vector<Vec3f>, std::vector<Vec3i> >
parsePly(const std::string& filename)
{
    std::ifstream ss(filename, std::ios::binary);

    if (ss.fail()) 
    {
        throw std::runtime_error("failed to open ply file" + filename);
    }

    PlyFile file;

    // parse header
    file.parse_header(ss);

    // will read vertex and face information
    std::shared_ptr<PlyData> tinyply_vertices, tinyply_faces;

    // read data from file
        // vertex data
    try 
    {
        tinyply_vertices = file.request_properties_from_element("vertex", { "x", "y", "z" });
    }
    catch (const std::exception & e) 
    {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }
        // face data
    try
    {
        tinyply_faces = file.request_properties_from_element("face", { "vertex_indices" });
    }
    catch (const std::exception & e)
    {
        try
        {
            tinyply_faces = file.request_properties_from_element("face", { "vertex_index" });
        }
        catch (const std::exception & e)
        {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }
        
    }
file.read(ss);

int numberPerElement = (tinyply_faces->buffer.size_bytes() / sizeof(int)) / tinyply_faces->count;

if(numberPerElement > 4 || numberPerElement < 3)
{
    throw std::runtime_error("Unsupported ply file");
}
    // get vertices
    const size_t numVerticesBytes = tinyply_vertices->buffer.size_bytes();
    std::vector<Vec3f> vertices(tinyply_vertices->count);
    std::memcpy(vertices.data(), tinyply_vertices->buffer.get(), numVerticesBytes);

    // create indices vector
    const size_t numFacesBytes = tinyply_faces->buffer.size_bytes();
    std::vector<Vec3i> indices;
    
    int* indexBuffer = (int*)tinyply_faces->buffer.get();

    int bufferIndex = 0;
    
    // fill indices vector
    for(int i = 0; i < tinyply_faces->count; i++)
    {
        if(numberPerElement == 3)
        {
            indices.push_back(
                Vec3i(indexBuffer[bufferIndex], indexBuffer[bufferIndex + 1], indexBuffer[bufferIndex + 2])
            );
        }
        else if(numberPerElement == 4)
        {
            indices.push_back(
                Vec3i(indexBuffer[bufferIndex], indexBuffer[bufferIndex + 1], indexBuffer[bufferIndex + 2])
            );

            indices.push_back(
                Vec3i(indexBuffer[bufferIndex + 2], indexBuffer[bufferIndex + 3], indexBuffer[bufferIndex])
            );
        }

        bufferIndex += numberPerElement;
    }

    return 
    Pair<
        std::vector<Vec3f>, std::vector<Vec3i>
        > (vertices, indices);
}