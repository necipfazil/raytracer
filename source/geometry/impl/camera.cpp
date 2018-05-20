#include "../../config.h"
#include "../headers/camera.hpp"
#include "../../utility/random_number_generator.hpp"
#include "../../utility/pair.hpp"
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

int Camera::getNumSamples() const
{
    return this->numSamples;
}

int Camera::getGridDim() const
{
    return this->gridDim;
}

void Camera::setNumSamples(int numSamples)
{
    this->numSamples = numSamples;

    // TODO: ceil function better be used
    this->gridDim = (int) round(sqrt((double)this->numSamples));
}

void Camera::setToneMapping(const ToneMappingParam& toneMappingParam)
{
    this->doToneMapping = true;
    
    this->toneMappingParam = toneMappingParam;
}

int Camera::getImageWidth() const
{
    return this->imageWidth;
}

int Camera::getImageHeight() const
{
    return this->imageHeight;
}

std::string Camera::getImageName() const
{
    return this->imageName;
}

Vector3 Camera::getU() const
{
    return this->u;
}

Vector3 Camera::getV() const
{
    return this->v;
}

Vector3 Camera::getW() const
{
    return this->w;
}

void Camera::setPosition(const Position3 & position)
{
    this->position = position;
}

void Camera::setGaze(const Vector3 & gaze)
{
    this->gaze = gaze;
}

void Camera::setUp(const Vector3 & up)
{
    this->up = up;
}

void Camera::setNearPlane(const Vec4f & nearPlane)
{
    this->nearPlane = nearPlane;
}

void Camera::setNearDistance(float nearDistance)
{
    this->nearDistance = nearDistance;
}

void Camera::setImageWidth(int imageWidth)
{
    this->imageWidth = imageWidth;
}

void Camera::setImageHeight(int imageHeight)
{
    this->imageHeight = imageHeight;
}

void Camera::setImageName(const std::string & imageName)
{
    this->imageName = imageName;
}

void Camera::setFocusDistance(float focusDistance)
{
    this->focusDistance = focusDistance;
}

void Camera::setApertureSize(float apertureSize)
{
    this->apertureSize = apertureSize;
}

float Camera::getUStep() const
{
    return this->uStep;
}

float Camera::getVStep() const
{
    return this->vStep;
}

void Camera::setGazePoint(const Position3 & gazePoint)
{
    this->gazePoint = gazePoint;
}

void Camera::setFovY(float fovY)
{
    this->fovY = fovY;
}

void Camera::computeCoordSys(Camera::CameraType cameraType)
{
    // if the camera type is simple, convert it to complex by filling the
    // required fields
    if(cameraType == CameraType::SIMPLE)
    {
            // compute gaze
        this->setGaze(this->position.to(this->gazePoint).normalize());

            // compute nearplane
        float verticalDist = tan((this->fovY / 2.f) * (M_PI / 180.f)) * this->nearDistance;
        float horizontalDist = verticalDist * ((float)this->imageWidth / (float)this->imageHeight);

        this->nearPlane.x = -horizontalDist; // left
        this->nearPlane.y = horizontalDist;  // right
        this->nearPlane.z = -verticalDist;   // bottom
        this->nearPlane.w = verticalDist;    // top
    }

    // compute V
    Vector3 vecV = this->up;
    vecV.normalize();
    
    // compute W
    Vector3 vecW = -gaze;
    gaze.normalize();
    vecW.normalize();
    
    // compute U
    Vector3 vecU = vecV * vecW; // cross product
    vecU.normalize();
   
    // correct V
    vecV = vecW * vecU;

    // set coord sys
    this->u = vecU;
    this->v = vecV;
    this->w = vecW;

    // correct the dimensions by focus distance if required
    if(this->focusDistance != 0.f)
    {
        float nearFocusRatio = this->focusDistance / this->nearDistance;

        float width = nearPlane.y - nearPlane.x;
        float height = nearPlane.w - nearPlane.z;

        float scaledWidth = width * nearFocusRatio;
        float scaledHeight = height * nearFocusRatio;

        float widthDiff = (width - scaledWidth) / 2;
        float heightDiff = (height - scaledHeight) / 2;

        nearPlane.x += widthDiff;
        nearPlane.y -= widthDiff;
        nearPlane.z += heightDiff;
        nearPlane.w -= heightDiff;

        nearDistance = focusDistance;
    }
    
    // get some references
    const float & left   = this->nearPlane.x;
    const float & right  = this->nearPlane.y;
    const float & bottom = this->nearPlane.z;
    const float & top    = this->nearPlane.w;
    const float & d      = this->nearDistance;


    /*const float & uConstant = this->uStep;
    const float & vConstant = this->vStep;*/

    // compute uStep and vStep and set them
    this->uStep = (right - left) / imageWidth;
    this->vStep = (top - bottom) / imageHeight;

    // vector from camera position to top left corner
    this->toTopLeft =
        gaze * d            // dir by w
        + this->u * left // dir by u
        + this->v * top; // dir by v

    this->topLeftCorner = this->position + toTopLeft;

    this->centerStepInGridX = (this->getU() * this->getUStep()) / (float)gridDim;
    this->centerStepInGridY = (this->getV() * -this->getVStep()) / (float)gridDim;
}

