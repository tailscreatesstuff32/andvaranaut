#pragma once

#include "Point.h"

typedef struct
{
    Point* point;
    int count;
    int max;
}
Points;

Points xpsnew(const int max);

Points xpsadd(Points, const Point, const char* why);

Points xpspop(const Points, const int max);

Point xpsrand(const Points);