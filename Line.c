#include "Line.h"

Line rotate(const Line line, const float t)
{
    return (Line) { trn(line.a, t), trn(line.b, t) };
}

Point lerp(const Line line, const float n)
{
    return add(line.a, mul(sub(line.b, line.a), n));
}

float focal(const Line line)
{
    return line.a.x / (line.b.y - line.a.y);
}

float ccast(const Line fov, const int res, const int x)
{
    return focal(fov) * res / (2 * x - (res - 1));
}

float fcast(const Line fov, const int res, const int x)
{
    return -ccast(fov, res, x);
}