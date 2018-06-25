#ifndef __ENUMS_H__
#define __ENUMS_H__

enum Axis
{
    X,
    Y,
    Z
};

enum ShadingMode
{
    FLAT,
    SMOOTH
};

enum Integrator
{
    DEFAULT,
    UNIFORM_PATHTRACING,
    IMPORTANCE_PATHTRACING
};

enum RandomFactor
{
    UNIFORM,
    IMPORTANCE
};

#endif