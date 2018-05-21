#ifndef __BRDF_H__
#define __BRDF_H__

class BRDF
{
    public:
        enum Mode
        {
            DEFAULT,
            PHONG,
            PHONG_MODIFIED,
            BLINNPHONG,
            BLINNPHONG_MODIFIED,
        };
    
    void setExponent(float exponent) { this->exponent = exponent; }
    float getExponent() const { return this->exponent; }
    
    void setNormalized(bool normalized) { this->normalized = normalized; }
    bool isNormalized() const { return this->normalized; }

    void setMode(Mode mode) { this->mode = mode; }
    Mode getMode() const { return this->mode; }

    private:
        float exponent;
        bool normalized = false;
        Mode mode;
};

#endif