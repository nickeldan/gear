CFLAGS := -std=gnu11 -fdiagnostics-color -Wall -Wextra -fvisibility=hidden
ifeq ($(debug),yes)
    CFLAGS += -O0 -g -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

all: _all

BUILD_DEPS :=
ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),format)
else ifeq ($(MAKECMDGOALS),uninstall)
else
    BUILD_DEPS := yes
endif

GEAR_DIR := .
include make.mk

TEST_DIR := tests
include $(TEST_DIR)/make.mk

.PHONY: all _all format tests install uninstall clean $(CLEAN_TARGETS)

_all: $(GEAR_SHARED_LIBRARY) $(GEAR_STATIC_LIBRARY)

format:
	find . -name '*.[hc]' -print0 | xargs -0 -n 1 clang-format -i

install: /usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY)) $(foreach file,$(GEAR_HEADER_FILES),/usr/local/include/gear/$(notdir $(file)))

/usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY)): $(GEAR_SHARED_LIBRARY)
	cp $< $@
	ldconfig

/usr/local/include/gear/%.h: include/gear/%.h /usr/local/include/gear
	@mkdir -p $(@D)
	cp $< $@

uninstall:
	rm -rf /usr/local/include/gear
	rm -f /usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY))
	ldconfig

clean: $(CLEAN_TARGETS)
	@rm -f $(DEPS_FILES)
