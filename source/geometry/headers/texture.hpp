#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "vec3i.hpp"
#include "vector3.hpp"
#include "position3.hpp"
#include "../../image/image.hpp"
#include "../../image/color.hpp"
#include "../../utility/random_number_generator.hpp"
#include "vec2f.hpp"
#include "iomethods.hpp"
#include <string>
#include <cmath>

enum InterpolationMode
{
    NEAREST,
    BILINEAR
};

enum DecalMode
{
    REPLACE_KD,
    BLEND_KD,
    REPLACE_ALL
};

enum AppearanceMode
{
    CLAMP,
    REPEAT
};

enum PerlinAppearanceMode
{
    PATCH,
    VEIN
};

enum TextureType
{
    IMAGE,
    PERLIN
};



class Texture
{
    private:
        TextureType textureType;
        DecalMode decalMode;
        bool bump = false;
        float bumpMapMultiplier = 1.f;

    public:
        virtual TextureType getTextureType() const = 0;

        bool isBump() const { return this->bump; }
        void setBump(bool bump) { this->bump = bump; }
        float getBumpMapMultiplier() const { return this->bumpMapMultiplier; }
        void setBumpMapMultiplier(float bumpMapMultiplier) { this->bumpMapMultiplier = bumpMapMultiplier; }
        DecalMode getDecalMode() const { return decalMode; }
        void setDecalMode(const DecalMode& decalMode) { this->decalMode = decalMode; }
};

class PerlinTexture : public Texture
{
    private:
        PerlinAppearanceMode appMode;
        float scalingFactor;

        int hashSize = 16;
        std::vector<Vector3> hashTable = {
                Vector3(1, 1, 0),
                Vector3(-1, 1, 0),
                Vector3(1, -1, 0),
                Vector3(-1, -1, 0),
                Vector3(1, 0, 1),
                Vector3(-1, 0, 1),
                Vector3(1, 0, -1),
                Vector3(-1, 0, -1),
                Vector3(0, 1, 1),
                Vector3(0, -1, 1),
                Vector3(0, 1, -1),
                Vector3(0, -1, -1),
                Vector3(1, 1, 0),
                Vector3(-1, 1, 0),
                Vector3(0, -1, 1),
                Vector3(0, -1, -1)
            };

        Vector3 hash(int i, int j, int k) const
        {
            int index = ((((k % hashSize) + j) % hashSize) + i) % hashSize;
            index = index < 0 ? index + 16 : index;

            return hashTable[index];
        }

        Vector3 hash(Vec3i p) const
        {
            return hash(p.x, p.y, p.z);
        }

        // 0 1 ok
        float w(float x) const
        {
            x = std::abs(x);

            float result =  -6 * pow(x, 5) + 15 * pow(x, 4) + -10 * pow(x, 3) + 1; 

            return result;
        }

        float perlinPart(const Position3& originalPoint, const Vec3i& floorPoint) const
        {
            Vector3 edgeVector = hash(floorPoint);

            Vector3 edge2point = Position3(floorPoint.x, floorPoint.y, floorPoint.z).to(originalPoint);

            float result = (edgeVector ^ edge2point) * w(edge2point.getX()) * w(edge2point.getY()) * w(edge2point.getZ());

            return result;
        }

        float perlin(const Position3& point) const
        {
            // floor
            int i = (int)std::floor(point.getX());
            int j = (int)std::floor(point.getY());
            int k = (int)std::floor(point.getZ());

            float value = 0;

            // for each edge(8 edges), find the perlin value
            value += perlinPart(point, Vec3i(i, j, k));
            value += perlinPart(point, Vec3i(i + 1, j, k));
            value += perlinPart(point, Vec3i(i + 1, j + 1, k));
            value += perlinPart(point, Vec3i(i + 1, j, k + 1));
            value += perlinPart(point, Vec3i(i + 1, j + 1, k + 1));
            value += perlinPart(point, Vec3i(i, j + 1, k));
            value += perlinPart(point, Vec3i(i, j + 1, k + 1));
            value += perlinPart(point, Vec3i(i, j, k + 1));

            return value;
        }

    public:
        PerlinTexture() 
        {
            // shuffle hash table
            std::random_shuffle(hashTable.begin(), hashTable.end(), getRand);
        }

        // getters
        PerlinAppearanceMode getAppearanceMode() const { return appMode; }
        float getScalingFactor() const { return scalingFactor; }
        virtual TextureType getTextureType() const { return TextureType::PERLIN; }

        Color getPerlinColor(const Position3& position) const
        {
            float perlinValue = perlin(position);

            if(appMode == PerlinAppearanceMode::PATCH)
            {
                perlinValue = (perlinValue + 1) / 2;
            } 
            else if(appMode == PerlinAppearanceMode::VEIN)
            {
                perlinValue = std::abs(perlinValue);
            }

            // scaling TODO: Or divide?
            perlinValue *= scalingFactor;

            return Color(perlinValue, perlinValue, perlinValue);
        }
        
        // setters
        void setAppearanceMode(const PerlinAppearanceMode& appMode) { this->appMode = appMode; }        
        void setScalingFactor(float scalingFactor) { this->scalingFactor = scalingFactor; }
        
};

class ImageTexture : public Texture
{
    private:
        Image image;
        InterpolationMode interpMode;
        AppearanceMode appMode;
        float normalizer = 255.f;
    
    public:
        ImageTexture() : image(0, 0) { }

        // getters
        Color getTexImageColor(int x, int y) const { return image.getColor(x, y); }
        InterpolationMode getInterpolationMode() const { return interpMode; }
        AppearanceMode getAppearanceMode() const { return appMode; }
        virtual TextureType getTextureType() const { return TextureType::IMAGE; }

        // ref getter
        const Image& getImage() const { return image; }

        // setters
        void setImage(const Image& image) { this->image = image; this->image.setNormalizer(normalizer); }
        void setImage(const std::string& imageFileName) { this->image = Image(imageFileName); }
        void setInterpolationMode(const InterpolationMode& interpMode) { this->interpMode = interpMode; }
        void setAppearanceMode(const AppearanceMode& appMode) { this->appMode = appMode; }
        void setNormalizer(float normalizer) { image.setNormalizer(normalizer); }

        void degammaImage() { image.degamma(); }

        Color getInterpolatedColor(float u, float v) const;
        Vec2f getGradient(float u, float v) const;
};

#endif