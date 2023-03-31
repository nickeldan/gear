The GEAR library provides self-expanding arrays of arbitrary types.

API
===

You can access GEAR's functionality by

```c
#include <gear/gear.h>
```

In order to create an array of, say, integers, do

```c
gear array;

gearInit(&array, sizeof(int));
```

A new object can be appended to the array by

```c
int x = 5;

if ( gearAppend(&array, &x) != GEAR_RET_OK ) {
    // handle the error
}
```

The possible return values of `gearAppend` are defined in [gear/gear.h](include/gear/gear.h).

Two fields of the `gear` structure are available to users, `length` and `item_size`.  Both should be self-explanatory.  It is important to note that no fields of the structure should be changed manually. Instead, all operations should be performed via the API.  The one exception is the `length` field which can only be decreased.

You can retrieve pointers to the elements of the array by

```c
int *p;

p = GEAR_GET_ITEM(&array, some_index);
```

Note that the elements of the array are stored contiguously in memory.

You can also iterate over the elements of the array via

```c
int *p;

GEAR_FOR_EACH(&array, p) {
    printf("%i\n", *p);
}
```

If you want to iterate with an index, do

```c
int *p;
size_t idx;

GEAR_FOR_EACH_WITH_INDEX(&array, p, idx) {
    printf("%zu: %i\n", idx, *p);
}
```

An array can have its data freed and its length set to zero by

```c
gearReset(&array);
```

Note that `array.item_size` will be unchanged.

You can copy data from memory to the end of an array via

```c
int
gearLoad(gear *array, const void *src, size_t num_items);
```

You may remove an element from an array via

```c
int
gearPop(gear *array, size_t idx, void *item);
```

For example,

```c
gear array;
int nums[] = {1, 2, 3, 4, 5, 6};

gearInit(&array, sizeof(int));
gearLoad(&array, nums, 6);
gearPop(&array, 3, NULL);
// The array now holds 1, 2, 3, 5, 6.
```

If `item` is not `NULL`, then the removed item will copied to that address.

You can also concatenate two arrays as long as they have the same item size via

```c
int
gearConcatenate(gear *dst, const gear *src);
```

Expansion
---------

At any moment, an array has a capacity which is at least as big as its length.  The capacity indicates how many elements the array can currently hold without needing to allocate more memory.  There are two methods from which you can select to determine how the capacity is expanded when needed.

The first method, which is set by default when an array is initialized with `gearInit`, uses two `unsigned int` values, both of which must be positive.  The first specifies an initial capacity.  That is, it specifies what the capacity will be set to when the first item is appended to the array.  The second constant specifies by how much the capacity will be increased when it is filled up.  The default values of these constants are defined in [gear/config.h](include/gear/config.h).

The second method uses a user-provided function pointer.  Specifically,

```c
typedef size_t gearExpander(size_t);
```

When determining the new capacity, `gearAppend` will call this function with the current capacity as the argument.  The function can either return the new capacity or return a value less than or equal to the current.  If the function returns the latter, that will tell `gearAppend` that the array cannot be expanded further and `gearAppend` will return `GEAR_RET_NO_EXPANSION` to the caller.  You can use this as a method of imposing a limit on the array's length.

The expansion method and associated parameter(s) can be set by the functions

```c
int
gearSetExpansion(gear *array, unsigned int init_capacity, unsigned int expansion);
```

and

```c
int
gearSetExpander(gear *array, gearExpander expander);
```

These function return `GEAR_RET_OK` if successful and `GEAR_RET_BAD_USAGE` if the arguments are invalid.

`gearReset` preserves the changes made by these two functions.

Building GEAR
=============

Shared and static libraries are built using make.  Adding `debug=yes` to the make invocation will disable optimization and build the libraries with debugging symbols.

You can also include GEAR in a larger project by including [make.mk](make.mk).  Before doing so, however, the `GEAR_DIR` variable must be set to the location of the GEAR directory.  You can also tell make where to place the shared and static libraries by defining the `GEAR_LIB_DIR` variable (defaults to `$(GEAR_DIR)`). Similarly, you can define the `GEAR_OBJ_DIR` variable which tells make where to place the object files (defaults to `$(GEAR_DIR)/src`).

make.mk adds a target to the `CLEAN_TARGETS` variable.  This is so that implementing

```make
clean: $(CLEAN_TARGETS)
    ...
```

in your project's Makefile will cause GEAR to be cleaned up as well.

The `CLEAN_TARGETS` variable should be added to `.PHONY` if you're using GNU make.

make.mk defines the variables `GEAR_SHARED_LIBRARY` and `GEAR_STATIC_LIBRARY` which contain the paths of the specified libraries.

Installation
------------

You can install the GEAR shared library by

```sh
make install
```

Run

```sh
make uninstall
```

to uninstall.

Testing
=======

Testing can be performed through the [Scrutiny framework](https://github.com/nickeldan/scrutiny).  After installing at least version 0.5.0 of the framework, you can run GEAR's tests by

```sh
make tests
```
