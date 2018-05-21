#include "../config.h"
#include "../scene.hpp"
#include "../filemanip/tinyxml2.h"
#include "../geometry/headers/geometry.hpp"
#include "../geometry/headers/transformation.hpp"
#include "../geometry/headers/texture.hpp"
#include "../geometry/headers/spot_light.hpp"
#include "../geometry/headers/brdf.hpp"
#include "../image/image.hpp"
#include "../utility/ply_parser.hpp"
#include "../utility/pair.hpp"
#include <string>
#include <sstream>


// read Position3 from stream to Position3 instance
std::stringstream &operator>>(std::stringstream &st, Position3 & pos);

// read Vector3 from stream to Vector3 instance
std::stringstream &operator>>(std::stringstream &st, Vector3 & vec);

bool doesHaveChild(tinyxml2::XMLElement* element, std::string childNameHoldingInfo)
{
    return element = element->FirstChildElement(childNameHoldingInfo.data());
}

Transformation
parseObjectTransformation(
    tinyxml2::XMLElement* objectElement,
    const std::vector<Translation>& translations,
    const std::vector<Scaling>& scalings,
    const std::vector<Rotation>& rotations
    )
{
    Transformation transformation;

    std::stringstream stream;
    
    auto child = objectElement->FirstChildElement("Transformations");
    if(child)
    {
        stream << child->GetText() << std::endl;
        std::string transformationInfo;

        while(!(stream >> transformationInfo).eof())
        {
            // get index
            int transformationIndex = std::stoi(transformationInfo.substr(1)) - 1; 

            // get the type of transformation
            switch(transformationInfo[0])
            {
                case 's':
                case 'S':
                    transformation += scalings[transformationIndex];
                    break;
                case 'r':
                case 'R':
                    transformation += rotations[transformationIndex];
                    break;
                case 't':
                case 'T':
                    transformation += translations[transformationIndex];
                    break;
            }
        }
    }

    return transformation;
}

template<class T>
T parseChild(tinyxml2::XMLElement* element, std::string childNameHoldingInfo = "")
{
    if(!element)
        throw std::runtime_error("Null instance of XMLElement!");

    if(!childNameHoldingInfo.empty())
    {
        element = element->FirstChildElement(childNameHoldingInfo.data());
    }

    if(!element)
        throw std::runtime_error("Requested child element could not be found in parseChild()");

    std::stringstream stream;

    // read to stream
    stream << element->GetText() << std::endl;

    // read from stream
    T _t;
    stream >> _t;

    return _t;
}

std::vector<Shape*>
createMeshTriangles(
    const std::vector<Vertex>& vertexData,
    const std::vector<Vec3i>& meshVertexIndices,
    ShadingMode shadingMode = ShadingMode::FLAT,
    Texture* texture = nullptr,
    const std::vector<Vec2i> & texCoordData = std::vector<Vec2i>(),
    int textureOffset = 0
)
{
    std::vector<Shape*> trianglesOfMesh;

    if(shadingMode == ShadingMode::SMOOTH)
    {
        // if shading mode is SMOOTH, then, additional normal computation is
        // required. copy the vertex data to manipulate its normals
        std::vector<Vertex> vertexDataCopy(vertexData);

        // manipulate vertices' normals
        for(int i = 0; i < (int)meshVertexIndices.size(); i++)
        {
            Vector3 normal = Triangle::computeNormal(
                vertexData[meshVertexIndices[i].x],
                vertexData[meshVertexIndices[i].y],
                vertexData[meshVertexIndices[i].z]
            );

            // change the data in vertexDataCopy
            vertexDataCopy[meshVertexIndices[i].x].addToNormal(normal);
            vertexDataCopy[meshVertexIndices[i].y].addToNormal(normal);
            vertexDataCopy[meshVertexIndices[i].z].addToNormal(normal);
        }

        // now, add the triangles by normalizing the vertices' normals
        // some computations may be repeated, but not much overhead..
        for(int i = 0; i < (int)meshVertexIndices.size(); i++)
        {
            // normalize normal of vertices
            vertexDataCopy[meshVertexIndices[i].x].normalizeNormal();
            vertexDataCopy[meshVertexIndices[i].y].normalizeNormal();
            vertexDataCopy[meshVertexIndices[i].z].normalizeNormal();

            // create triangle
            Triangle * triangle = new Triangle(
                vertexDataCopy[meshVertexIndices[i].x],  // v0
                vertexDataCopy[meshVertexIndices[i].y],  // v1
                vertexDataCopy[meshVertexIndices[i].z],  // v2
                shadingMode
            );

            // push the triangle to the surfaces vector
            trianglesOfMesh.push_back((Surface*)(triangle));
        }
    }
    else
    {
        // otherwise (FLAT), normal computation is not needed
        for(int i = 0; i < (int)meshVertexIndices.size(); i++)
        {
            // create triangle
            Triangle * triangle = new Triangle(
                vertexData[meshVertexIndices[i].x],  // v0
                vertexData[meshVertexIndices[i].y],  // v1
                vertexData[meshVertexIndices[i].z],  // v2
                shadingMode
            );

            // check texture
            if(texture)
            {
                triangle->setTexture(texture);
                
                // texCoord
                if(texture->getTextureType() == TextureType::IMAGE)
                {    
                    triangle->setTexCoord(
                        texCoordData[meshVertexIndices[i].x + textureOffset],
                        texCoordData[meshVertexIndices[i].y + textureOffset],
                        texCoordData[meshVertexIndices[i].z + textureOffset]
                    );
                }
            }

            // push the triangle to the surfaces vector
            trianglesOfMesh.push_back((Surface*)(triangle));
        }
    }

    return trianglesOfMesh;
}

