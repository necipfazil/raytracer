#include "../headers/brdf.hpp"
#include "../headers/structs.hpp"
#include "../headers/ray.hpp"
#include "../headers/vector3.hpp"
#include "../../image/color.hpp"
#include <cmath>

Color getDiffuseDefault(const HitInfo & hitInfo, const IncidentLight& incidentLight)
{    
    if(incidentLight.inShadow)
        return Color::Black();

    float normalDotLight = (hitInfo.normal ^ incidentLight.hitToLightDirection);

    if(normalDotLight < 0.f)
        normalDotLight = 0.f;

    Color diffuseColor = Color(hitInfo.material.getDiffuse().intensify(incidentLight.intensity) * normalDotLight);

    return diffuseColor;
}


Color getSpecularDefault(const Ray & ray, const HitInfo & hitInfo, const IncidentLight& incidentLight)
{
    if(incidentLight.inShadow)
        return Color::Black();

    Vector3 h = (( -ray.getDirection()) + incidentLight.hitToLightDirection ).normalize();
    
    float max = hitInfo.normal ^ h;
    max = max < 0.0f ? 0.0f : max;
    max = pow(max, hitInfo.material.getPhongExponent());
    
    Vector3 colorVector = hitInfo.material.getSpecular().intensify(incidentLight.intensity) * max;
    
    return Color(colorVector);
}

// make it func of brdf
Color getPhong(const Ray & ray, const HitInfo & hitInfo, const IncidentLight& incidentLight, bool modified, bool normalized)
{
    if(incidentLight.inShadow)
        return Color::Black();
    
    const Vector3 w_i = incidentLight.hitToLightDirection;
    const Vector3 w_o = -ray.getDirection();

    // cosinus of the angle between w_i and surface normal
    const float cosTheta_i = w_i ^ hitInfo.normal;

    // if theta_i < 90, then, return 0
    if(cosTheta_i < 0.f)
        return Color::Black();

    Vector3 reflectionDirection = -w_i - (hitInfo.normal * (2.0 * (-w_i ^ hitInfo.normal)));
    reflectionDirection.normalize();

    // cosinus of the angle between perfect reflection direction of wi and wo
    float cosAlfa_r = reflectionDirection ^ w_o;
    
    // raise to exponent p
    float raisedCosAlfa_r = pow(cosAlfa_r, hitInfo.material.getBRDF().getExponent());
    const Material& material = hitInfo.material;

    Vector3 diffuse, specular;

    // compute diffuse & specular according to modified and normalized
    if(!modified && !normalized)
    {
        diffuse = material.getDiffuse();
        specular = material.getSpecular() * ( raisedCosAlfa_r / cosTheta_i );
    }
    else if(modified && !normalized)
    {
        diffuse = material.getDiffuse();
        specular = material.getSpecular() * raisedCosAlfa_r;
    }
    else if(modified && normalized)
    {
        diffuse = material.getDiffuse() * M_1_PI;
        specular = material.getSpecular() * raisedCosAlfa_r * (hitInfo.material.getBRDF().getExponent() + 2) * M_1_PI * 0.5f;
    }

    Vector3 result = ((diffuse + specular) * cosTheta_i).elementwiseMultiply(incidentLight.intensity);

    return Color(result);
}

// Methods for other brdfs
Color BRDF::computeReflectedLight(const Ray & ray, const HitInfo & hitInfo, const IncidentLight& incidentLight) const
{
    Color color = Color::Black();

    switch(this->mode)
    {
        case DEFAULT:
            color += getDiffuseDefault(hitInfo, incidentLight);
            color += getSpecularDefault(ray, hitInfo, incidentLight);
            break;
        case PHONG:
            color += getPhong(ray, hitInfo, incidentLight, false, false);
            break;
        case PHONG_MODIFIED:
            if(!normalized) color += getPhong(ray, hitInfo, incidentLight, true, false);
            else            color += getPhong(ray, hitInfo, incidentLight, true, true);
            break;
        case BLINNPHONG:
            break;
        case BLINNPHONG_MODIFIED:
            break;
    }

    return color;
}