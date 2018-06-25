#ifndef __PIXEL_MISSION_GENERATOR__
#define __PIXEL_MISSION_GENERATOR__

#include "../geometry/headers/geometry.hpp"
#include <mutex>
#include <condition_variable>

class PixelMissionGenerator
{
    private:
        int x, y, width, height;
        bool done;
        std::mutex bagMutex;
        std::condition_variable change_on_filled;

        int totalNumOfPixels;
        int filled = 0.f;
        int filledNotNotified = 0.f;
    public:
        PixelMissionGenerator(int width, int height)
            : width(width), height(height), done(false), x(0), y(0)
            {
                totalNumOfPixels = width * height;
            }
        
        bool getFilledPerc(float & perc)
        {
            // lock bag mutex
            std::unique_lock<std::mutex> lk(bagMutex);

            while(!done && filledNotNotified == 0)
            {
                change_on_filled.wait(lk);
            }

            filledNotNotified = 0;

            perc = filled / (float)totalNumOfPixels;

            if(perc > 1.f) perc = 1.f;
            if(perc < 0.f) perc = 0.f;

            lk.unlock();

            return done;
        }

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
                filled++; filledNotNotified++;
                //if(filledNotNotified > totalNumOfPixels / 300.f)
                    change_on_filled.notify_one();
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
                    filled++; filledNotNotified++;
                    //if(filledNotNotified > totalNumOfPixels / 300.f)
                        change_on_filled.notify_one();
                    return true;
                }
                else
                {
                    done = true;
                    change_on_filled.notify_one();
                    return false;
                }
            }
        }
};

#endif