ToneMappingParam parseToneMapping(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    ToneMappingParam tmParam;

    if(doesHaveChild(element, "TMOOptions"))
    {
        auto child = element->FirstChildElement("TMOOptions");
        stream << child->GetText() << std::endl;

        float keyValue, burnoutPercentage;
        stream >> keyValue >> burnoutPercentage;
        
        tmParam.keyValue = keyValue;
        tmParam.burnoutPercentage = burnoutPercentage;

        stream.clear();
    }

    if(doesHaveChild(element, "Saturation"))
    {
        tmParam.saturation = parseChild<float>(element, "Saturation");
    }

    if(doesHaveChild(element, "Gamma"))
    {
        tmParam.gamma = parseChild<float>(element, "Gamma");
    }

    return tmParam;
}

// Does not set mode of the BRDF
BRDF parseBRDF(tinyxml2::XMLElement* element)
{
    BRDF brdf;

    // exponent
    if(doesHaveChild(element, "Exponent"))
        brdf.setExponent( parseChild<float>(element, "Exponent") );

    // isNormalized
    const char* normalized = element->Attribute("normalized");
    brdf.setNormalized(normalized && normalized[0] == 't');

    return brdf;
}

Camera parseCamera(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    Camera camera;

    // position
    if(doesHaveChild(element, "Position"))
        camera.setPosition(parseChild<Position3>(element, "Position"));

    // gaze
    if(doesHaveChild(element, "Gaze"))
        camera.setGaze(parseChild<Vector3>(element, "Gaze"));

    // up
    if(doesHaveChild(element, "Up"))
        camera.setUp(parseChild<Vector3>(element, "Up"));

    // near plane
    if(doesHaveChild(element, "NearPlane"))
    {
        auto child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        Vec4f nearPlane;
        stream >> nearPlane.x >> nearPlane.y >> nearPlane.z >> nearPlane.w;
        camera.setNearPlane(nearPlane);
    }


    // near distance
    if(doesHaveChild(element, "NearDistance"))
        camera.setNearDistance(parseChild<float>(element, "NearDistance"));

    // image resolution
    if(doesHaveChild(element, "ImageResolution"))
    {
        auto child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
            // image width & height
        float imageWidth, imageHeight;
        stream >> imageWidth >> imageHeight;
        camera.setImageWidth(imageWidth);
        camera.setImageHeight(imageHeight);
    }

    // image name
    if(doesHaveChild(element, "ImageName"))
        camera.setImageName(parseChild<std::string>(element, "ImageName"));

    // NumSamples
    if(doesHaveChild(element, "NumSamples"))
        camera.setNumSamples(parseChild<float>(element, "NumSamples"));
    else
        camera.setNumSamples(1);

    // FocusDistance
    if(doesHaveChild(element, "FocusDistance"))
        camera.setFocusDistance(parseChild<float>(element, "FocusDistance"));
    else
        camera.setFocusDistance(0);

    // ApertureSize
    if(doesHaveChild(element, "ApertureSize"))
        camera.setApertureSize(parseChild<float>(element, "ApertureSize"));
    else
        camera.setApertureSize(0);

    // GazePoint
    if(doesHaveChild(element, "GazePoint"))
        camera.setGazePoint(parseChild<Position3>(element, "GazePoint"));

    // FovY
    if(doesHaveChild(element, "FovY"))
        camera.setFovY(parseChild<float>(element, "FovY"));

    // ToneMapping
    if(doesHaveChild(element, "Tonemap"))
    {
        camera.setToneMapping(parseToneMapping(element->FirstChildElement("Tonemap")));
    }

    // if camera type is simple, make required computations and set other fields
    const char * cameraType = element->Attribute("type");

    if(cameraType && cameraType[0] == 's')
    {
        camera.computeCoordSys(Camera::CameraType::SIMPLE);

    }
    else
    {
        camera.computeCoordSys(Camera::CameraType::COMPLEX);
    }

    return camera;
}

