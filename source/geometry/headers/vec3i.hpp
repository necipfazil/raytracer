#ifndef __VEC3I_H__
#define __VEC3I_H__

struct Vec3i
{
    int x, y, z;

    Vec3i() {}

    Vec3i(int x, int y, int z)
        : x(x), y(y), z(z) { }
};

#endif