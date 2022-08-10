#ifndef GENERAL_ARRAY_H
#define GENERAL_ARRAY_H

#include <sys/types.h>

#include "config.h"

#define GEAR_VERSION "0.3.0"

enum gearRetValue {
    GEAR_RET_OK = 0,
    GEAR_RET_BAD_USAGE,
    GEAR_RET_OUT_OF_MEMORY,
    GEAR_RET_NO_EXPANSION,
};

typedef enum gearExpansionMethod {
    GEAR_EXP_CONSTANTS,
    GEAR_EXP_FUNCTION,
} gearExpansionMethod;

typedef size_t (*gearExpander)(size_t);

typedef struct gear {
    union {
        gearExpander _expander;
        struct {
            unsigned int _init_capacity;
            unsigned int _expansion;
        };
    };
    void *_data;
    size_t item_size;
    size_t length;
    size_t _capacity;
    unsigned int _use_expander : 1;
} gear;

#define GEAR_INIT(type)                                                                     \
    (gear)                                                                                  \
    {                                                                                       \
        ._init_capacity = GEAR_DEFAULT_INIT_CAPACITY, ._expansion = GEAR_DEFAULT_EXPANSION, \
        .item_size = sizeof(type)                                                           \
    }

int
gearAppend(gear *array, const void *item);

int
gearConcatenate(gear *dst, const gear *src);

void
gearFree(gear *array);

int
gearSetExpansion(gear *array, gearExpansionMethod method, ...);

#define GEAR_GET_ITEM(array, idx) ((void *)((char *)(array)->_data + (idx) * (array)->item_size))

#define GEAR_FOR_EACH(array, item)                                             \
    for (item = (array)->_data; item != GEAR_GET_ITEM(array, (array)->length); \
         item = (void *)((char *)item + (array)->item_size))

#define GEAR_FOR_EACH_WITH_INDEX(array, item, idx)              \
    for (item = (array)->_data, idx = 0; idx < (array)->length; \
         item = (void *)((char *)item + (array)->item_size), idx++)

#endif  // GENERAL_ARRAY_H