PointLight parsePointLight(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    // read point light info to stream
    auto child = element->FirstChildElement("Position");
    stream << child->GetText() << std::endl;
    child = element->FirstChildElement("Intensity");
    stream << child->GetText() << std::endl;

    // read point light info from stream
        // position
    Position3 position;
    stream >> position;
        // intensity
    Vector3 intensity;
    stream >> intensity;

    // push the point light to the pointlights vector
    return PointLight(position, intensity);
}

SpotLight parseSpotLight(tinyxml2::XMLElement* element)
{
    Position3 position;
    Vector3 direction, intensity;
    float coverageAngle, falloffAngle;

    // position
    if(doesHaveChild(element, "Position"))
    {
        position = parseChild<Position3>(element, "Position");
    }

    // direction
    if(doesHaveChild(element, "Direction"))
    {
        direction = parseChild<Vector3>(element, "Direction");
    }

    // intensity
    if(doesHaveChild(element, "Intensity"))
    {
        intensity = parseChild<Vector3>(element, "Intensity");
    }

    // coverage angle
    if(doesHaveChild(element, "CoverageAngle"))
    {
        coverageAngle = parseChild<float>(element, "CoverageAngle");
    }

    // falloff angle
    if(doesHaveChild(element, "FalloffAngle"))
    {
        falloffAngle = parseChild<float>(element, "FalloffAngle");
    }

    return SpotLight(position, direction, intensity, coverageAngle, falloffAngle);
}

AreaLight parseAreaLight(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    // read area light info to stream
    auto child = element->FirstChildElement("Position");
    stream << child->GetText() << std::endl;
    child = element->FirstChildElement("Intensity");
    stream << child->GetText() << std::endl;
    child = element->FirstChildElement("EdgeVector1");
    stream << child->GetText() << std::endl;
    child = element->FirstChildElement("EdgeVector2");
    stream << child->GetText() << std::endl;

    // read point light info from stream
        // position
    Position3 position;
    stream >> position;
        // intensity
    Vector3 intensity;
    stream >> intensity;
        // edge vectors
    Vector3 edgeVectors[2];
    stream >> edgeVectors[0];
    stream >> edgeVectors[1];

    // push the area light to the arealights vector
    return AreaLight(position, intensity, edgeVectors);
}

Color parseBackgroundColor(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        // if not specified, get default
        stream << DEFAULT_BACKGROUND_COLOR << std::endl;
    }

    // set BackgroundColor
    int bgColorR, bgColorG, bgColorB;
    stream >> bgColorR >> bgColorG >> bgColorB;
    
    Color backgroundColor;
    backgroundColor.setR(bgColorR);
    backgroundColor.setG(bgColorG);
    backgroundColor.setB(bgColorB);

    return backgroundColor;
}

Material parseMaterial(tinyxml2::XMLElement* element)
{
    std::stringstream stream;

    Material material;
        
        // read ambient reflectance
    auto child = element->FirstChildElement("AmbientReflectance");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        Vector3 ambient;
        stream >> ambient;
            
        // set ambient
        material.setAmbient(ambient);
    }

        // read diffuse reflectance
    child = element->FirstChildElement("DiffuseReflectance");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        Vector3 diffuse;
        stream >> diffuse;

        // set diffuse
        material.setDiffuse(diffuse);
    }
        
        // read specular reflectance
    child = element->FirstChildElement("SpecularReflectance");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        Vector3 specular;
        stream >> specular;

        // set specular
        material.setSpecular(specular);
    }
        
        // read mirror reflectance
    child = element->FirstChildElement("MirrorReflectance");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        Vector3 mirror;
        stream >> mirror;

        // set mirror
        material.setMirror(mirror);
    }

        // read transparency
    child = element->FirstChildElement("Transparency");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        Vector3 transparency;
        stream >> transparency;

        // set transparency
        material.setTransparency(transparency);
    }

        // read refraction index
    if(doesHaveChild(element, "RefractionIndex"))
        material.setRefractionIndex(parseChild<float>(element, "RefractionIndex"));
        
        // phong exponent
    child = element->FirstChildElement("PhongExponent");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        float phongExponent;
        stream >> phongExponent;

        material.setPhongExponent(phongExponent);
    }

        // roughness
    child = element->FirstChildElement("Roughness");
    if(child)
    {
        // read to stream
        stream << child->GetText() << std::endl;

        // read from stream
        float roughness;
        stream >> roughness;

        material.setRoughness(roughness);
    }

        // degamma
    const char * degamma = element->Attribute("degamma");
    bool isDegamma = degamma && degamma[0] == 't';

    if(isDegamma)
    {
        // TODO
        material.degamma();
    }
    
    // push material to the materials vector
    return material;
}

