// helper.c
#include "helper.h"
#include "parser.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void log_json(const char *message, json_t *json)
{
    char *dump = json_dumps(json, JSON_INDENT(2));
    printf("%s: %s\n", message, dump);
    free(dump);
}

void print_parse_tree(PgQueryParseTree *parse_tree)
{
    if (!parse_tree)
    {
        printf("Parse tree is NULL\n");
        return;
    }

    printf("PostgreSQL Parse Tree (Version: %d)\n", parse_tree->version);
    printf("Number of statements: %zu\n", parse_tree->num_stmts);

    for (size_t i = 0; i < parse_tree->num_stmts; i++)
    {
        printf("\nStatement #%zu:\n", i + 1);
        print_statement(&parse_tree->stmts[i]);
    }
}

void print_statement(PgQueryStmt *stmt)
{
    if (!stmt)
    {
        printf("Statement is NULL\n");
        return;
    }

    switch (stmt->stmt_type)
    {
    case STMT_CREATE_TABLE:
        printf("Type: CREATE TABLE\n");
        print_create_table(&stmt->create_stmt);
        break;
    case STMT_ALTER_TABLE:
        printf("Type: ALTER TABLE\n");
        print_alter_table(&stmt->alter_stmt);
        break;
    case STMT_UNKNOWN:
        printf("Type: UNKNOWN\n");
        break;
    default:
        printf("Type: INVALID (%d)\n", stmt->stmt_type);
        break;
    }
}

void print_create_table(CreateStmt *create_stmt)
{
    if (!create_stmt)
    {
        printf("Create statement is NULL\n");
        return;
    }

    printf("Table: %s\n", create_stmt->relation.relname ? create_stmt->relation.relname : "UNNAMED");
    printf("Columns (%zu):\n", create_stmt->num_columns);

    for (size_t i = 0; i < create_stmt->num_columns; i++)
    {
        printf("  - %s: %s\n", (create_stmt->columns[i].colname ? create_stmt->columns[i].colname : "UNNAMED"), (create_stmt->columns[i].typeName));
    }
}

void print_alter_table(AlterTableStmt *alter_stmt)
{
    if (!alter_stmt)
    {
        printf("Alter statement is NULL\n");
        return;
    }

    printf("Table: %s\n", alter_stmt->relation.relname ? alter_stmt->relation.relname : "UNNAMED");
    printf("Columns (%zu):\n", alter_stmt->num_columns);

    for (size_t i = 0; i < alter_stmt->num_columns; i++)
    {
        printf("  - %s\n", alter_stmt->columns[i].colname ? alter_stmt->columns[i].colname : "UNNAMED");
    }
}