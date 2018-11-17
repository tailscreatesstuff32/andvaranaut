#pragma once

#include "Classification.h"

typedef struct
{
    int index;
    Classification clas;
    const char* cstr;
    const char* name;
    const char* desc;
    float damage;
    int hurts;
    float amplitude;
    float period;
}
Item;

Item i_new(const Classification, const int index);

Item i_rand(void);

void i_test(void);
