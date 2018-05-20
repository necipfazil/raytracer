#include "image.hpp"
#include "color.hpp"
#include "reinhard/reinhard_tonemapping.hpp"
#include <string>
#include <algorithm>
#include <cstring>
#include <limits>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

Image::Image(std::string fileName)
{
    imageMat = cv::imread(fileName, cv::IMREAD_COLOR);
    cv::cvtColor(imageMat, imageMat, CV_BGR2RGB);

    // each pixel is defined to be float
    imageMat.convertTo(imageMat, CV_32FC3);

    width = imageMat.cols;
    height = imageMat.rows;
}

Image::Image(int width, int height)
{
    imageMat = cv::Mat(height, width, CV_32FC3);
}

void Image::setColor(int positionX, int positionY, Color color)
{
    imageMat.at<cv::Vec3f>(positionY, positionX)[0] = color.getFR();
    imageMat.at<cv::Vec3f>(positionY, positionX)[1] = color.getFG();
    imageMat.at<cv::Vec3f>(positionY, positionX)[2] = color.getFB();
}

float Image::getGrayscaleColor(int positionX, int positionY) const
{
    if(positionX == width)
        positionX--;

    if(positionY == height)
        positionY--;

    cv::Vec3f color = imageMat.at<cv::Vec3f>(positionY, positionX);

    float colorSum = (float)color[0] + (float)color[1] + (float)color[2];

    return colorSum / 3;
}

Color Image::getColor(int positionX, int positionY) const
{
    if(positionX == width)
        positionX--;

    if(positionY == height)
        positionY--;

    cv::Vec3f color = imageMat.at<cv::Vec3f>(positionY, positionX);

    return Color(color[0], color[1], color[2]) / normalizer;
}

void Image::write(std::string fileName)
{
    // cv::imwrite takes BGR images. however, our image is RGB. convert!
    cv::Mat bgrImg;
    cv::cvtColor(imageMat, bgrImg, CV_RGB2BGR);

    // write
    cv::imwrite(fileName, bgrImg);
}

void Image::write(std::string fileName, ToneMappingParam toneMappingParam)
{
    cv::Mat bgrImg;
    cv::cvtColor(imageMat, bgrImg, CV_RGB2BGR);

    float Lwhite = std::numeric_limits<float>::max();

    // find the Lwhite value, if burnout percentage is specified
    if(toneMappingParam.burnoutPercentage > 0.f && toneMappingParam.burnoutPercentage < 100.f)
    {
        // change Lwhite accordingly
        cv::Mat lumImg = getLuminanceImage(bgrImg);

        // make it a row matrix
        lumImg = lumImg.reshape(1, 1);

        // sort the luminance image
        cv::sort(lumImg, lumImg, CV_SORT_DESCENDING);

        // find the index of the desired Lwhite
        int index = (toneMappingParam.burnoutPercentage / 100) * lumImg.cols;

        // get Lwhite
        Lwhite = lumImg.at<float>(index);
    }

    cv::Mat tonemappedImage = 
        globalTMO(
            bgrImg, // hdr image
            toneMappingParam.keyValue,
            Lwhite,
            toneMappingParam.saturation
        );

    // clamp
    cv::threshold(tonemappedImage, tonemappedImage, 0, 0, cv::THRESH_TOZERO);
    cv::threshold(tonemappedImage, tonemappedImage, 1, 1, cv::THRESH_TRUNC);

    // gamma correction
    cv::pow(tonemappedImage, 1 / toneMappingParam.gamma, tonemappedImage);

    // bring to 0-255 range
    tonemappedImage *= 255;

    // write
    cv::imwrite(fileName, tonemappedImage);
}