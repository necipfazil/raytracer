#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "shape.hpp"
#include "structs.hpp"
#include "ray.hpp"
#include "position3.hpp"
#include "texture.hpp"

// abstract class for surfaces that can be hit by a ray & has a material to be rendered
class Surface : public Shape
{
    protected:
        Surface(const Material& material) : Shape(material) { }

        Surface() { }

        bool hasImageTexture = false;
        bool hasPerlinTexture = false;
        ImageTexture imageTexture;
        PerlinTexture perlinTexture;
    public:
        // returns true if ray hits the surface and records the hit position
        // .. in hitPosition object
        virtual bool hit(const Ray & ray, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const = 0;

        void setTexture(Texture* texture)
        {
            if(texture->getTextureType() == TextureType::IMAGE)
            {
                hasImageTexture = true;
                imageTexture = *((ImageTexture*)texture);
            }
            else if(texture->getTextureType() == TextureType::PERLIN)
            {
                hasPerlinTexture = true;
                perlinTexture = *((PerlinTexture*)texture);
            }
        }
};

#endif