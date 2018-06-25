#include "config.h"
#include "scene.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    #ifdef SEEDED_RANDOMIZATION
    srand(1);
    #endif

    Scene scene;

    scene.loadFromXml(argv[1]);
    scene.generateImages(NUM_OF_THREADS);
   
    return 0;
}
