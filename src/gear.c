#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <gear/config.h>
#include <gear/gear.h>

static size_t
useExpander(gearExpander expander, size_t capacity, size_t new_length)
{
    do {
        size_t new_capacity;

        new_capacity = expander(capacity);
        if (new_capacity <= capacity) {
            return 0;
        }
        capacity = new_capacity;
    } while (capacity < new_length);

    return capacity;
}

static size_t
computeNewCapacity(const gear *array, size_t new_length)
{
    size_t capacity, missing;

    if (array->_use_expander) {
        return useExpander(array->_expander, array->_capacity, new_length);
    }

    capacity = array->_capacity;
    if (capacity == 0) {
        capacity = array->_init_capacity;
        if (capacity >= new_length) {
            return capacity;
        }
    }
    missing = new_length - capacity + array->_expansion - 1;
    missing -= missing % array->_expansion;
    capacity += missing;

    return capacity;
}

static int
increaseCapacity(gear *array, size_t more)
{
    size_t new_length = array->length + more, new_capacity;
    void *success;

    if (new_length <= array->_capacity) {
        return GEAR_RET_OK;
    }

    new_capacity = computeNewCapacity(array, new_length);
    if (new_capacity < new_length) {
        return GEAR_RET_NO_EXPANSION;
    }

    success = realloc(array->_data, array->item_size * new_capacity);
    if (!success) {
        return GEAR_RET_OUT_OF_MEMORY;
    }
    array->_data = success;
    array->_capacity = new_capacity;

    return GEAR_RET_OK;
}

void
gearInit(gear *array, size_t item_size)
{
    if (!array || item_size == 0) {
        return;
    }

    *array = (gear){0};
    array->item_size = item_size;
    array->_init_capacity = GEAR_DEFAULT_INIT_CAPACITY;
    array->_expansion = GEAR_DEFAULT_EXPANSION;
}

int
gearLoad(gear *array, const void *src, size_t num_items)
{
    int ret;

    if (!array || !src) {
        return GEAR_RET_BAD_USAGE;
    }

    ret = increaseCapacity(array, num_items);
    if (ret != GEAR_RET_OK) {
        return ret;
    }

    memcpy(GEAR_GET_ITEM(array, array->length), src, array->item_size * num_items);
    array->length += num_items;

    return GEAR_RET_OK;
}

int
gearAppend(gear *array, const void *item)
{
    return gearLoad(array, item, 1);
}

int
gearPop(gear *array, size_t idx, void *item)
{
    void *ptr;

    if (!array || idx >= array->length) {
        return GEAR_RET_BAD_USAGE;
    }

    ptr = GEAR_GET_ITEM(array, idx);
    if (item) {
        memcpy(item, ptr, array->item_size);
    }
    memmove(ptr, GEAR_GET_ITEM(array, idx + 1), array->item_size * (array->length - idx - 1));
    array->length--;

    return GEAR_RET_OK;
}

int
gearConcatenate(gear *dst, const gear *src)
{
    if (!dst || !src || dst->item_size != src->item_size) {
        return GEAR_RET_BAD_USAGE;
    }

    return gearLoad(dst, src->_data, src->length);
}

void
gearReset(gear *array)
{
    if (!array) {
        return;
    }

    free(array->_data);
    array->_data = NULL;
    array->length = array->_capacity = 0;
}

int
gearSetExpansion(gear *array, unsigned int init_capacity, unsigned int expansion)
{
    if (!array || init_capacity == 0 || expansion == 0) {
        return GEAR_RET_BAD_USAGE;
    }

    array->_init_capacity = init_capacity;
    array->_expansion = expansion;
    array->_use_expander = 0;

    return GEAR_RET_OK;
}

int
gearSetExpander(gear *array, gearExpander expander)
{
    if (!array || !expander) {
        return GEAR_RET_BAD_USAGE;
    }

    array->_expander = expander;
    array->_use_expander = 1;

    return GEAR_RET_OK;
}