// parses ImageTexture specific attributes
ImageTexture* parseImageTexture(tinyxml2::XMLElement* element)
{
    ImageTexture* texture = new ImageTexture();

    // Image
    if(doesHaveChild(element, "ImageName"))
    {
        std::string imageName = parseChild<std::string>(element, "ImageName");
        texture->setImage(imageName);
    }

    // Interpolation
    if(doesHaveChild(element, "Interpolation"))
    {
        std::string interpModeString = parseChild<std::string>(element, "Interpolation");
        switch(interpModeString[0])
        {
            case 'b':
            case 'B':
                texture->setInterpolationMode(InterpolationMode::BILINEAR);
                break;
            case 'n':
            case 'N':
                texture->setInterpolationMode(InterpolationMode::NEAREST);
                break;
        }
    }

    // AppearanceMode
    if(doesHaveChild(element, "Appearance"))
    {
        std::string appModeString = parseChild<std::string>(element, "Appearance");

        switch(appModeString[0])
        {
            case 'c':
            case 'C':
                texture->setAppearanceMode(AppearanceMode::CLAMP);
                break;
            case 'r':
            case 'R':
                texture->setAppearanceMode(AppearanceMode::REPEAT);
                break;
        }
    }

    // decal mode
    if(doesHaveChild(element, "DecalMode"))
    {
        std::string decalModeString = parseChild<std::string>(element, "DecalMode");
        if(decalModeString == "blend_kd")
        {
            texture->setDecalMode(DecalMode::BLEND_KD);
        }
        else if(decalModeString == "replace_kd")
        {
            texture->setDecalMode(DecalMode::REPLACE_KD);
        }
        else if(decalModeString == "replace_all")
        {
            texture->setDecalMode(DecalMode::REPLACE_ALL);
        }
    }

    // Normalizer
    if(doesHaveChild(element, "Normalizer"))
    {
        float normalizer = parseChild<float>(element, "Normalizer");
        texture->setNormalizer(normalizer);
    }
    else
    {
        switch(texture->getDecalMode())
        {
            case BLEND_KD:
            case REPLACE_KD:
                texture->setNormalizer(255.f);
                break;
            case REPLACE_ALL:
                texture->setNormalizer(1.f);
                break;
        }
    }

    return texture;
}

// parses PerlinTexture specific attributes
PerlinTexture* parsePerlinTexture(tinyxml2::XMLElement* element)
{
    PerlinTexture* texture = new PerlinTexture();

    // scaling factor
    if(doesHaveChild(element, "ScalingFactor"))
    {
        float scalingFactor = parseChild<float>(element, "ScalingFactor");
        texture->setScalingFactor(scalingFactor);
    }
    else
    {
        texture->setScalingFactor(1.f);
    }

    // PerlinAppearanceMode
    if(doesHaveChild(element, "Appearance"))
    {
        std::string appModeString = parseChild<std::string>(element, "Appearance");

        switch(appModeString[0])
        {
            case 'p':
            case 'P':
                texture->setAppearanceMode(PerlinAppearanceMode::PATCH);
                break;
            case 'v':
            case 'V':
                texture->setAppearanceMode(PerlinAppearanceMode::VEIN);
                break;
        }
    }

    // decal mode
    if(doesHaveChild(element, "DecalMode"))
    {
        std::string decalModeString = parseChild<std::string>(element, "DecalMode");
        if(decalModeString == "blend_kd")
        {
            texture->setDecalMode(DecalMode::BLEND_KD);
        }
        else if(decalModeString == "replace_kd")
        {
            texture->setDecalMode(DecalMode::REPLACE_KD);
        }
        else if(decalModeString == "replace_all")
        {
            texture->setDecalMode(DecalMode::REPLACE_ALL);
        }
    }
    
    return texture;
}

