#pragma once

#include "Line.h"
#include "Gpu.h"
#include "Map.h"
#include "Portals.h"

#include <stdbool.h>

typedef struct
{
    bool inside;
    Line fov;
    Point where;
    Point velocity;
    float speed;
    float acceleration;
    float theta;
}
Hero;

Hero spawn();
Hero spin(const Hero hero);
Hero move(const Hero hero, char** const walling);
int handle(const Hero hero, char** const walling); // Returns a portal handle if near
void render(const Hero hero, const Blocks blocks, const int res, const Gpu gpu);
Hero teleport(const Hero hero, const Portal portal, const Map map);