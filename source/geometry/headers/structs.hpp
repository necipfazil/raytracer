#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "vector3.hpp"
#include "position3.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "vec3i.hpp"

struct Vec2i
{
    int x, y;
    Vec2i() : x(0), y(0) {  }
    Vec2i(int x, int y) : x(x), y(y) { }
};

struct Vec3f
{
    float x, y, z;
};

struct Vec4f
{
    float x, y, z, w;
};

struct TextureInfo
{
    bool hasTexture = false;

    Color textureColor;
    DecalMode decalMode;
};

struct HitInfo
{
    Vector3 normal;
    Position3 hitPosition;
    Material material;
    TextureInfo textureInfo;
    float t;
    float time = 0.f;
};

#endif