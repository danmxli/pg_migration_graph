#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

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

void build_graph_from_ast(const char *ast_json);

#endif
