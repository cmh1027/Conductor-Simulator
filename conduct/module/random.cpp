#include <cstdlib>
#include <ctime>
#include "random.h"

const int Random::MAXIMUM = 10000;
bool Random::setSeed = false;

bool Random::percent(double prob){
    if(!Random::setSeed){
        srand(static_cast<unsigned int>(time(nullptr)));
        Random::setSeed = true;
    }
    int randNum = rand() % MAXIMUM;
    if(0 <= randNum && randNum < MAXIMUM * prob)
        return true;
    else
        return false;
}

int Random::pick(int minimum, int maximum){
    if(!Random::setSeed){
        srand(static_cast<unsigned int>(time(nullptr)));
        Random::setSeed = true;
    }
    if(minimum >= maximum)
        return minimum;
    int increment = rand() % (maximum - minimum + 1);
    return minimum + increment;
}