std::vector<Pair<Vector3, float> > Camera::getRayDeviationsForSamples() const
{
    std::vector<Pair<Vector3, float> > result;

    int gridDim = this->getGridDim();

    float centerDistanceX = -0.5f, centerDistanceY = -0.5f;

    const float centerDistanceStepX = 1.f / gridDim;
    const float centerDistanceStepY = 1.f / gridDim;

    for(int y = 0; y < gridDim; y++)
    {
        for(int x = 0; x < gridDim; x++)
        {
            Vector3 topleftOfCurrentGrid = (this->centerStepInGridX * x) + (this->centerStepInGridY * y);

            float rnd_x = getRandomBtw01();
            float rnd_y = getRandomBtw01();

            Vector3 rndSampleInsideGrid =
                topleftOfCurrentGrid
                + (this->centerStepInGridX * rnd_x)
                + (this->centerStepInGridY * rnd_y);

            float weight = gaussianWeight(centerDistanceX * (1 + rnd_x), centerDistanceY * (1 + rnd_y));

            result.push_back(Pair<Vector3, float>(rndSampleInsideGrid, weight));

            // advance distance to center in X
            centerDistanceX += centerDistanceStepX;
        }

        // advance distance to center in Y
        centerDistanceY += centerDistanceStepY;
        
        // reset centerDistanceX
        centerDistanceX = -0.5f;
    }

    return result;
}

Position3 Camera::getPosition() const
{
    if(apertureSize == 0.f)
        return this->position;
        

    // deviation in unit square
    Vector3 deviation = (u * getRandom0_5()) + (v * getRandom0_5());

    // deviation in aperture size
    deviation = deviation * apertureSize;
    
    // deviated camera position inside aperture square
    return this->position + deviation;
}

Vector3 Camera::getToTopLeft() const
{
    return getPosition().to(topLeftCorner);
}

// xDistance and yDistance are the distances from the pixel center
// and should be in the interval [-0.5, 0.5]
float Camera::gaussianWeight(float xDistance, float yDistance)
{
    // some precomputed constants to make the computation faster
    thread_local static float weightingStandartDev = 1.16f;
    thread_local static float k_1 = 1 / (M_PI * 2.f * pow(weightingStandartDev, 2));
    thread_local static float k_2 = exp( - 1.f / (2.f * pow(weightingStandartDev, 2)));
    thread_local static float k = k_1 * k_2;

    return k * exp( -(pow(xDistance, 2) + pow(yDistance, 2) ) );
}

std::vector<Ray> Camera::getRays(int imageCoordX, int imageCoordY) const
{
    std::vector<Ray> rays;
    // deviate rays and add them to list

        // if number of samples is 1, take the ray passing through the center of the pixel
        // since deviating a ray having only 1 simple produces images with artifacts
    if(this->numSamples == 1)
    {
        Vector3 rayDirection =
            this->toTopLeft // topleft corner
            + this->u * (this->uStep * (imageCoordX + 0.5f)) // get to center in u direction
            - this->v * (this->vStep * (imageCoordY + 0.5f)); // get to center in v direction

        // create ray
        Ray ray = Ray(this->position, rayDirection);

        // set time
            // better to have a single time value for sample size 1
        ray.setTimeCreated(0.5f);

        // push ray to rays vector
            // although the vector will have a single ray, this will keep the convention
        rays.push_back(ray);
    }
        // otherwise (numSamples != 1), create rays by random deviation
    else
    {
        // generate deviations
        std::vector<Pair<Vector3, float> > weightedRayDeviations = this->getRayDeviationsForSamples();
        
        for(int i = 0; i < (int)weightedRayDeviations.size(); i++)
        {
            // get a new position
            Position3 cameraPosition = getPosition();

            // top left corner of 'the pixel'
            Vector3 direction =
                //this->toTopLeft
                cameraPosition.to(topLeftCorner)
                + this->u * (this->uStep * imageCoordX)
                - this->v * (this->vStep * imageCoordY);

            // create devated direction
            Vector3 deviatedDirection = direction + weightedRayDeviations[i].p1;

            // create ray
            Ray ray = Ray(cameraPosition, deviatedDirection);

            // set weight
            ray.setWeight(weightedRayDeviations[i].p2);

            // set time
            float time = getRandomBtw01();
            ray.setTimeCreated(time);
            // push ray to rays vector
            rays.push_back(ray);
        }
    }

    return rays;
}