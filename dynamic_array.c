#include "stdbool.h"
#include "board.h"
#include "stdio.h"
#include "stdlib.h"
#include "dynamic_array.h"

bool append_dynamic(struct dynamic_array *arr, void *value)
{
    if (arr->values == NULL)
    {
        arr->values = malloc(sizeof(void *));
        arr->current_index = 0;

        if (arr->values == NULL) 
        {
            printf("Error mallocating memory with value: %p\n", value);
            return false;
        }
    } else {
        void **temp = realloc(arr->values, (arr->current_index + 1) * sizeof(void *));

        if (temp == NULL) 
        {
            printf("Error reallocating memory with value: %p at index: %u\n", value, arr->current_index);
            return false;
        }

        arr->values = temp;
    }

    arr->values[arr->current_index++] = value;

    return true;
}

bool uint_equals(const void *a, const void *b)
{
    return *(unsigned int *)a == *(unsigned int *)b;
}

bool ulong_equals(const void *a, const void *b)
{
    return *(unsigned long *)a == *(unsigned long *)b;
}

unsigned int *as_uint(const struct dynamic_array *arr)
{
    if (arr == NULL or arr->values == NULL)
        return NULL;

    unsigned int *result = malloc(arr->current_index * sizeof(unsigned int));
    if (not result) return NULL;

    for (unsigned int i = 0; i < arr->current_index; i++)
        result[i] = *(unsigned int *)arr->values[i];

    return result;
}

unsigned long *as_ulong(const struct dynamic_array *arr)
{
    if (arr == NULL or arr->values == NULL)
        return NULL;

    unsigned long *result = malloc(arr->current_index * sizeof(unsigned long));
    if (not result) return NULL;

    for (unsigned int i = 0; i < arr->current_index; i++)
        result[i] = *(unsigned long *)arr->values[i];

    return result;
}

bool search_dynamic(const struct dynamic_array *arr, void *value, enum types type)
{
    bool (*equals) (const void *, const void *);
    switch (type)
    {
        case UINT:
            equals = uint_equals;
            break;

        case ULONG:
            equals = ulong_equals;
            break;
        
        default:
            equals = NULL;
    }

    if (equals == NULL) 
    {
        printf("Couldn't find a equals func for type %d", type);
        return false;
    }

    for (unsigned int i = 0; i < arr->current_index; i++)
        if (equals(arr->values[i], value))
            return true;

    return false;
}