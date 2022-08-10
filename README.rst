====
GEAR
====

:Author: Daniel Walker
:Version: 0.3.0
:Date: 2022-08-09

The GEAR library provides self-expanding arrays of arbitrary types.

API
===

You can access GEAR's functionality by

.. code-block:: c

    #include <gear/gear.h>

In order to create an array of, say, integers, declare

.. code-block:: c

    gear array = GEAR_INIT(int);

A new object can be appended to the array by

.. code-block:: c

    int x = 5;

    if ( gearAppend(&array, &x) != GEAR_RET_OK ) {
        // handle the error
    }

The possible return values of **gearAppend** are defined in gear/gear.h.

Two fields of the **gear** structure are available to users, **length** and **item_size**.  Both should be
self-explanatory.  It is important to note that no fields of the structure should be changed manually.
Instead, all operations should be performed via the API.  The one exception is the **length** field which can
only be decreased.

You can retrieve pointers to the elements of the array by

.. code-block:: c

    int *p;

    p = GEAR_GET_ITEM(&array, some_index);

You can also iterate over the elements of the array via

.. code-block:: c

    int *p;

    GEAR_FOR_EACH(&array, p) {
        printf("%i\n", *p);
    }

If you want to iterate with an index, do

.. code-block:: c

    int *p;
    size_t idx;

    GEAR_FOR_EACH_WITH_INDEX(&array, p, idx) {
        printf("%zu: %i\n", idx, *p);
    }

An array can have its data freed and its length set to zero by

.. code-block:: c

    gearFree(&array);

You can also concatenate two arrays as long as they have the same item size via

.. code-block:: c

    int
    gearConcatenate(gear *dst, const gear *src);

This function returns **GEAR_RET_OK** if successful and an error code otherwise.  Note that **src** is
untouched.

Expansion
---------

At any moment, an array has a capacity which is at least as big as its length.  The capacity indicates how
many elements the array can currently hold without needing to allocate more memory.  There are two methods
from which you can select to determine how the capacity is expanded when needed.

The first method, which is set by default when an array is initialized with **GEAR_INIT**, uses two
**unsigned int** constants, both of which must be positive.  The first specifies an initial capacity.  That
is, it specifies what the capacity will be set to when the first item is appended to the array.  The second
constant specifies by how much the capacity will be increased when it is filled up.  The default values of
these constants are defined in gear/config.h.

The second method uses a user-provided function pointer.  Specifically,

.. code-block:: c

    typedef size_t (*gearExpander)(size_t);

When determining the new capacity, **gearAppend** will call this function with the current capacity as the
argument.  The function can either return the new capacity or return a value less than or equal to the
current.  If the function returns the latter, that will tell **gearAppend** that the array cannot be expanded
further and **gearAppend** will return **GEAR_RET_NO_EXPANSION** to the caller.  You can use this as a method
of imposing a limit on the array's length.

The expansion method and associated parameter(s) can be set by

.. code-block:: c

    int
    gearSetExpansion(gear *array, gearExpansionMethod method, ...);

This function returns **GEAR_RET_OK** if successful and **GEAR_RET_BAD_USAGE** if the arguments are invalid.
**gearExpansionMethod** is an enum type.  For example,

.. code-block:: c

    gearSetExpansion(&array, GEAR_EXP_CONSTANTS, 5, 10); // Initial capacity of 5 with an expansion of 10

    gearSetExpansion(&array, GEAR_EXP_FUNCTION, someFunc);

**gearFree** preserves the changes made by **gearSetExpansion**.

Building GEAR
=============

Shared and static libraries are built using make.  Adding "debug=yes" to the make invocation will disable
optimization and build the libraries with debugging symbols.

You can also include GEAR in a larger project by including make.mk.  Before doing so, however, the
**GEAR_DIR** variable must be set to the location of the GEAR directory.  You can also tell make where to
place the shared and static libraries by defining the **GEAR_LIB_DIR** variable (defaults to **GEAR_DIR**).

make.mk adds a target to the **CLEAN_TARGETS** variable.  This is so that implementing

.. code-block:: make

    clean: $(CLEAN_TARGETS)
        ...

in your project's Makefile will cause GEAR to be cleaned up as well.

The **CLEAN_TARGETS** variable should be added to **.PHONY** if you're using GNU make.

make.mk defines the variables **GEAR_SHARED_LIBRARY** and **GEAR_STATIC_LIBRARY** which contain the paths of
the specified libraries.
