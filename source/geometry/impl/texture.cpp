#include "../headers/texture.hpp"
#include "../../image/image.hpp"
#include "../../image/color.hpp"


#include<iostream>
using namespace std;

int closestPositiveInt(float val)
{
    int result = val;

    if(val - result > 0.5f)
        return result + 1;
    
    return result;
}

Vec2f ImageTexture::getGradient(float u, float v) const
{
    float i = u * image.getWidth();
    float j = v * image.getHeight();

    float iNext = i + 1;
    float jNext = j + 1;

    // update next coordinates according to limits
    iNext = iNext <= image.getWidth() ? iNext : i;
    jNext = jNext <= image.getWidth() ? jNext : j;

    return Vec2f(
        image.getGrayscaleColor(iNext, j) - image.getGrayscaleColor(i, j),
        image.getGrayscaleColor(i, jNext) - image.getGrayscaleColor(i, j)
    );
}

Color ImageTexture::getInterpolatedColor(float u, float v) const
{
    float i = u * image.getWidth();
    float j = v * image.getHeight();

    // set texture color depending on interpolation mode
    if(interpMode == InterpolationMode::NEAREST)
    {
        // TODO: Limits? Repeat clamp etc.
        return image.getColor(closestPositiveInt(i), closestPositiveInt(j));
    }
    else if(interpMode == InterpolationMode::BILINEAR)
    {
        // floor of coordinates
        int iFloor = i;
        int jFloor = j;

        // distance to floor coord
        float dx = i - iFloor;
        float dy = j - jFloor;

        // there are 4 cells to get interpolated
        Color color =
            image.getColor(iFloor    , jFloor    ).intensify((1 - dx) * (1 - dy)) +
            image.getColor(iFloor + 1, jFloor + 1).intensify(dx       * dy      ) +
            image.getColor(iFloor + 1, jFloor    ).intensify(dx       * (1 - dy)) +
            image.getColor(iFloor    , jFloor + 1).intensify((1 - dx) * dy      );
        
        
        return color;
    }
    else
    {
        return Color::Black();
    }
}