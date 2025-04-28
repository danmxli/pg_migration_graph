CC = gcc
CFLAGS = -I/usr/include -Wall -Wextra
LDFLAGS = -lpg_query -ljansson -lmicrohttpd

# Main application
pg_migration_graph.out: src/main.c src/parser.c src/graph_builder.c src/helper.c src/api.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Test binary
test_parser.out: tests/test_parser.c src/parser.c src/helper.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Default target
all: pg_migration_graph.out

# Run tests
test: test_parser.out
	./test_parser.out
	rm -f test_parser.out

# Clean up
clean:
	rm -f pg_migration_graph.out test_parser.out