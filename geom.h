#pragma once

#include <stdbool.h>

struct point
{
    double x, y;
};

double
geom_mag(const struct point point);

double
geom_wallpercentage(const struct point point);

struct point
geom_cast(const struct point hero, const double radians);

bool
geom_wallcollision(const struct point point);

int
geom_wallingtile(const struct point point);

int
geom_flooringtile(const struct point point);

int
geom_ceilingtile(const struct point point);

bool
geom_outofbounds(const struct point point);

// Performance inlines
static inline struct point
geom_sub(const struct point i, const struct point j)
{
    return (struct point){ i.x - j.x, i.y - j.y };
}

static inline struct point
geom_add(const struct point i, const struct point j)
{
    return (struct point){ i.x + j.x, i.y + j.y };
}

static inline struct point
geom_mul(const struct point i, const double n)
{
    return (struct point){ i.x * n, i.y * n };
}

static inline double
geom_mod(const double d)
{
    return d - (int)d;
}