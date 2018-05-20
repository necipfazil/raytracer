#ifndef __VEC2F_H__
#define __VEC2F_H__

struct Vec2f
{
    float u, v;

    Vec2f() : u(0), v(0) { }

    Vec2f(float u, float v) : u(u), v(v) { }

};

#endif