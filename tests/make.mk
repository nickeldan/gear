TEST_BINARY := $(TEST_DIR)/tests

$(TEST_BINARY): $(TEST_DIR)/tests.c $(GEAR_STATIC_LIBRARY)
	$(CC) $(CFLAGS) $(GEAR_INCLUDE_FLAGS) $^ -lscrutiny -o $@

tests: $(TEST_BINARY)
	@$<

tests_clean:
	@rm -f $(TEST_BINARY)

CLEAN_TARGETS += tests_clean
