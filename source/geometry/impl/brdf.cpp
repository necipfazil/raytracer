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

Color getBlinnPhong(const Ray & ray, const HitInfo & hitInfo, const IncidentLight& incidentLight, bool modified, bool normalized)
{
    if(incidentLight.inShadow)
        return Color::Black();

    const Vector3 w_i = incidentLight.hitToLightDirection;
    const Vector3 w_o = -ray.getDirection();

    const Material& material = hitInfo.material;

    // cosinus of the angle between w_i and surface normal
    const float cosTheta_i = w_i ^ hitInfo.normal;

    // if theta_i < 90, then, return 0
    if(cosTheta_i < 0.f)
        return Color::Black();

    // half vector
    Vector3 w_h = (w_i + w_o).normalize();

    // cosinus of the angle between half vector and surface normal
    float cosAlfa_h = w_h ^ hitInfo.normal;

    // raise to exponent p
    float raisedCosAlfa_h = pow(cosAlfa_h, material.getBRDF().getExponent());

    Vector3 diffuse, specular;

    if(!modified && !normalized)
    {
        diffuse = material.getDiffuse();
        specular = material.getSpecular() * (raisedCosAlfa_h / cosTheta_i);
    }
    else if(modified && !normalized)
    {
        diffuse = material.getDiffuse();
        specular = material.getSpecular() * raisedCosAlfa_h;
    }
    else if(modified && normalized)
    {
        diffuse = material.getDiffuse() * M_1_PI;
        specular = material.getSpecular() * (material.getBRDF().getExponent() + 8) * (1 / 8.f) * M_1_PI * raisedCosAlfa_h;
    }

    Vector3 result = ((diffuse + specular) * cosTheta_i).elementwiseMultiply(incidentLight.intensity);

    return Color(result);
}

float computeGeometryTerm(const Vector3& normal, const Vector3& w_h, const Vector3& w_o, const Vector3& w_i)
{
    float n_dot_wh = normal ^ w_h;
    float n_dot_wo = normal ^ w_o;
    float n_dot_wi = normal ^ w_i;
    float wo_dot_wh = w_o ^ w_h;

    float p1 = 2 * n_dot_wh * n_dot_wo * (1 / wo_dot_wh);
    float p2 = 2 * n_dot_wh * n_dot_wi * (1 / wo_dot_wh);

    return std::min(1.f, std::min(p1, p2));
}

float computeFresnelReflectance(float refractiveIndex, float cosBeta)
{
    float R_0 = pow((refractiveIndex - 1) / (refractiveIndex + 1), 2);

    return R_0 + (1 - R_0) * pow(1 - cosBeta, 5);
}

Color getTorranceSparrow(const Ray & ray, const HitInfo & hitInfo, const IncidentLight& incidentLight)
{
    if(incidentLight.inShadow)
        return Color::Black();

    const Vector3 w_i = incidentLight.hitToLightDirection;
    const Vector3 w_o = -ray.getDirection();

    const Material& material = hitInfo.material;
    float exponent = material.getBRDF().getExponent();

    // cosinus of the angle between w_i and surface normal
    const float cosTheta_i = w_i ^ hitInfo.normal;

    // if theta_i < 90, then, return 0
    if(cosTheta_i < 0.f)
        return Color::Black();

    // half vector
    Vector3 w_h = (w_i + w_o).normalize();

    // cosinus of the angle between half vector and surface normal
    float cosAlfa_h = w_h ^ hitInfo.normal;

    // raise to exponent p
    float raisedCosAlfa_h = pow(cosAlfa_h, exponent);

    // D(alfa)
    float prob = (exponent + 2)  * 0.5f * M_1_PI * raisedCosAlfa_h;

    // compute geometry term G(w_i, w_o)
    float geometryTerm = computeGeometryTerm(hitInfo.normal, w_h, w_o, w_i);

    // compute fresnel reflectance
    float fresnel = computeFresnelReflectance(material.getBRDF().getRefractiveIndex(), w_o ^ w_h);

    Vector3 diffuse = material.getDiffuse() * M_1_PI;
    Vector3 specular = material.getSpecular() * prob * fresnel * geometryTerm;
            specular /= 4 * cosTheta_i * (hitInfo.normal ^ w_o);

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
            color += getBlinnPhong(ray, hitInfo, incidentLight, false, false);
            break;
        case BLINNPHONG_MODIFIED:
            if(!normalized) color += getBlinnPhong(ray, hitInfo, incidentLight, true, false);
            else            color += getBlinnPhong(ray, hitInfo, incidentLight, true, true);
            break;
        case TORRANCE_SPARROW:
            color += getTorranceSparrow(ray, hitInfo, incidentLight);
            break;
    }

    return color;
}