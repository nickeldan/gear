GEAR_LIB_DIR ?= $(GEAR_DIR)
GEAR_OBJ_DIR ?= $(GEAR_DIR)/src
GEAR_SHARED_LIBRARY := $(GEAR_LIB_DIR)/libgear.so
GEAR_STATIC_LIBRARY := $(GEAR_LIB_DIR)/libgear.a

GEAR_SOURCE_FILES := $(wildcard $(GEAR_DIR)/src/*.c)
GEAR_OBJECT_FILES := $(patsubst $(GEAR_DIR)/src/%.c,$(GEAR_OBJ_DIR)/%.o,$(GEAR_SOURCE_FILES))
GEAR_HEADER_FILES := $(wildcard $(GEAR_DIR)/include/gear/*.h) 
GEAR_INCLUDE_DIR := $(GEAR_DIR)/include
GEAR_INCLUDE_FLAGS := -I$(GEAR_INCLUDE_DIR) 

GEAR_DEPS_FILE := $(GEAR_DIR)/deps.mk
DEPS_FILES += $(GEAR_DEPS_FILE)

ifneq ($(MAKECMDGOALS),clean)

$(GEAR_DEPS_FILE): $(GEAR_SOURCE_FILES) $(GEAR_HEADER_FILES)
	rm -f $@
	for file in $(GEAR_SOURCE_FILES); do \
	  echo "$(GEAR_OBJ_DIR)/`$(CC) $(GEAR_INCLUDE_FLAGS) -MM $$file`" >> $@ && \
	  echo '\t$$(CC) $$(CFLAGS) -fpic -ffunction-sections $(GEAR_INCLUDE_FLAGS) -c $$< -o $$@' >> $@; \
	done
include $(GEAR_DEPS_FILE)

endif

$(GEAR_SHARED_LIBRARY): $(GEAR_OBJECT_FILES)
	$(CC) -shared -o $@ $^

$(GEAR_STATIC_LIBRARY): $(GEAR_OBJECT_FILES)
	$(AR) rcs $@ $^

gear_clean:
	@rm -f $(GEAR_SHARED_LIBRARY) $(GEAR_STATIC_LIBRARY) $(GEAR_OBJECT_FILES)

CLEAN_TARGETS += gear_clean
