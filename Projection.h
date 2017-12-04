#pragma once

#include "Point.h"

typedef struct
{
    int bot;
    int top;
}
Clamped;

typedef struct
{
    float bot;
    float top;
    Clamped clamped;
    float size;
    float height;
    int yres;
    float mid;
    float level;
}
Projection;

// Creates a line projection suited for the display based on the focal distance
// and corrected normal distance from the hero to the wall.
Projection xproject(const int yres, const float focal, const float yaw, const Point corrected, const float height);

Projection xstack(const Projection, const float shift);

Projection xdrop(const Projection, const float shift);

float xccast(const Projection, const int x);

float xfcast(const Projection, const int x);
