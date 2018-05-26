/**
 * METU CENG795 RayTracer
 * Author: Necip Fazil Yildiran
 * Adapted from author's METU CENG587 Project
 * */

#include "reinhard_tonemapping.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>
#include <cmath>

#include <vector>

#define _USE_MATH_DEFINES

using namespace std;
using namespace cv;

Mat getTonemappedColorImg(const Mat & rgbHdrImg, const Mat & luminanceImg, const Mat & tonemappedLum, float s = 1.0f);

// color assignment
// s: saturation
Mat getTonemappedColorImg(const Mat & rgbHdrImg, const Mat & luminanceImg, const Mat & tonemappedLum, float s)
{
	// for each color, compute C_out = L_out * ( C_in / L_in ) ^ s
	std::vector<cv::Mat> bgrChannels(3);
	
	cv::split(rgbHdrImg, bgrChannels);
	
	// for each channel, compute the division: C_in / L_in
	for(int i = 0; i < 3; i++)
		cv::divide(
			bgrChannels[i],
			luminanceImg,
			bgrChannels[i]
		);
	
	// for each channel, compute the raise to s
	if(s != 1.0f)
		for(int i = 0; i < 3; i++)
			cv::pow(
				bgrChannels[i],
				s,
				bgrChannels[i]
			);

	// for each channel, element-wise multiplication with L_out
	for(int i = 0; i < 3; i++)
		cv::multiply(
			bgrChannels[i],
			tonemappedLum,
			bgrChannels[i]
		);
	
	// merge channels and return
	Mat result;
	merge(bgrChannels, result);
	return result;
}

// color assignment, overloaded
Mat getTonemappedColorImg(const Mat & rgbHdrImg, const Mat & tonemappedLum, float s = 1.0f)
{
	Mat luminanceImg = getLuminanceImage(rgbHdrImg);
	
	return getTonemappedColorImg(rgbHdrImg, luminanceImg, tonemappedLum, s);
}

Mat globalTMO(const Mat & hdrImg, float keyA, float Lwhite, float saturation)
{
	Mat LsquareOverLWhite, numerator, denominator, toneMappedLuminanceImg;

	// get luminance image
	Mat luminanceImg = getLuminanceImage(hdrImg);
	
	// scale luminance image by key value keyA
	Mat scaledLuminanceImg = scaleLuminanceByKey(luminanceImg, keyA);

    // scale Lwhite also
    {
        // get log-average luminance
        float logAvgLum = getLogAvgLuminance(luminanceImg);
        
        // get the scale factor
        float scaleFactor = keyA / logAvgLum;
        
        // scale and return
        Lwhite *= scaleFactor;
    }
	
	// notice that multiply, add and divide are element-wise operators
	
	// compute LsquareOverLWhite
	cv::multiply(
		scaledLuminanceImg,			// src1
		scaledLuminanceImg,			// src2
		LsquareOverLWhite,			// dst
		1.0 / (Lwhite * Lwhite)		// scale
	);
	
	// compute numerator
	cv::add(
		LsquareOverLWhite,			// src1
		scaledLuminanceImg,			// src2
		numerator					// dst
	);
	
	// compute denominator
	denominator = scaledLuminanceImg + 1;
	
	// compute tone-mapped luminance image
	cv::divide(
		numerator,				// src1
		denominator,			// src2
		toneMappedLuminanceImg	// dst
	);
	
	// compute global tone-mapped color image
	Mat toneMappedColoredImg = getTonemappedColorImg(hdrImg, luminanceImg, toneMappedLuminanceImg, saturation);
	
	return toneMappedColoredImg;
}

Mat scaleLuminanceByKey(const Mat & lumImg, float keyA = 0.18f)
{
	if(lumImg.empty() || lumImg.channels() != 1 || lumImg.type() != CV_32FC1)
	{
		cerr << "Error: Incorrect input for scaleLuminanceByKey()" << endl;
		exit(1);
	}

	// get log-average luminance
	float logAvgLum = getLogAvgLuminance(lumImg);
	
	// get the scale factor
	float scaleFactor = keyA / logAvgLum;
	
	// scale and return
	return lumImg * scaleFactor;
	
}

float getLogAvgLuminance(const Mat & _lumImg)
{
	if(_lumImg.empty() || _lumImg.channels() != 1 || _lumImg.type() != CV_32FC1)
	{
		cerr << "Error: Incorrect input for getLogLuminanceAvg()" << endl;
		exit(1);
	}

	// Create a new Mat, clone lumImg, and add an epsilon value to avoid log(0)
	Mat lumImg = _lumImg.clone();
	lumImg += .01f;
	
	int numOfRows = lumImg.rows;
	int numOfCols = lumImg.cols;
	
	// get log(luminance) image. notice that the base is e
    Mat logLumImg;
	cv::log(lumImg, logLumImg);
	
	
	Mat mask = logLumImg != logLumImg;
	
	// NaN check
	for(int x = 0; x < numOfRows; x++)
	{	
		for(int y = 0; y < numOfCols; y++)
		{
			unsigned char maskVal = mask.at<unsigned char>(x, y);
			if(maskVal != 0x00)
			{
                cout << "Warning: NaN in getLogLuminanceAvg()" << endl;
			}
		}
	}
	
	// sum log(luminances)
	float sum = cv::sum(logLumImg)[0];
	
	// divide the sum by number of pixels
	sum /= numOfRows * numOfCols;
	
	// exponantiate to get the result
	float avg = std::exp(sum);
	
	return avg;
}

void splitImageIntoChannels(const cv::Mat & image, cv::Mat & B, cv::Mat & G, cv::Mat & R)
{
    cv::Mat channelArray[3];

    cv::split(image, channelArray);

    // The order shall be BGR
    B = channelArray[0];
    G = channelArray[1];
    R = channelArray[2];
}

Mat getLuminanceImage(const Mat & colorImage)
{
    if(colorImage.empty() || colorImage.channels() != 3 || colorImage.type() != CV_32FC3)
	{
		cerr << "Error: Incorrect input for getLuminanceImage()" << endl;
		exit(1);
	}

    // same resolution but one channnel, which will carry the luminance value
    Mat luminanceImage;
    Mat B, G, R;
    splitImageIntoChannels(colorImage, B, G, R);

    luminanceImage = 0.2126 * R + 0.7152 * G + 0.0722 * B; //0.27f * R + 0.67f * G + 0.06f * B;

    return luminanceImage;
}

bool checkInput(const Mat & inputHdrImage)
{
    if(inputHdrImage.empty())
    {
        cerr << "Error: Couldn't read the input file" << endl;
        return false;
    }

    // type check: CV_32F
    else if(inputHdrImage.type() != CV_32FC3)
    {
        cerr << "Error: Incorrect image type. Was waiting for CV_32FC3" << endl;
        return false;
    }

    else if(!inputHdrImage.isContinuous())
    {
        cout << "Error: Not continuous!" << endl;
        return false;
    }

    return true;
}
