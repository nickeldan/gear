CC ?= gcc
debug ?= no

CFLAGS := -std=gnu11 -fdiagnostics-color -Wall -Wextra
ifeq ($(debug),yes)
    CFLAGS += -O0 -g -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

all: _all

GEAR_DIR := .
include make.mk

.PHONY: all _all format install uninstall clean $(CLEAN_TARGETS)

_all: $(GEAR_SHARED_LIBRARY) $(GEAR_STATIC_LIBRARY) tests

format:
	find . -name '*.[hc]' -print0 | xargs -0 clang-format -i

install: /usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY)) $(foreach file,$(GEAR_HEADER_FILES),/usr/local/include/reap/$(notdir $(file)))

/usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY)): $(GEAR_SHARED_LIBRARY)
	cp $< $@

/usr/local/include/reap/%.h: $(GEAR_INCLUDE_DIR)/reap/%.h /usr/local/include/reap
	cp $< $@

/usr/local/include/reap:
	mkdir -p $@

uninstall:
	rm -rf /usr/local/include/reap
	rm -f /usr/local/lib/$(notdir $(GEAR_SHARED_LIBRARY))

clean: $(CLEAN_TARGETS)
	@rm -f $(DEPS_FILES)
