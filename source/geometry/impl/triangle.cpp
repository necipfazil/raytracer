#include "../../config.h"
#include "../headers/triangle.hpp"
#include "../../utility/random_number_generator.hpp"
#include <iostream>

//#define _BACKFACE_CULLING_

Triangle::Triangle(
    const Material & material,
    const Vertex & vertex0,
    const Vertex & vertex1,
    const Vertex & vertex2,
    const ShadingMode & shadingMode
) : Surface(material),
    vertex{vertex0, vertex1, vertex2},
    shadingMode(shadingMode),
    normal(computeNormal(vertex0, vertex1, vertex2))
{
    minPosition = computeMinPosition();
    maxPosition = computeMaxPosition();

    fillLookUpTable();
}

Triangle::Triangle(
    const Vertex & vertex0,
    const Vertex & vertex1,
    const Vertex & vertex2,
    const ShadingMode & shadingMode
) : vertex{vertex0, vertex1, vertex2},
    shadingMode(shadingMode),
    normal(computeNormal(vertex0, vertex1, vertex2))
{
    minPosition = computeMinPosition();
    maxPosition = computeMaxPosition();

    fillLookUpTable();
}

Position3 Triangle::computeMinPosition() const
{
    // initialize with vertex 0
    float minX = this->getVertex(0).getX();
    float minY = this->getVertex(0).getY();
    float minZ = this->getVertex(0).getZ();

    // update if necessary

    // comparing with vertex 1
        // X
    minX = minX < this->getVertex(1).getX() ? minX : this->getVertex(1).getX();
        // Y
    minY = minY < this->getVertex(1).getY() ? minY : this->getVertex(1).getY();
        // Z
    minZ = minZ < this->getVertex(1).getZ() ? minZ : this->getVertex(1).getZ();

    // comparing with vertex 2
        // X
    minX = minX < this->getVertex(2).getX() ? minX : this->getVertex(2).getX();
        // Y
    minY = minY < this->getVertex(2).getY() ? minY : this->getVertex(2).getY();
        // Z
    minZ = minZ < this->getVertex(2).getZ() ? minZ : this->getVertex(2).getZ();

    return Position3(minX, minY, minZ);
}

Position3 Triangle::computeMaxPosition() const
{
    // initialize with vertex 0
    float maxX = this->getVertex(0).getX();
    float maxY = this->getVertex(0).getY();
    float maxZ = this->getVertex(0).getZ();

    // update if necessary

    // comparing with vertex 1
        // X
    maxX = maxX > this->getVertex(1).getX() ? maxX : this->getVertex(1).getX();
        // Y
    maxY = maxY > this->getVertex(1).getY() ? maxY : this->getVertex(1).getY();
        // Z
    maxZ = maxZ > this->getVertex(1).getZ() ? maxZ : this->getVertex(1).getZ();

    // comparing with vertex 2
        // X
    maxX = maxX > this->getVertex(2).getX() ? maxX : this->getVertex(2).getX();
        // Y
    maxY = maxY > this->getVertex(2).getY() ? maxY : this->getVertex(2).getY();
        // Z
    maxZ = maxZ > this->getVertex(2).getZ() ? maxZ : this->getVertex(2).getZ();

    return Position3(maxX, maxY, maxZ);
}

Vertex Triangle::getVertex(int vertexId) const
{
    if(vertexId < 3)
        return this->vertex[vertexId];
    else
        return Vertex(-1.0f, -1.0f, -1.0f);
}

Vector3 Triangle::getNormal() const
{
    return this->normal;
}

Vector3 Triangle::computeNormal(const Position3 & vertex0,
                             const Position3 & vertex1,
                             const Position3 & vertex2 )
{
    Vector3 normal = vertex0.to(vertex1) * vertex0.to(vertex2);
    
    normal.normalize();
    
    return normal;
}

Position3 Triangle::getUniformPoint() const
{
    // vectors between vertices
    Vector3 v0_to_v1 = vertex[0].to(vertex[1]);
    Vector3 v1_to_v2 = vertex[1].to(vertex[2]);

    // generate displacement from v0 by uniformly random amount
    float sqrtpsi1 = sqrt(getRandomBtw01());
    float     psi2 = getRandomBtw01();

    v0_to_v1 = v0_to_v1 * sqrtpsi1;
    v1_to_v2 = v1_to_v2 * psi2;

    // apply displacement on v0 to find the point to get the result
    Position3 result = vertex[0] + (v0_to_v1 + v1_to_v2);

    if(this->hasTransformation)
    {
        result = this->transformation.transform(result);
    }

    return result;
}

