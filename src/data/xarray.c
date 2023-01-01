#include "global.h"

#define XAL_HAS_VALUE 1

struct xarray
{
    /* 
    element1 element2 ... elementN ... Capacity
     */
    void *buffer;
    size_t count;
    size_t capacity;
    size_t element_size;
};

xarray *xarray_new(size_t element_size, size_t capacity)
{
    xarray *arr = calloc(1, sizeof(xarray));
    if (arr == NULL)
    {
        return NULL;
    }
    arr->buffer = calloc(capacity, element_size);
    if (arr->buffer == NULL)
    {
        free(arr);
        return NULL;
    }
    arr->count        = 0;
    arr->capacity     = capacity;
    arr->element_size = element_size;
    return arr;
}

size_t xarray_count(xarray *arr)
{
    return arr->count;
}
size_t xarray_require_size(xarray *arr, size_t required_count)
{
    if (arr->count + required_count > arr->capacity)
    {
        size_t new_capacity = arr->capacity + required_count + 4;
        void *new_buffer    = realloc(arr->buffer, new_capacity * arr->element_size);
        if (!new_buffer)
        {
            LOG_DBG("realloc failed");
            return 0;
        }
        arr->buffer   = new_buffer;
        arr->capacity = new_capacity;
    }
    return arr->capacity - arr->count;
}

void *xarray_get_at(xarray *arr, int index)
{
    if (index < 0 || index >= arr->count)
    {
        return NULL;
    }
    return (char *)arr->buffer + index * arr->element_size;
}
int xarray_index_of(xarray *arr, void *element)
{
    if (element < arr->buffer || (char *)element >= (char *)arr->buffer + arr->count * arr->element_size)
    {
        return -1;
    }
    return ((char *)element - (char *)arr->buffer) / arr->element_size;
}
void *xarray_get_first(xarray *arr)
{
    return xarray_get_at(arr, 0);
}
void *xarray_get_last(xarray *arr)
{
    return xarray_get_at(arr, arr->count - 1);
}
void *xarray_get_next(xarray *arr, void *ptr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    int index = xarray_index_of(arr, ptr);
    return index >= 0 ? xarray_get_at(arr, index + 1) : NULL;
}
void *xarray_get_prev(xarray *arr, void *ptr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    int index = xarray_index_of(arr, ptr);
    return index >= 0 ? xarray_get_at(arr, index - 1) : NULL;
}

int xarray_add(xarray *arr, void *ptr)
{
    if (xarray_require_size(arr, 1) < 1)
    {
        return -1;
    }
    void *target = (char *)arr->buffer + arr->count * arr->element_size;
    memcpy(target, ptr, arr->element_size);
    arr->count++;
    return arr->count - 1;
}
void *xarray_add_new_item(xarray *arr)
{
    if (xarray_require_size(arr, 1) < 1)
    {
        return NULL;
    }
    void *target = (char *)arr->buffer + arr->count * arr->element_size;
    arr->count++;
    return target;
}

void xarray_remove_at(xarray *arr, int index, void (*cb)(void *ptr))
{
    if (index < 0 || index >= arr->count)
    {
        return;
    }
    void *target = (char *)arr->buffer + index * arr->element_size;
    if (cb)
    {
        cb(target);
    }
    memset(target, 0, arr->element_size);
    if (index < arr->count - 1)
    {
        memmove(target, (char *)target + arr->element_size, (arr->count - index - 1) * arr->element_size);
    }
    arr->count--;
}
void xarray_remove(xarray *arr, void *ptr, void (*cb)(void *ptr))
{
    int index = xarray_index_of(arr, ptr);
    if (index < 0)
    {
        return;
    }
    xarray_remove_at(arr, index, cb);
}

void xarray_clean(xarray *arr, void (*cb)(void *ptr))
{
    if (cb)
    {
        for (int i = 0; i < arr->count; i++)
        {
            cb((char *)arr->buffer + i * arr->element_size);
        }
    }
    arr->count = 0;
}
void xarray_free(xarray *arr, void (*cb)(void *ptr))
{
    if (cb)
    {
        for (int i = 0; i < arr->count; i++)
        {
            cb((char *)arr->buffer + i * arr->element_size);
        }
    }
    free(arr->buffer);
    free(arr);
}
