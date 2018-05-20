#include "config.h"
#include "scene.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    Scene scene;

    scene.loadFromXml(argv[1]);
    scene.generateImages(NUM_OF_THREADS);
   
    return 0;
}
