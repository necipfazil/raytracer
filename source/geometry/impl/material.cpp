#include "../headers/material.hpp"
#include "../headers/brdf.hpp"

BRDF Material::getBRDF() const
{
    return this->brdf;
}

Vector3 Material::getAmbient() const
{
    return this->ambient;
}

Vector3 Material::getDiffuse() const
{
    return this->diffuse;
}

Vector3 Material::getSpecular() const
{
    return this->specular;
}

Vector3 Material::getMirror() const
{
    return this->mirror;
}

float Material::getPhongExponent() const
{
    return this->phongExponent;
}

Vector3 Material::getTransparency() const
{
    return this->transparency;
}

float Material::getRefractionIndex() const
{
    return this->refractionIndex;
}

float Material::getRoughness() const
{
    return this->roughness;
}


void Material::setBRDF(const BRDF& brdf)
{
    this->brdf = brdf;
}

void Material::setAmbient(const Vector3& ambient)
{
    this->ambient = ambient;
}

void Material::setDiffuse(const Vector3& diffuse)
{
    this->diffuse = diffuse;
}

void Material::setSpecular(const Vector3& specular)
{
    this->specular = specular;
}

void Material::setMirror(const Vector3& mirror)
{
    this->mirror = mirror;
}

void Material::setPhongExponent(float phongExponent)
{
    this->phongExponent = phongExponent;
}

void Material::setTransparency(const Vector3& transparency)
{
    this->transparency = transparency;
}

void Material::setRefractionIndex(float refractionIndex)
{
    this->refractionIndex = refractionIndex;
}

void Material::setRoughness(float roughness)
{
    this->roughness = roughness;
}

void Material::degamma()
{
    float gamma = 2.2f;
    
    this->ambient = this->ambient.power(gamma);
    this->diffuse = this->diffuse.power(gamma);
    this->specular = this->specular.power(gamma);
}