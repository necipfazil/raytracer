#ifndef __PIXEL_MISSION_GENERATOR__
#define __PIXEL_MISSION_GENERATOR__

#include "../geometry/headers/geometry.hpp"
#include <mutex>

class PixelMissionGenerator
{
    private:
        int x, y, width, height;
        bool done;
        std::mutex bagMutex;
    public:
        PixelMissionGenerator(int width, int height)
            : width(width), height(height), done(false), x(0), y(0) { }
        
        bool getPixelToFill(Vec2i & pixelToFill)
        {
            std::lock_guard<std::mutex> guard(bagMutex);

            if(done)
                return false;
            
            if(x < width)
            {
                pixelToFill.x = x;
                pixelToFill.y = y;
                x++;
                return true;
            }
            else
            {
                x = 0;
                if(++y < height)
                {
                    pixelToFill.x = x;
                    pixelToFill.y = y;
                    x++;
                    return true;
                }
                else
                {
                    done = true;
                    return false;
                }
            }
        }
};

#endif
