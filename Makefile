# TODO: dockerize this eventually
CC = gcc
CFLAGS = -I./deps/libpg_query -I/opt/homebrew/Cellar/jansson/2.14.1/include -Wall -Wextra
LDFLAGS = -L./deps/libpg_query -L/opt/homebrew/Cellar/jansson/2.14.1/lib -lpg_query -ljansson

# target to build the main program
pg_migration_graph.out: src/main.c src/parser.c src/graph_builder.c src/helper.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# target to build the test parser
test_parser.out: tests/test_parser.c src/parser.c src/helper.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# command to build the project
all: pg_migration_graph.out

# command to run the tests
test: test_parser.out
	./test_parser.out
	rm -f test_parser.out

# command to clean the project
clean:
	rm -f pg_migration_graph.out test_parser