#ifndef GENERAL_ARRAY_H
#define GENERAL_ARRAY_H

#include <sys/types.h>

#include "config.h"

#define GEAR_VERSION "0.0.1"

typedef struct gear {
    void *data;
    size_t (*expander)(size_t);
    size_t item_size;
    size_t length;
    size_t capacity;
} gear;

enum gearRetValue {
    GEAR_RET_OK,
    GEAR_RET_BAD_USAGE,
    GEAR_RET_OUT_OF_MEMORY,
    GEAR_RET_NO_EXPANSION,
};

size_t
gearDefaultExpander(size_t capacity);

#define GEAR_INIT(type)                               \
    (gear)                                            \
    {                                                 \
        NULL, gearDefaultExpander, sizeof(type), 0, 0 \
    }

int
gearAppend(gear *array, const void *item);

void
gearFree(gear *array);

#define GEAR_GET_ITEM(array, idx) ((void *)((char *)(array)->data + (idx) * (array)->item_size))

#define GEAR_FOR_EACH(array, item)                                            \
    for (item = (array)->data; item != GEAR_GET_ITEM(array, (array)->length); \
         item = (void *)((char *)item + (array)->item_size))

#endif  // GENERAL_ARRAY_H
