#include "parser.h"
#include <pg_query.h>
#include <stddef.h>
#include <jansson.h>
#include <stdio.h>
#include <string.h>

ParseResult parse_query(const char *query)
{
    PgQueryParseResult result;
    result = pg_query_parse(query);
    if (result.error)
    {
        return (ParseResult){
            .parse_tree = NULL,
            .error = result.error->message};
    }

    json_error_t error;
    json_t *ast_json = json_loads(result.parse_tree, 0, &error);
    if (!ast_json)
    {
        return (ParseResult){
            .parse_tree = NULL,
            .error = error.text};
    }

    // // logging: print the ast_json
    // printf("AST JSON: %s\n", json_dumps(ast_json, JSON_INDENT(2)));

    PgQueryParseTree *parse_tree = (PgQueryParseTree *)malloc(sizeof(PgQueryParseTree));

    // get the postgres version of the parse tree
    parse_tree->version = json_integer_value(json_object_get(ast_json, "version"));

    // get the statements of the parse tree
    json_t *stmts = json_object_get(ast_json, "stmts");

    parse_tree->num_stmts = json_array_size(stmts);
    parse_tree->stmts = (PgQueryStmt *)calloc(parse_tree->num_stmts, sizeof(PgQueryStmt));

    for (size_t i = 0; i < parse_tree->num_stmts; i++)
    {
        json_t *stmt = json_array_get(stmts, i);
        json_t *stmt_node = json_object_get(stmt, "stmt");

        // // logging: print the stmt_node
        // printf("stmt_node: %s\n", json_dumps(stmt_node, JSON_INDENT(2)));

        // get the first key of the stmt_node
        void *iter = json_object_iter(stmt_node);
        const char *key = json_object_iter_key(iter);
        if (strcmp(key, "CreateStmt") == 0)
        {
            // map the parse_tree properties to the stmt properties
            parse_tree->stmts[i].stmt_type = STMT_CREATE_TABLE;
            json_t *create_stmt = json_object_get(stmt_node, "CreateStmt");
            json_t *create_stmt_relation = json_object_get(create_stmt, "relation");
            const char *relname = json_string_value(json_object_get(create_stmt_relation, "relname"));
            parse_tree->stmts[i].create_stmt.relation = (RangeVar){
                .relname = strdup(relname)};
            json_t *create_stmt_columns = json_object_get(create_stmt, "tableElts");

            // // logging: print the create_stmt_columns
            // printf("create_stmt_columns: %s\n", json_dumps(create_stmt_columns, JSON_INDENT(2)));

            // get the number of columns
            size_t num_columns = json_array_size(create_stmt_columns);
            parse_tree->stmts[i].create_stmt.num_columns = num_columns;

            parse_tree->stmts[i].create_stmt.columns = (ColumnDef *)calloc(num_columns, sizeof(ColumnDef));
            for (size_t j = 0; j < num_columns; j++)
            {
                json_t *column = json_array_get(create_stmt_columns, j);
                json_t *column_def = json_object_get(column, "ColumnDef");
                json_t *column_name = json_object_get(column_def, "colname");
                json_t *column_type = json_object_get(column_def, "typeName");

                json_t *column_type_names = json_object_get(column_type, "names");

                // // logging: print the column_type_names
                // printf("column_type_names: %s\n", json_dumps(column_type_names, JSON_INDENT(2)));

                // if the length of the column_type_names is 2, then it is a pg_catalog type
                if (json_array_size(column_type_names) == 2)
                {
                    // ignore the first element of the column_type_names, as it is pg_catalog
                    json_t *column_type_name = json_array_get(column_type_names, 1);
                    json_t *string_obj = json_object_get(column_type_name, "String");
                    if (string_obj)
                    {
                        json_t *string_val = json_object_get(string_obj, "sval");
                        if (json_is_string(string_val))
                        {
                            parse_tree->stmts[i].create_stmt.columns[j].typeName = strdup(json_string_value(string_val));

                            // // logging: print the column_type_name
                            // printf("column_type_name: %s\n", parse_tree->stmts[i].create_stmt.columns[j].typeName);
                        }
                    }
                }
                else
                {
                    // TODO
                }

                const char *column_name_str = json_string_value(column_name);

                parse_tree->stmts[i].create_stmt.columns[j].colname = strdup(column_name_str);
            }
        }
        else if (strcmp(key, "AlterTableStmt") == 0)
        {
            parse_tree->stmts[i].stmt_type = STMT_ALTER_TABLE;
        }
        else
        {
            parse_tree->stmts[i].stmt_type = STMT_UNKNOWN;
        }
    }

    json_decref(ast_json);
    pg_query_free_parse_result(result);

    return (ParseResult){
        .parse_tree = parse_tree,
        .error = NULL};
}

void free_parse_tree(PgQueryParseTree *parse_tree)
{
    for (size_t i = 0; i < parse_tree->num_stmts; i++)
    {
        switch (parse_tree->stmts[i].stmt_type)
        {
        case STMT_CREATE_TABLE:
            free(parse_tree->stmts[i].create_stmt.columns);
            break;
        case STMT_ALTER_TABLE:
            free(parse_tree->stmts[i].alter_stmt.columns);
            break;
        default:
            break;
        }
    }
    free(parse_tree->stmts);
    free(parse_tree);
}
