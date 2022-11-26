#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <gear/config.h>
#include <gear/gear.h>

static size_t
computeNewCapacity(const gear *array, size_t new_length)
{
    size_t capacity = array->_capacity;
    bool use_expander = array->_use_expander;

    if (capacity == 0 && !use_expander) {
        capacity = array->_init_capacity;
    }

    while (capacity < new_length) {
        size_t new_capacity;

        if (use_expander) {
            new_capacity = array->_expander(capacity);
        }
        else {
            new_capacity = capacity + array->_expansion;
        }

        if (new_capacity < capacity) {
            return 0;
        }
        capacity = new_capacity;
    }

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
    if (new_capacity == 0) {
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
gearAppend(gear *array, const void *item)
{
    int ret;

    if (!array || !item) {
        return GEAR_RET_BAD_USAGE;
    }

    ret = increaseCapacity(array, 1);
    if (ret != GEAR_RET_OK) {
        return ret;
    }

    memcpy(GEAR_GET_ITEM(array, array->length), item, array->item_size);
    array->length++;

    return GEAR_RET_OK;
}

int
gearConcatenate(gear *dst, const gear *src)
{
    int ret;
    size_t item_size;

    if (!dst || !src || dst->item_size != src->item_size) {
        return GEAR_RET_BAD_USAGE;
    }
    item_size = dst->item_size;

    ret = increaseCapacity(dst, src->length);
    if (ret != GEAR_RET_OK) {
        return ret;
    }

    memcpy(GEAR_GET_ITEM(dst, dst->length), src->_data, item_size * src->length);
    dst->length += src->length;

    return GEAR_RET_OK;
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
