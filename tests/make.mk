TEST_BINARY := $(TEST_DIR)/tests

$(TEST_BINARY): $(TEST_DIR)/tests.c $(GEAR_SHARED_LIBRARY)
	$(CC) $(CFLAGS) $(GEAR_INCLUDE_FLAGS) $< -Wl,-rpath $(realpath $(GEAR_LIB_DIR)) -L$(GEAR_LIB_DIR) -lgear -lscrutiny -o $@

tests: $(TEST_BINARY)
	@$<

tests_clean:
	@rm -f $(TEST_BINARY)

CLEAN_TARGETS += tests_clean
