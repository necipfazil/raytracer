#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "vector3.hpp"
#include "brdf.hpp"

class Material
{
    private:
        BRDF brdf;

        Vector3 ambient;
        Vector3 diffuse;
        Vector3 specular;
        Vector3 mirror;
        Vector3 transparency;
        float refractionIndex;
        float phongExponent;
        float roughness;

    public:
        Material() : phongExponent(0.f), refractionIndex(1.f), roughness(0.f) { brdf.setMode(BRDF::Mode::DEFAULT); }

        BRDF getBRDF() const;
        Vector3 getAmbient() const;
        Vector3 getDiffuse() const;
        Vector3 getSpecular() const;
        Vector3 getMirror() const;
        Vector3 getTransparency() const;
        float getPhongExponent() const;
        float getRefractionIndex() const;
        float getRoughness() const;

        void setBRDF(const BRDF& brdf);
        void setAmbient(const Vector3& ambient);
        void setDiffuse(const Vector3& diffuse);
        void setSpecular(const Vector3& specular);
        void setMirror(const Vector3& mirror);
        void setTransparency(const Vector3& transparency);
        void setPhongExponent(float phongExponent);
        void setRefractionIndex(float refractionIndex);
        void setRoughness(float roughness);

        void degamma();
};

#endif