void Triangle::fillLookUpTable()
{
    const Position3 & a = vertex[0];
    const Position3 & b = vertex[1];
    const Position3 & c = vertex[2];
    
    // A: ax - bx
    float A = lookUpTable.Ax_Bx = a.getX() - b.getX();
    
    // B: ay - by
    float B = lookUpTable.Ay_By = a.getY() - b.getY();
    
    // C: az - bz
    float C = lookUpTable.Az_Bz = a.getZ() - b.getZ();
    
    // D: ax - cx
    float D = lookUpTable.Ax_Cx = a.getX() - c.getX();
    
    // F: ay - cy
    float F = lookUpTable.Ay_Cy = a.getY() - c.getY();
    
    // E: az - cz
    float E = lookUpTable.Az_Cz = a.getZ() - c.getZ();
    
    lookUpTable.A_C = Vector3( lookUpTable.Ax_Cx, lookUpTable.Ay_Cy, lookUpTable.Az_Cz );
    lookUpTable.A_B = Vector3( lookUpTable.Ax_Bx, lookUpTable.Ay_By, lookUpTable.Az_Bz );

    this->area = (a.to(b) * a.to(c)).getNorm() / 2.f;

}

bool Triangle::hit(const Ray& originalRay, HitInfo & hitInfo, bool backfaceCulling, bool opaqueSearch) const
{
    // set time of hit
    hitInfo.time = originalRay.getTimeCreated();
    
    Ray ray = transformRayForIntersection(originalRay);

    const Vector3 & rayDirection = ray.getDirection();
    const Position3 & rayOrigin = ray.getOrigin();
    
    // if the angle between ray and normal bigger than 90 degree, 
    //.. there is no need for coloring this triangle,
    //.. simply ignore the intersection
    //#ifdef __BACKFACE_CULLING__
    if(backfaceCulling && (normal ^ rayDirection) > 0)
    {
        return false;
    }
    //#endif
    
    Vector3 A_O = Vector3( vertex[0].getX() - rayOrigin.getX(),
                           vertex[0].getY() - rayOrigin.getY(),
                           vertex[0].getZ() - rayOrigin.getZ() );
                           
    const Vector3 & A_B = lookUpTable.A_B;
    
    const Vector3 & A_C = lookUpTable.A_C;
     
    const float & a = A_B.getX();
    const float & b = A_B.getY();
    const float & c = A_B.getZ();
    
    const float & d = A_C.getX();
    const float & e = A_C.getY();
    const float & f = A_C.getZ();
     
    const float & g = rayDirection.getX();
    const float & h = rayDirection.getY();
    const float & i = rayDirection.getZ();
     
    const float & j = A_O.getX();
    const float & k = A_O.getY();
    const float & l = A_O.getZ();
     
    const float cv1 = e*i - h*f;
    const float cv2 = g*f - d*i;
    const float cv3 = d*h - e*g;
    const float cv4 = a*k - j*b;
    const float cv5 = j*c - a*l;
    const float cv6 = b*l - k*c;
    
    const float determinantA = a*cv1 + b*cv2 + c*cv3;
     
    if(determinantA == 0.0f)
        return false;
     // z, y, x
    float Y = (i*cv4 + h*cv5 + g*cv6) / determinantA;
     
    if(Y < 0.0f || Y > 1.0f)
        return false;
     // vertex[0] - rayorigin
    float B = (j*cv1 + k*cv2 + l*cv3) / determinantA;
     
    if(B < 0 || B + Y > 1)
        return false;
      
    float T = - (f*cv4 + e*cv5 + d*cv6) / determinantA;

    if(T <= 0.0f)
        return false;

    
    
    // update hit info to inform caller
    hitInfo.t = T;
    hitInfo.hitPosition = ray.getPoint(hitInfo.t);
    if(this->hasMaterial)
        hitInfo.material = this->material;

    if(this->shadingMode == ShadingMode::FLAT)
    {
        hitInfo.normal = this->getNormal();
    }
    else if(this->shadingMode == ShadingMode::SMOOTH)
    {
        Vector3 normal =
            this->vertex[0].getNormal() * (1 - (Y + B)) +
            this->vertex[1].getNormal() * B +
            this->vertex[2].getNormal() * Y;

        hitInfo.normal = normal.normalize();                
    }

    // texture info
    hitInfo.textureInfo.hasTexture = this->hasImageTexture || this->hasPerlinTexture;

    if(this->hasImageTexture)
    {
        hitInfo.textureInfo.decalMode = imageTexture.getDecalMode();

        // compute u and v
        float u =
            texCoord[0].x +
            B * (texCoord[1].x - texCoord[0].x) +
            Y * (texCoord[2].x - texCoord[0].x);

        float v =
            texCoord[0].y +
            B * (texCoord[1].y - texCoord[0].y) +
            Y * (texCoord[2].y - texCoord[0].y);

        // change u and v depending on AppearanceMode
        if(imageTexture.getAppearanceMode() == AppearanceMode::CLAMP)
        {
            u = u < 0.f ? 0.f : u;
            v = v < 0.f ? 0.f : v;

            u = u > 1.f ? 1.f : u;
            v = v > 1.f ? 1.f : v;
        }
        else if(imageTexture.getAppearanceMode() == AppearanceMode::REPEAT)
        {
            u = u - (int)u;
            v = v - (int)v;

            u = u < 0.f ? u + 1 : u;
            v = v < 0.f ? v + 1 : v;
        }

        // assign color
        hitInfo.textureInfo.textureColor = imageTexture.getInterpolatedColor(u, v);

        // check decal mode
        if(imageTexture.getDecalMode() == DecalMode::REPLACE_KD)
        {
            hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
        }
        else if(imageTexture.getDecalMode() == DecalMode::BLEND_KD)
        {
            hitInfo.material.setDiffuse(
                ((hitInfo.textureInfo.textureColor.getVector3()) + hitInfo.material.getDiffuse()) / 2.f
            );
        }

        if(imageTexture.isBump())
        {
            // compute dpdu and dpdv
            
            // TODO: Same for a triangle, compute once
            float a = texCoord[1].x - texCoord[0].x;
            float b = texCoord[1].y - texCoord[0].y;
            float c = texCoord[2].x - texCoord[0].x;
            float d = texCoord[2].y - texCoord[0].y;

            Vector3 b_a = vertex[1] - vertex[0];
            Vector3 b_b = vertex[2] - vertex[0];

            float det = (a*d - b*c);
            a /= det;
            b /= det;
            c /= det;
            d /= det;

            Vector3 dpdu = (b_a * d) + (b_b * -b);
            Vector3 dpdv = (b_a * -c) + (b_b * a);

            Vec2f grd = imageTexture.getGradient(u, v);

            Vector3 dpPrimedu = dpdu + (hitInfo.normal * (grd.x * imageTexture.getBumpMapMultiplier()));
            Vector3 dpPrimedv = dpdv + (hitInfo.normal * (grd.y * imageTexture.getBumpMapMultiplier()));

            // update normal
            hitInfo.normal = (dpPrimedv * dpPrimedu).normalize();
        }
    }
    else if(this->hasPerlinTexture)
    {
        hitInfo.textureInfo.decalMode = perlinTexture.getDecalMode();
        hitInfo.textureInfo.textureColor = perlinTexture.getPerlinColor(hitInfo.hitPosition);

        // check decal mode
        if(perlinTexture.getDecalMode() == DecalMode::REPLACE_KD)
        {
            hitInfo.material.setDiffuse(hitInfo.textureInfo.textureColor.getVector3());
        }
        else if(perlinTexture.getDecalMode() == DecalMode::BLEND_KD)
        {
            hitInfo.material.setDiffuse(
                ((hitInfo.textureInfo.textureColor.getVector3()) + hitInfo.material.getDiffuse()) / 2.f
            );
        }

        if(perlinTexture.isBump())
        {
            Vector3 gradient = perlinTexture.getPerlinColor(hitInfo.hitPosition).getVector3();

            Vector3 gParallel = hitInfo.normal * (gradient ^ hitInfo.normal);
            Vector3 gOrth = gradient - gParallel;

            // update normal
            hitInfo.normal = (hitInfo.normal - gOrth).normalize();
        }
    }
    
    transformHitInfoAfterIntersection(originalRay, hitInfo);

    return true;

}