#pragma once

#include <stddef.h>

typedef struct xarray xarray;

#define xarray_of(type) xarray

xarray *xarray_new(size_t element_size, size_t capacity);
#define xarray_new_of(type, capacity) xarray_new(sizeof(type), capacity)

size_t xarray_count(xarray *arr);
// return: actual remain count
size_t xarray_require_size(xarray *arr, size_t required_count);

void *xarray_get_at(xarray *arr, int index);
int xarray_index_of(xarray *arr, void *element);
void *xarray_get_first(xarray *arr);
void *xarray_get_last(xarray *arr);
void *xarray_get_next(xarray *arr, void *ptr);
void *xarray_get_prev(xarray *arr, void *ptr);

// -1: error, >=0: index
int xarray_add(xarray *arr, void *ptr);
void *xarray_add_new_item(xarray *arr);

void xarray_remove_at(xarray *arr, int index, void (*cb)(void *ptr));
void xarray_remove(xarray *arr, void *ptr, void (*cb)(void *ptr));

void xarray_clean(xarray *arr, void (*cb)(void *ptr));
void xarray_free(xarray *arr, void (*cb)(void *ptr));

#define xarray_foreach(type, var, arr) for (type var = xarray_get_first(arr); var; var = xarray_get_next(arr, var))
#define xarray_foreach_reverse(type, var, arr)                                                                         \
    for (type var = xarray_get_last(arr); var; var = xarray_get_prev(arr, var))

#define xarray_free_safe(arr, cb)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (arr)                                                                                                       \
        {                                                                                                              \
            xarray_free(arr, cb);                                                                                      \
            arr = NULL;                                                                                                \
        }                                                                                                              \
    } while (0)
