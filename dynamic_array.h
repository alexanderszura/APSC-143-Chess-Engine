#ifndef APSC143__DYNAMIC_ARRAY_H
#define APSC143__DYNAMIC_ARRAY_H

#include "stdbool.h"

enum types 
{
    UINT,
    ULONG,
};

struct dynamic_array 
{
    void **values;
    unsigned int current_index;
};

bool uint_equals (const void *a, const void *b);
bool ulong_equals(const void *a, const void *b);

unsigned int  *as_uint (const struct dynamic_array *arr);
unsigned long *as_ulong(const struct dynamic_array *arr);

struct dynamic_array init_array();
bool append_dynamic(      struct dynamic_array *arr, void *value);
bool search_dynamic(const struct dynamic_array *arr, void *value, enum types type);

#endif