#ifndef __COLOR_H__
#define __COLOR_H__

#include "../geometry/headers/vector3.hpp"
#include<iostream>

class Color
{
    private:
        float R, G, B;
    public:
        Color(float R = 0.0f, float G = 0.0f, float B = 0.0f)
        {
            this->R = R;
            this->G = G;
            this->B = B;
                        
            this->R = R;
            this->G = G;
            this->B = B;
        }
        
        Color(const Vector3 & colorVector)
        :   Color(colorVector.getX(), colorVector.getY(), colorVector.getZ()) {}
        
        Color operator+(const Color & rhs) const
        {
            return Color( R + rhs.getFR(),
                          G + rhs.getFG(),
                          B + rhs.getFB() );
        }

        Vector3 getVector3() const
        {
            return Vector3(R, G, B);
        }
        
        Color & intensify(const Vector3 & rhs)
        {
            this->setR(this->R * rhs.getX());
            this->setG(this->G * rhs.getY());
            this->setB(this->B * rhs.getZ());
            
            return *this;
        }

        Color & intensify(float weight)
        {
            this->setR(this->R * weight);
            this->setG(this->G * weight);
            this->setB(this->B * weight);
            
            return *this;
        }
        
        Color & operator+=(const Color & rhs)
        {
            setR( R + rhs.getFR() );
            setG( G + rhs.getFG() );
            setB( B + rhs.getFB() );

            return *this;
        }

        Color operator/(float divider)
        {
            return Color(R / divider, G / divider, B / divider);
        }
        
        float getFR() const
        {
            return this->R;
        }
        
        float getFG() const
        {
            return this->G;
        }
        
        float getFB() const
        {
            return this->B;
        }
        
        unsigned char getR() const
        {
            if(this->R < 0.0f)
                return (unsigned char)0;
            
            if(this->R > 255.0f)
                return (unsigned char)255;
            
            return (unsigned char)this->R;
        }
        unsigned char getG() const
        {
            if(this->G < 0.0f)
                return (unsigned char)0;
            
            if(this->G > 255.0f)
                return (unsigned char)255;
                
            return (unsigned char)this->G;
        }
        
        unsigned char getB() const
        {
            if(this->B < 0.0f)
                return (unsigned char)0;
            
            if(this->B > 255.0f)
                return (unsigned char)255;
                
            return (unsigned char)this->B;
        }
        
        void setR(float R)
        {
            this->R = R;
        }
        
        void setG(float G)
        {
            this->G = G;
        }
        void setB(float B)
        {
            this->B = B;
        }

        bool isBlack() const
        {
            return getR() == 0 && getG() == 0 && getB() == 0;
        }

        static Color Black()
        {
            return Color(0.0f, 0.0f, 0.0f);
        }
        
        static Color White()
        {
            return Color(255.0f, 255.0f, 255.0f);
        }

        static Color Red()
        {
            return Color(255.f, 0.f, 0.f);
        }

        static Color Blue()
        {
            return Color(0.f, 0.f, 255.f);
        }

        static Color Green()
        {
            return Color(0.f, 255.f, 0.f);
        }
};

//std::ostream &operator<<(std::ostream &output, const Color & color);

#endif
