/**
 * METU CENG795 RayTracer
 * Author: Necip Fazil Yildiran
 * Adapted from author's METU CENG587 Project
 * */

#include <vector>
#include <limits>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// returns global tone-mapped color image
cv::Mat globalTMO(
	const cv::Mat & hdrImg,
	float keyA = 0.18f,
	float Lwhite = std::numeric_limits<float>::max(),
	float saturation = 1.f
);


// scaling of luminance image by using key value a
// by default, the key value is set to 0.18 as suggested in the paper of Reinhard et al.
cv::Mat scaleLuminanceByKey(const cv::Mat & lumImg, float keyA);

// given the Mat of HDR image which has 3 channels where each channel is represented by 4 byte float value,
// returns the luminance Mat.
cv::Mat getLuminanceImage(const cv::Mat & rgbHdrImage);

// color assignment
cv::Mat getTonemappedColorImg(const cv::Mat & rgbHdrImg, const cv::Mat & luminanceImg, const cv::Mat & tonemappedLum, float s);

// color assignment, overloaded, no need to give luminanceImg as parameter
// however, if it was already computed, other version will increase performance since it will not re-compute
cv::Mat getTonemappedColorImg(const cv::Mat & rgbHdrImg, const cv::Mat & tonemappedLum, float s);

// given luminance image, returns average luminance
// the approach is to compute average in log domain, then, return back to scalar
float getLogAvgLuminance(const cv::Mat & lumImg);

// check input image if operable
bool checkInput(const cv::Mat & inputHdrImage);
