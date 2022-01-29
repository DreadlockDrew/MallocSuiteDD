CC := gcc


CFLAGS := -g -Wall -Werror -std=c99 -fPIC -D_DEFAULT_SOURCE


TESTS := test_bulk test_simple_malloc
NEWTESTS := test_malloc_varied_insert test_single_malloc_deref test_malloc_overflow test_single_malloc_deref test_free_ptr test_calloc test_realloc

all: libcsemalloc.so


libcsemalloc.so: src/mm.o src/bulk.o
	$(CC) -shared -fPIC -o $@ $^

test: $(TESTS) $(NEWTESTS)
	@echo
	@for test in $^; do                                   \
	    printf "Running %-30s: " "$$test";                \
	    (./$$test && echo "passed") || echo "failed";       \
	done
	@echo


submission: malloc.tar

malloc.tar: src/mm.c
	tar cf $@ $^


%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%: tests/%.o src/mm.o src/bulk.o
	$(CC) -o $@ $^

clean:
	rm -f $(TESTS) $(NEWTESTS) libcsemalloc.so malloc.tar
	rm -f src/*.o tests/*.o *~ src/*~ tests/*~ src/test_*

.PHONY: all clean submission test
