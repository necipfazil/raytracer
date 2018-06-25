#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "position3.hpp"
#include "vector3.hpp"
#include "ray.hpp"
#include "../../utility/pair.hpp"
#include "../../image/image.hpp"

#include <string>
#include <vector>

class Camera
{
    public:
        enum Handedness
        {
            LEFT,
            RIGHT
        };

        enum CameraType
        {
            SIMPLE,
            COMPLEX
        };

        enum GammaCorrection
        {
            NONE,
            SRGB
        };
    private:
        Position3 position;
        Vector3 gaze;
        Vector3 up;

        // coord sys
        Vector3 u, v, w;

        // the vector from camera position to the images' top left corner
        Vector3 toTopLeft;

        // the step required to be taken from center of one grid to the neighbour
        Vector3 centerStepInGridX, centerStepInGridY;
        float uStep, vStep;
        Vec4f nearPlane;
        float nearDistance;
        int imageWidth, imageHeight;
        int numSamples = 1, gridDim = 1;
        std::string imageName;

        bool doToneMapping = false;
        ToneMappingParam toneMappingParam;

        // additional fields for simple camera
        Position3 gazePoint;
        float fovY;

        Position3 topLeftCorner;

        float focusDistance;
        float apertureSize;

        GammaCorrection gammaCorrection = NONE;

        Handedness handedness = Handedness::RIGHT;

        float clampValue;
        bool clampingEnabled = false;

        // useful for encapsulating aperture computations
        Position3 getPosition() const;
        Vector3 getToTopLeft() const;

        static float gaussianWeight(float xDistance, float yDistance);

    public:
        // getters
        int getImageWidth() const;
        int getImageHeight() const;
        int getNumSamples() const;
        int getGridDim() const;

        // coord sys, should be filled by calling computeCoordSys
        Vector3 getU() const;
        Vector3 getV() const;
        Vector3 getW() const;

        // constants computed by uvw and image dimensions
        float getUStep() const;
        float getVStep() const;

        std::vector<Ray> getRays(int imageCoordX, int imageCoordY) const;
        std::string getImageName() const;

        std::vector<Pair<Vector3, float> > getRayDeviationsForSamples() const;

        // setters
        void setPosition(const Position3 & Position);
        void setGaze(const Vector3 & gaze);
        void setUp(const Vector3 & up);
        void setNearPlane(const Vec4f & nearPlane);
        void setNearDistance(float nearDistance);
        void setImageWidth(int imageWidth);
        void setImageHeight(int imageHeight);
        void setImageName(const std::string & imageName);
        void setNumSamples(int numSamples);
        void setFocusDistance(float focusDistance);
        void setApertureSize(float apertureSize);
        void setGazePoint(const Position3 & gazePoint);
        void setFovY(float fovY);
        void setToneMapping(const ToneMappingParam& toneMappingParam);
        void setHandedness(Camera::Handedness handedness);
        void setGammaCorrection(GammaCorrection gammaCorrection);
        void setClamp(float clampValue) { this->clampValue = clampValue; this->clampingEnabled = true; }
        float getGammaCorrection() const;
        bool doTonemap() const { return this->doToneMapping; }
        bool isClampingEnabled() const { return this->clampingEnabled; }
        float getClampValue() const { return this->clampValue; }
        ToneMappingParam getToneMappingParam() const { return this->toneMappingParam; }

        // A method for correcting gaze and up and filling some camera information
        void computeCoordSys(Camera::CameraType cameraType);
};

#endif