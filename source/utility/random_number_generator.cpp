#include "random_number_generator.hpp"
#include <random>

float getRandomBtw01()
{
    thread_local static std::random_device rd;

    return rd() / (float)rd.max();
}

// random number in interval [-0.5, 0.5]
float getRandom0_5()
{
    return getRandomBtw01() - 0.5f;
}

// get random [0, i)
int getRand(int i)
{
    int val = (int)(getRandomBtw01() * RAND_MAX) % i;

    return val;
}