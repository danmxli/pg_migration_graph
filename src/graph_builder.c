#include <stdio.h>

typedef struct {
    char *id;
    char *type;
    void *properties;
} GraphNode;

typedef struct {
    char *source;
    char *target;
    int weight;
    char *label;
} GraphEdge;

void build_graph_from_ast(const char *ast_json) {
    // TODO
    printf("Building graph from AST: %s\n", ast_json);
}