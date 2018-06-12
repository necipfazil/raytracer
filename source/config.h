#ifndef __CONFIG_H__
#define __CONFIG_H__

//--------------------------------------------------------------------------//
// enable-disable features
//--------------------------------------------------------------------------//

// There are two options for allowing threads to obtain their tasks:
// 1) Fill a ConcurrentBag with pixel values, send the bag to the threads
//    and let them obtain their tasks by popping elements from the bag
// 2) Create an instance PixelMissionGenerator of class, which does nothing
//    but traces the next pixel to fill. Send the generator to the threads
//    and let them obtain their tasks by querying the next pixel to fill.
// Mainly, the difference is comes from the trade off between performance
// and memory usage. While first option uses 70MB of memory space for a
// 800x800 image and this usage scales linearly by the image resolution, 
// second one does occupy negligible memory space but needs ~.5sec more for
// a 800x800 image, which again scales linearly by the image resolution.
// To use first option, define __CONCURRENT_BAG_TASK_DIST__. Otherwise,
// second option is used.
//#define __CONCURRENT_BAG_TASK_DIST__

// There are to options while creating BoundingVolumeHiearchy. First one is
// to partition array of shapes into two by making use of the geometric center
// of all the shapes. The second option is to partition the array into half.
// To use first option, define __BVH_DIVISION_BY_GEOMETRIC_CENTER__. Otherwise,
// second option is used.
#define __BVH_DIVISION_BY_GEOMETRIC_CENTER__

//--------------------------------------------------------------------------//
// configurable variables
//--------------------------------------------------------------------------//
#define NUM_OF_THREADS 4
#define AIR_REFRACTION_INDEX 1.f
#define RAY_TRANSLATION_EPSILON 0.001f
#define DEFAULT_SHADOW_RAY_EPSILON "0.001"
#define DEFAULT_BACKGROUND_COLOR "0 0 0"
#define DEFAULT_MAXRECURSIONDEPTH "1"
#define DEFAULT_SHADING_MODE ShadingMode::FLAT
#define BACKFACE_CULLING
#define SEEDED_RANDOMIZATION

#endif