Texture* parseTexture(tinyxml2::XMLElement* element)
{
    Texture* texture = nullptr;

    // image name
    if(doesHaveChild(element, "ImageName"))
    {
        std::string imageName = parseChild<std::string>(element, "ImageName");

        if(imageName != "perlin")
        {
            texture = parseImageTexture(element);


            // TODO
            /*
            ImageTexture imgText = *(ImageTexture*)texture;

            const Image& img = imgText.getImage();
            Image newImg(img.getWidth(), img.getHeight());

            for(int x = 0; x < img.getWidth(); x++)
            {
                for(int y = 0; y < img.getHeight(); y++)
                {
                    newImg.setColor(x, y, img.getColor(x, y));
                }
            }
            newImg.write("new.jpeg");
            exit(0);
*/
            // TODO
        }
        else
        {
            texture = parsePerlinTexture(element);
        }
    }

    // is bump?
    const char * bumpmap = element->Attribute("bumpmap");
    bool isBumpMap = bumpmap && bumpmap[0] == 't';
    texture->setBump(isBumpMap);

    // bumpmap multiplier
    if(isBumpMap)
    {
        float multiplier = 1.f;
        element->QueryAttribute("bumpmapMultiplier", &multiplier);
        texture->setBumpMapMultiplier(multiplier);
    }
    
    return texture;
}

void Scene::loadFromXml(const std::string& filepath)
{
    tinyxml2::XMLDocument file;
    std::stringstream stream;

    std::vector<Shape*> shapes;

    // transformation vectors
    std::vector<Scaling> scalings;
    std::vector<Translation> translations;
    std::vector<Rotation> rotations;
    std::vector<Texture*> textures; // to be cleaned after required assignments
    std::vector<Vec2i> texCoordData;
    
    auto res = file.LoadFile(filepath.data());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    // read the root (scene)
    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //
    // BackgroundColor
    //
    auto element = root->FirstChildElement("BackgroundColor");
    this->backgroundColor = parseBackgroundColor(element);

    //
    // ShadowRayEpsilon
    //
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        // if not specified, get it as 0.001
        stream << DEFAULT_SHADOW_RAY_EPSILON << std::endl;
    }

    stream >> this->shadowRayEpsilon;

    //
    // MaxRecursionDepth
    //
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        // if not specified, get as 0
        stream << DEFAULT_MAXRECURSIONDEPTH << std::endl;
    }

    stream >> this->maxRecursionDepth;

    //
    // Camera
    //
    {
        element = root->FirstChildElement("Cameras");
        element = element->FirstChildElement("Camera");

        while (element)
        {
            // parse camera info
            this->cameras.push_back(parseCamera(element));

            // read next camera sibling
            element = element->NextSiblingElement("Camera");
        }
    }
    
    //
    // Lights
    //
    element = root->FirstChildElement("Lights");
        //
        // ambient light
        //
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> this->ambientLight;
        //
        // point lights
        //
    element = element->FirstChildElement("PointLight");
    while (element)
    {
        this->pointLights.push_back(parsePointLight(element));

        // read the next point light sibling
        element = element->NextSiblingElement("PointLight");
    }
        //
        // area lights
        //
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("AreaLight");
    while (element)
    {
        this->areaLights.push_back(parseAreaLight(element));

        // read the next area light sibling
        element = element->NextSiblingElement("AreaLight");
    }
        //
        // directional lights
        //
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("DirectionalLight");
    while(element)
    {
        Vector3 direction, radiance;

        if(doesHaveChild(element, "Direction"))
        {
            direction = parseChild<Vector3>(element, "Direction");
        }

        if(doesHaveChild(element, "Radiance"))
        {
            radiance = parseChild<Vector3>(element, "Radiance");
        }

        this->directionalLights.push_back(DirectionalLight(direction, radiance));

        // read the next sibling
        element = element->NextSiblingElement("DirectionalLight");
    }
        //
        // spot lights
        //
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("SpotLight");
    while(element)
    {
        this->spotLights.push_back(parseSpotLight(element));

        // read the next sibling
        element = element->NextSiblingElement("SpotLight");
    }

    //
    // BRDFs
    //
    element = root->FirstChildElement("BRDFs");

    if(element)
    {
        // OriginalPhong
        auto child = element->FirstChildElement("OriginalPhong");
        while(child)
        {
            BRDF brdf = parseBRDF(child);

            // mode
            brdf.setMode(BRDF::Mode::PHONG);

            // push to brdfs vector
            this->brdfs.push_back(brdf);
            
            child = child->NextSiblingElement("OriginalPhong");
        }
        
        // ModifiedPhong
        child = element->FirstChildElement("ModifiedPhong");
        while(child)
        {
            BRDF brdf = parseBRDF(child);

            // mode
            brdf.setMode(BRDF::Mode::PHONG_MODIFIED);

            // push to brdfs vector
            this->brdfs.push_back(brdf);
            
            child = child->NextSiblingElement("ModifiedPhong");
        }

        // OriginalBlinnPhong
        child = element->FirstChildElement("OriginalBlinnPhong");
        while(child)
        {
            BRDF brdf = parseBRDF(child);

            // mode
            brdf.setMode(BRDF::Mode::BLINNPHONG);

            // push to brdfs vector
            this->brdfs.push_back(brdf);
            
            child = child->NextSiblingElement("OriginalBlinnPhong");
        }

        // ModifiedBlinnPhong
        child = element->FirstChildElement("ModifiedBlinnPhong");
        while(child)
        {
            BRDF brdf = parseBRDF(child);

            // mode
            brdf.setMode(BRDF::Mode::BLINNPHONG_MODIFIED);

            // push to brdfs vector
            this->brdfs.push_back(brdf);
            
            child = child->NextSiblingElement("ModifiedBlinnPhong");
        }
    }
