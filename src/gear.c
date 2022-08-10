#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
    size_t new_length = array->item_size + more, new_capacity;
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
gearFree(gear *array)
{
    if (!array) {
        return;
    }

    free(array->_data);
    array->_data = NULL;
    array->length = array->_capacity = 0;
}

int
gearSetExpansion(gear *array, gearExpansionMethod method, ...)
{
    int ret = GEAR_RET_BAD_USAGE;
    va_list args;

    if (!array) {
        return GEAR_RET_BAD_USAGE;
    }

    va_start(args, method);

    if (method == GEAR_EXP_CONSTANTS) {
        unsigned int init_capacity, expansion;

        init_capacity = va_arg(args, unsigned int);
        expansion = va_arg(args, unsigned int);
        if (init_capacity == 0 || expansion == 0) {
            goto done;
        }

        array->_init_capacity = init_capacity;
        array->_expansion = expansion;
        array->_use_expander = 0;
    }
    else if (method == GEAR_EXP_FUNCTION) {
        gearExpander expander;

        expander = va_arg(args, gearExpander);
        if (!expander) {
            goto done;
        }
        array->_expander = expander;
        array->_use_expander = 1;
    }
    else {
        goto done;
    }

    ret = GEAR_RET_OK;

done:
    va_end(args);
    return ret;
}
