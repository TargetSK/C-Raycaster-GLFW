#include <math.h>

#include "include/idk.h"

float dist(float ax,float ay, float bx, float by, float ang){
    return ( sqrt((bx - ax) * (bx - ax) + (by-ay) * (by - ay)) );
}

