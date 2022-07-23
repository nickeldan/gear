#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <gear/gear.h>

int
gearAppend(gear *array, const void *item)
{
    if (!array || !item) {
        return GEAR_RET_BAD_USAGE;
    }

    if (array->length == array->_capacity) {
        void *success;
        size_t new_capacity;

        if (array->_use_expander) {
            new_capacity = array->_expander(array->_capacity);
        }
        else if (array->_capacity == 0) {
            new_capacity = array->_init_capacity;
        }
        else {
            new_capacity = array->_capacity + array->_expansion;
        }

        if (new_capacity <= array->_capacity) {
            return GEAR_RET_NO_EXPANSION;
        }

        success = realloc(array->_data, array->item_size * new_capacity);
        if (!success) {
            return GEAR_RET_OUT_OF_MEMORY;
        }
        array->_data = success;
        array->_capacity = new_capacity;
    }

    memcpy((char *)array->_data + array->item_size * array->length, item, array->item_size);
    array->length++;

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
        size_t init_capacity, expansion;

        init_capacity = va_arg(args, size_t);
        expansion = va_arg(args, size_t);
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
