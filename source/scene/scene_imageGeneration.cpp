#include "../config.h"
#include "../scene.hpp"
#include "../geometry/headers/geometry.hpp"
#include "../image/image.hpp"
#include "../image/color.hpp"
#include <thread>
#include <iostream>
#include <chrono>


#ifdef __CONCURRENT_BAG_TASK_DIST__

void Scene::imageFiller(Camera * camera, Image * image, Scene * scene, ConcurrentBag<Vec2i> * missionsBag)
{
    if(camera != NULL && image != NULL && scene != NULL && missionsBag != NULL)
    {

        Vec2i pixelToFill;
        
        while(missionsBag->pop(pixelToFill))
        {
            const std::vector<Ray> & raysToSample = camera->getRays(pixelToFill.x, pixelToFill.y);

            Color rayColor = Color::Black();
            float sumOfWeights = 0.f;

            for(int i = 0; i < (int)raysToSample.size(); i++)
            {
                // get sample from ray
                rayColor += scene->getRayColor(raysToSample[i], scene->maxRecursionDepth);

                // add the weight of the ray to sum of weights
                sumOfWeights += raysToSample[i].getWeight();
            }

            // normalize color
            rayColor = rayColor / sumOfWeights;

            // set color
            image->setColor(pixelToFill.x, pixelToFill.y, rayColor);
        }
    }
    else
    {
        throw "Scene::imageGeneratorThread(): NULL object!";
    }
}

#else

void Scene::imageFiller(Camera * camera, Image * image, Scene * scene, PixelMissionGenerator * pixelMissionGenerator)
{
    if(camera != NULL && image != NULL && scene != NULL && pixelMissionGenerator != NULL)
    {
        Vec2i pixelToFill;

        while(pixelMissionGenerator->getPixelToFill(pixelToFill))
        {
            // Debugging block
            /*int x = pixelToFill.x, y = pixelToFill.y;

            if(!(x < 75 && y < 75))
            {
                image->setColor(pixelToFill.x, pixelToFill.y, Color::Black());
                continue;
            }*/

            const std::vector<Ray> raysToSample = camera->getRays(pixelToFill.x, pixelToFill.y);

            Color rayColor = Color::Black();
            float sumOfWeights = 0.f;

            for(int i = 0; i < (int)raysToSample.size(); i++)
            {
                // get ray weight
                float rayWeight = raysToSample[i].getWeight();

                // get sample from ray by intensfying it by ray weight
                    // backfaceCulling is applied to primary rays if defined
                bool backfaceCulling = false;
                #ifdef BACKFACE_CULLING
                backfaceCulling = true;
                #endif
                rayColor += scene->getRayColor(raysToSample[i], scene->maxRecursionDepth, backfaceCulling).intensify(rayWeight);

                // add the weight of the ray to sum of weights
                sumOfWeights += rayWeight;
            }

            // normalize color
            rayColor = rayColor / sumOfWeights;

            // if needed, apply gamma correction
            // TODO

            // set color
            image->setColor(pixelToFill.x, pixelToFill.y, rayColor);
        }
    }
    else
    {
        throw "Scene::imageGeneratorThread(): NULL object!";
    }
}

#endif

void dumpInfoUntilCompletion(
    PixelMissionGenerator& missionDist, 
    const int numberOfDashes = 60, 
    const char completeDash = '|',
    const char incompleteDash = '-'
)
{
    float perc = 0.f;
    int numberOfCompletedDashes = 0;
    int numberOfIncompleteDashes = numberOfDashes;
    bool done = false;

    auto start_time = std::chrono::high_resolution_clock::now();

    // newline
    std::cout << std::endl;
    
    // while tracing is not completed
    while(true)
    {
        done = missionDist.getFilledPerc(perc);

        std::cout << "\r[";

        numberOfCompletedDashes  = numberOfDashes * perc;
        numberOfIncompleteDashes = numberOfDashes - numberOfCompletedDashes;

        for(int i = 0; i < numberOfCompletedDashes; i++)
            std::cout << completeDash;

        for(int i = 0; i < numberOfIncompleteDashes; i++)
            std::cout << incompleteDash;

        std::cout << "] ";

        // write percetange
        printf("%3.2f", perc * 100);
        std::cout << "%";

        // write time
        auto current_time = std::chrono::high_resolution_clock::now();
        long sec = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        printf(" - %3ldm %2lds", sec / 60, sec % 60);

        if(done) return;
    }
}

void Scene::generateImages(unsigned short numberOfThreads)
{
    // generate one image for each camera
    for(int i = 0; i < this->cameras.size(); i++)
    {
        Camera & camera = this->cameras[i];
        
        int imageWidth = camera.getImageWidth();
        int imageHeight = camera.getImageHeight();
        
        // create the image object
        Image image(imageWidth, imageHeight);

#ifdef __CONCURRENT_BAG_TASK_DIST__
        // create the missions bag, which will include a mission for each pixel
        std::forward_list<Vec2i> missionsBag;
        for(int x = 0; x < imageWidth; x++)
        {
            for(int y = 0; y < imageHeight; y++)
            {
                Vec2i pixel { x, y };
                missionsBag.push_front(pixel);
            }
        }

        // create concurrent missions bag
        ConcurrentBag<Vec2i> missionDist(missionsBag);
#else
        PixelMissionGenerator missionDist(imageWidth, imageHeight);
#endif
        // check the number of threads
            // if 0, do not create an extra thread but use the current
        if(numberOfThreads == 0)
        {
            imageFiller(
                &camera,
                &image,
                this,
                &missionDist
            );
        }
        else
        {
            // create threads
            std::vector<std::thread> threads;
            
            for(int i = 0; i < numberOfThreads; i++)
            {
                threads.push_back(
                    std::thread(
                        imageFiller,
                            &camera,
                            &image,
                            this,
                            &missionDist
                    )
                );
            }

            dumpInfoUntilCompletion(missionDist);

            std::cout << std::endl;

            // join threads (wait for all to finish their job)
            for(int i = 0; i < (int)threads.size(); i++)
            {
                threads[i].join();
            }
        }

        // apply gamma correction
        //image.applyGammaCorrection(camera.getGammaCorrection());

        image.write(camera.getImageName());

        // check HDR
        if(camera.doTonemap())
        {
            // change image name
            std::string imageName = camera.getImageName();
            int extensionInd = imageName.find('.');
            int strLen = imageName.length();
            imageName = imageName.substr(0, extensionInd) + ".png";

            // tonemap&write image
            ToneMappingParam toneMappingParam = camera.getToneMappingParam();
            toneMappingParam.gamma = camera.getGammaCorrection();

            image.write(imageName, toneMappingParam);
        }
    }
    
}
