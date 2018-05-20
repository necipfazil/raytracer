#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "color.hpp"
#include<string>

#include<opencv2/core.hpp>

struct ToneMappingParam
{
    float keyValue = .18f;
    float burnoutPercentage = 0.f;
    float saturation = 1.f;
    float gamma = 2.2f;
};

class Image
{
    private:
        cv::Mat imageMat;
        
        int width, height;

        float normalizer = 1.f;
    
    public:
        Image(std::string fileName);

        Image(int width, int height);

        float getGrayscaleColor(int positionX, int positionY) const;

        void setColor(int positionX, int positionY, Color color);
        
        Color getColor(int positionX, int positionY) const;

        void write(std::string fileName);

        // write tonemapped image
        void write(std::string fileName, ToneMappingParam toneMappingParam);

        int getWidth() const { return width; }
        int getHeight() const { return height; }

        void setNormalizer(float normalizer) { this->normalizer = normalizer; }
};

#endif
