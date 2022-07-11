#include <stdlib.h>
#include <string.h>

#include "gear.h"

size_t
gearDefaultExpander(size_t capacity)
{
    size_t ret;

    if (capacity == 0) {
        return GEAR_DEFAULT_INIT_CAPACITY;
    }
    ret = capacity + GEAR_DEFAULT_EXPANSION;
    // Check for integer overflow
    return (ret < capacity) ? capacity : ret;
}

int
gearAppend(gear *array, const void *item)
{
    if (!array || !item) {
        return GEAR_RET_BAD_USAGE;
    }

    if (array->length == array->capacity) {
        void *success;
        size_t new_capacity;

        new_capacity = array->expander(array->capacity);
        if (new_capacity <= array->capacity) {
            return GEAR_RET_NO_EXPANSION;
        }

        success = realloc(array->data, array->item_size * new_capacity);
        if (!success) {
            return GEAR_RET_OUT_OF_MEMORY;
        }
        array->data = success;
        array->capacity = new_capacity;
    }

    memcpy((char *)array->data + array->item_size * array->length, item, array->item_size);
    array->length++;

    return GEAR_RET_OK;
}

void
gearFree(gear *array)
{
    if (!array) {
        return;
    }

    free(array->data);
    array->data = NULL;
    arracy->length = array->capacity = 0;
}