exit(0);
    //
    // Materials
    //
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    
    while (element)
    {
        this->materials.push_back(parseMaterial(element));

        // read next material sibling
        element = element->NextSiblingElement("Material");
    }

    //
    // Textures
    //
    element = root->FirstChildElement("Textures");

    if(element)
        element = element->FirstChildElement("Texture");
    
    while(element)
    {
        textures.push_back(parseTexture(element));
        
        element = element->NextSiblingElement("Texture");
    }

    //
    // TexCoordData
    //

    element = root->FirstChildElement("TexCoordData");

    if(element)
    {
        stream << element->GetText() << std::endl;

        // read from stream and push to texCoordData vector
        int x, y;
        while (!(stream >> x).eof())
        {
            stream >> y;
            texCoordData.push_back(Vec2i(x, y)); // 0-index
        }
    }

    stream.clear();

    //
    // Transformations
    //
    element = root->FirstChildElement("Transformations");

    if(element)
    {
        // read scalings
        child = element->FirstChildElement("Scaling");

        while(child)
        {
            stream << child->GetText() << std::endl;
            
            float x, y, z;

            stream >> x >> y >> z;
            
            scalings.push_back(Scaling(x, y, z));
            
            child = child->NextSiblingElement("Scaling");
        }

        // read translations
        child = element->FirstChildElement("Translation");

        while(child)
        {
            stream << child->GetText() << std::endl;
        
            float x, y, z;

            stream >> x >> y >> z;
            
            translations.push_back(Translation(x, y, z));
            
            child = child->NextSiblingElement("Translation");
        }

        // read rotations
        child = element->FirstChildElement("Rotation");

        while(child)
        {
            stream << child->GetText() << std::endl;
        
            float angle, x, y, z;

            stream >> angle >> x >> y >> z;
            
            rotations.push_back(Rotation(angle, Vector3(x, y, z)));
            
            child = child->NextSiblingElement("Rotation");
        }
    }

    stream.clear();
    
    //
    // Vertex Data
    //
        // read to stream
    element = root->FirstChildElement("VertexData");
    if(element)
    {
        stream << element->GetText() << std::endl;
            // read from stream and push to vertex data vector
        Position3 vertex;
        while (!(stream >> vertex).eof())
        {
            vertexData.push_back(vertex);
        }

        stream.clear();
    }


    //
    // Mesh
    //
        // simply read meshes as triangles, so that the problem simplifies
    auto objects = root->FirstChildElement("Objects");
    element = objects->FirstChildElement("Mesh");
    while (element)
    {
        std::vector<Shape*> trianglesOfMesh;
        Texture* texture = nullptr;

        // flag for indicating transformation
        bool hasTransformation = false;
        bool hasMotionBlur = false;
        Vector3 motionBlur;

        int materialId;
        int meshId;
        ShadingMode shadingMode = DEFAULT_SHADING_MODE;
        Transformation transformation;
        
        // read mesh id
        element->QueryAttribute("id", &meshId);
        
        // read material id
        if(doesHaveChild(element, "Material"))
            materialId = parseChild<int>(element, "Material") - 1;

        // read shading mode
        const char * shadingModeString = element->Attribute("shadingMode");
        if(shadingModeString)
        {
            switch(*shadingModeString)
            {
                case 's':
                case 'S':
                    shadingMode = ShadingMode::SMOOTH;
                    break;
                case 'f':
                case 'F':
                    shadingMode = ShadingMode::FLAT;
                    break;
            }
        }
        
        // read transformations
        if(doesHaveChild(element, "Transformations"))
        {
            hasTransformation = true;
            transformation = parseObjectTransformation(element, translations, scalings, rotations);
        }

        // read motion blur
        if(doesHaveChild(element, "MotionBlur"))
        {
            hasMotionBlur = true;
            motionBlur = parseChild<Vector3>(element, "MotionBlur");
        }

        // read texture
        if(doesHaveChild(element, "Texture"))
        {
            int textureId = parseChild<int>(element, "Texture") - 1;

            texture = textures[textureId];
        }

        stream.clear();

        // read faces
        child = element->FirstChildElement("Faces");
        
        // check if to be read from ply
        const char * plyFileName = child->Attribute("plyFile");
        if(plyFileName)
        {
            Pair< std::vector<Vec3f>, std::vector<Vec3i> > plyMesh = parsePly(plyFileName);

            // copy position data as Vertex and copy meshVertexIndices
            std::vector<Vertex> vertexData;
            std::vector<Vec3i> meshVertexIndices(plyMesh.p2);

            for(int i = 0; i < plyMesh.p1.size(); i++)
            {
                vertexData.push_back(Vertex(plyMesh.p1[i].x, plyMesh.p1[i].y, plyMesh.p1[i].z));
            }

            // TODO: No support for ply with texture
            trianglesOfMesh = createMeshTriangles(
                vertexData,
                meshVertexIndices,
                shadingMode
            );

        }
        else
        {
            // no ply file is supplied - get mesh information from xml file
            child = element->FirstChildElement("Faces");
            stream << child->GetText() << std::endl;

            // get vertex offset
            int vertexOffset = 0;
            child->QueryAttribute("vertexOffset", &vertexOffset);

            // get texture offset
            int textureOffset = 0;
            child->QueryAttribute("textureOffset", &textureOffset);
            textureOffset -= vertexOffset;

            std::vector<Vec3i> meshVertexIndices;

            // read vertex ids from stream
            int v0_id, v1_id, v2_id;
            while (!(stream >> v0_id).eof())
            {
                stream >> v1_id >> v2_id;
                meshVertexIndices.push_back(
                    Vec3i(
                        v0_id + vertexOffset - 1,   // decrement by one for 0-based indexing
                        v1_id + vertexOffset - 1,
                        v2_id + vertexOffset - 1
                        )
                    );
            }
            stream.clear();

            trianglesOfMesh = createMeshTriangles(
                vertexData,
                meshVertexIndices,
                shadingMode,
                texture,
                texCoordData,
                textureOffset
            );

            // if has texture, make each triangle have its own material
            if(texture)
            {
                for(int i = 0; i < trianglesOfMesh.size(); i++)
                {
                    trianglesOfMesh[i]->setMaterial(materials[materialId]);
                }
            }
        }


        // from triangles of mesh, create a BVH
        Shape* meshBVH = BoundingVolume::createBoundingVolumeHiearchy(trianglesOfMesh);

        // set the material - if the material of each triangle is not set, which could happen
        // .. if they have texture
        if(!texture)
            meshBVH->setMaterial(materials[materialId]);

        // search if it has instances. if yes, create instances of it
        auto instanceElem = objects->FirstChildElement("MeshInstance");
        while(instanceElem)
        {
            int instanceId = -1;
            instanceElem->QueryAttribute("baseMeshId", &instanceId);
            if(instanceId == meshId)
            {
                int instanceMaterialId;
                Transformation instanceTransformation;
                bool instanceHasTransformation = false;

                bool instanceHasMotionBlur = false;
                Vector3 instanceMotionBlur;

                // found an instance of the mesh created above!

                // read material id
                if(doesHaveChild(instanceElem, "Material"))
                    instanceMaterialId = parseChild<int>(instanceElem, "Material") - 1;

                // transformation
                    // check reset transform
                const char* resetTransform = instanceElem->Attribute("resetTransform");
                if((!resetTransform || (resetTransform && resetTransform[0] == 'f')) && hasTransformation)
                {
                    // add the transformation of the instanced mesh
                    instanceHasTransformation = true;
                    instanceTransformation += transformation;
                }
                    // read transformations
                if(doesHaveChild(instanceElem, "Transformations"))
                {
                    instanceHasTransformation = true;
                    instanceTransformation += parseObjectTransformation(instanceElem, translations, scalings, rotations);
                }

                    // read motion blur
                if(doesHaveChild(instanceElem, "MotionBlur"))
                {
                    instanceHasMotionBlur = true;
                    instanceMotionBlur = parseChild<Vector3>(instanceElem, "MotionBlur");
                }

                // make instance
                Shape* instanceBVH = BoundingVolume::makeInstanceOf((BoundingVolume*)meshBVH);

                    // set

                // if it has transformation, apply
                if(instanceHasTransformation)
                {
                    instanceBVH->transform(instanceTransformation);
                }

                // if it has motion blur, apply
                if(instanceHasMotionBlur)
                {
                    instanceBVH->setMotionBlur(instanceMotionBlur);
                }

                // set the material
                instanceBVH->setMaterial(materials[instanceMaterialId]);

                shapes.push_back(instanceBVH);
            }

            instanceElem = instanceElem->NextSiblingElement("MeshInstance");
        }
        
        // if it has transformation, apply
        if(hasTransformation)
        {
            meshBVH->transform(transformation);
        }

        // if it has motion blur, apply
        if(hasMotionBlur)
        {
            meshBVH->setMotionBlur(motionBlur);
        }

        // push the BVH of shape to main vector
        shapes.push_back(meshBVH);

        element = element->NextSiblingElement("Mesh");
    }

    stream.clear();

    //
    // Triangles
    //
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");

    while (element)
    {
        int v0_id, v1_id, v2_id, materialId;
        
        // material id
        if(doesHaveChild(element, "Material"))
            materialId = parseChild<int>(element, "Material") - 1;

        // read vertex indices
        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> v0_id >> v1_id >> v2_id;

        // create triangle
        Triangle * triangle = new Triangle(
            materials[materialId],          // material
            Position3(vertexData[v0_id - 1]),   // v0
            Position3(vertexData[v1_id - 1]),   // v1
            Position3(vertexData[v2_id - 1])    // v2
        );

        // transformations
        if(doesHaveChild(element, "Transformations"))
        {
            Transformation transformation = parseObjectTransformation(element, translations, scalings, rotations);
            triangle->transform(transformation);
        }

        // motion blur
        if(doesHaveChild(element, "MotionBlur"))
        {
            Vector3 motionBlur = parseChild<Vector3>(element, "MotionBlur");
            triangle->setMotionBlur(motionBlur);
        }
        
        // push the triangle to the surfaces vector
        shapes.push_back((Shape*)(triangle));
                
        element = element->NextSiblingElement("Triangle");
    }

    stream.clear();

    //
    // Spheres
    //
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");

    while (element)
    {
        int centerVertexId, materialId;
        float radius;

        // material id
        if(doesHaveChild(element, "Material"))
            materialId = parseChild<int>(element, "Material") - 1;
        
        // center vertex id
        if(doesHaveChild(element, "Center"))
            centerVertexId = parseChild<int>(element, "Center") - 1;
        
        Position3 center = Position3(
                            vertexData[centerVertexId].getX(), 
                            vertexData[centerVertexId].getY(), 
                            vertexData[centerVertexId].getZ()
                            );

        // radius
        if(doesHaveChild(element, "Radius"))
            radius = parseChild<float>(element, "Radius");
        
        // create a new sphere
        Sphere * sphere = new Sphere(center, radius, materials[materialId]);

        // transformations
        if(doesHaveChild(element, "Transformations"))
        {
            Transformation transformation = parseObjectTransformation(element, translations, scalings, rotations);
            sphere->transform(transformation);
        }

        // motion blur
        if(doesHaveChild(element, "MotionBlur"))
        {
            Vector3 motionBlur = parseChild<Vector3>(element, "MotionBlur");
            sphere->setMotionBlur(motionBlur);
        }

        // texture
        if(doesHaveChild(element, "Texture"))
        {
            int textureId = parseChild<int>(element, "Texture") - 1;
            
            sphere->setTexture(textures[textureId]);
        }
        
        // push the sphere to the surfaces vector
        shapes.push_back((Shape*)(sphere));

        // read the next sphere sibling
        element = element->NextSiblingElement("Sphere");
    }
    
    // create bounding volume hiearchy
    this->BVH = BoundingVolume::createBoundingVolumeHiearchy(shapes);

    // clean textures
    for(int i = 0; i < textures.size(); i++)
        delete textures[i];
}

std::stringstream &operator>>(std::stringstream &st, Position3 & position)
{
    // read from stream
    float xyz[3];

    for(int i = 0; i < 3; i++)
    {
        if(!st.eof())
            st >> xyz[i];
    }

    // set position
    position.setX(xyz[0]);
    position.setY(xyz[1]);
    position.setZ(xyz[2]);

    return st;
}


std::stringstream &operator>>(std::stringstream &st, Vector3 & vec)
{
    // read from stream
    float xyz[3];

    for(int i = 0; i < 3; i++)
    {
        if(!st.eof())
            st >> xyz[i];
    }

    // set vector
    vec.setX(xyz[0]);
    vec.setY(xyz[1]);
    vec.setZ(xyz[2]);

    return st;
}
