#ifndef __TRANSFORMATION_H__
#define __TRANSFORMATION_H__

#include "matrix4.hpp"
#include "vector3.hpp"

#include <cmath>

class Transformation
{
    protected:
        Matrix4 transformationMatrix;
        Matrix4 inverseTransformationMatrix;
        Matrix4 normalTransformationMatrix;
        
    public:
    
        // default constructor
        Transformation() { }
        
        Transformation(const Matrix4 & transformationMatrix,
                       const Matrix4 & inverseTransformationMatrix)
            : transformationMatrix(transformationMatrix),
              inverseTransformationMatrix(inverseTransformationMatrix)
        {
            normalTransformationMatrix = inverseTransformationMatrix.getTranspose();
        }
        
        const Matrix4 & getTransformationMatrix() const
        {
            return this->transformationMatrix;
        }
        
        const Matrix4 & getInverseTransformationMatrix() const
        {
            return this->inverseTransformationMatrix;
        }
        
        Transformation operator+(const Transformation & rhs)
        {
            Matrix4 newTMatrix = rhs.transformationMatrix * this->transformationMatrix;
            Matrix4 newInverseTMatrix = this->inverseTransformationMatrix * rhs.inverseTransformationMatrix;
            
            return Transformation(newTMatrix, newInverseTMatrix);
        }
        
        Transformation & operator+=(const Transformation & rhs)
        {
            Transformation newTransformation = *this + rhs;
            
            this->transformationMatrix = newTransformation.transformationMatrix;
            this->inverseTransformationMatrix = newTransformation.inverseTransformationMatrix;
            this->normalTransformationMatrix = this->inverseTransformationMatrix.getTranspose();
            
            return *this;
        }
        
        template<class T>
        void transformMutating(T & transformable) const
        {
            transformable = this->transformationMatrix * transformable;
        }
        
        template<class T>
        T transform(const T & transformable) const
        {
            return this->transformationMatrix * transformable;
        }

        template<class T>
        T inverseTransform(const T & transformable) const
        {
            return this->inverseTransformationMatrix * transformable;
        }

        Vector3 normalTransform(const Vector3 & normal) const
        {
            return normalTransformationMatrix * normal;
        }
        
        HitInfo hitInfoTransform(const HitInfo & hitInfo, const Ray & originalRay) const
        {
            HitInfo result = hitInfo;

            result.hitPosition = this->transform(hitInfo.hitPosition);
            result.normal = this->normalTransform(hitInfo.normal).normalize();
            result.t = originalRay.getTValue(result.hitPosition);

            return result;
        }
        
};

class Scaling : public Transformation
{

    public:
        Scaling(float scaleX, float scaleY, float scaleZ)
        {
            transformationMatrix.setEl(0, 0, scaleX);
            transformationMatrix.setEl(1, 1, scaleY);
            transformationMatrix.setEl(2, 2, scaleZ);

            inverseTransformationMatrix.setEl(0, 0, 1 / scaleX);
            inverseTransformationMatrix.setEl(1, 1, 1 / scaleY);
            inverseTransformationMatrix.setEl(2, 2, 1 / scaleZ);
        }
};

class Rotation : public Transformation
{
    public:
        Rotation(float rotationAngle, Vector3 rotationAxis) {

            Vector3 & u = rotationAxis.normalize();
            
            Vector3 v = u.generateDifferentlyDirectedVector();
            v.normalize();
            
            Vector3 w = u * v;
            w.normalize();
            
            // then, correct v
            v = w * u;
            v.normalize();
            
            
            float MArray[4][4] = {
                { u.getX(), u.getY(), u.getZ(), 0 },
                { v.getX(), v.getY(), v.getZ(), 0 },
                { w.getX(), w.getY(), w.getZ(), 0 },
                {        0,        0,        0, 1 }
            };
            
            float MInverseArray[4][4] = {
                { u.getX(), v.getX(), w.getX(), 0 },
                { u.getY(), v.getY(), w.getY(), 0 },
                { u.getZ(), v.getZ(), w.getZ(), 0 },
                {        0,        0,        0, 1 }
            };

            float cosine = cos(rotationAngle * M_PI/180);
            float sine = sin(rotationAngle * M_PI/180); 
            
            float rotationArray[4][4] = {
                { 1,      0,      0, 0 },
                { 0, cosine,  -sine, 0 },
                { 0,   sine, cosine, 0 },
                { 0,      0,      0, 1 }
            };      
            
            float inverseRotationArray[4][4] = {
                { 1,      0,      0, 0 },
                { 0, cosine,   sine, 0 },
                { 0,  -sine, cosine, 0 },
                { 0,      0,      0, 1 }
            };
            
            Matrix4 M(MArray);
            Matrix4 inverseM(MInverseArray);
            
            transformationMatrix = inverseM * Matrix4(rotationArray) * M;        
            inverseTransformationMatrix =  inverseM * Matrix4(inverseRotationArray) * M;
        }
};

class Translation : public Transformation
{

    public:
        Translation(float translationX, float translationY, float translationZ)
        {
            transformationMatrix.setEl(0, 3, translationX);
            transformationMatrix.setEl(1, 3, translationY);
            transformationMatrix.setEl(2, 3, translationZ);

            inverseTransformationMatrix.setEl(0, 3, -translationX);
            inverseTransformationMatrix.setEl(1, 3, -translationY);
            inverseTransformationMatrix.setEl(2, 3, -translationZ);
        }

        Translation(Vector3 motionBlurVector)
            : Translation(motionBlurVector.getX(), motionBlurVector.getY(), motionBlurVector.getZ())
         { }
    
};

#endif
