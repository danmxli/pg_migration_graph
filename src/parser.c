#include "parser.h"
#include <pg_query.h>
#include <stddef.h>
#include <jansson.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"
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
    // log_json("AST JSON", ast_json);

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
        // log_json("STMT NODE", stmt_node);

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
            // log_json("CREATE STMT COLUMNS", create_stmt_columns);

            // get the number of columns
            size_t num_columns = json_array_size(create_stmt_columns);
            parse_tree->stmts[i].create_stmt.num_columns = num_columns;

            parse_tree->stmts[i].create_stmt.columns = (ColumnDef *)calloc(num_columns, sizeof(ColumnDef));
            for (size_t j = 0; j < num_columns; j++)
            {
                json_t *column = json_array_get(create_stmt_columns, j);

                // Check for ColumnDef first
                json_t *column_def = json_object_get(column, "ColumnDef");
                if (column_def)
                {
                    json_t *column_name = json_object_get(column_def, "colname");
                    json_t *column_type = json_object_get(column_def, "typeName");

                    // Initialize column with name
                    const char *column_name_str = json_string_value(column_name);
                    parse_tree->stmts[i].create_stmt.columns[j].colname = strdup(column_name_str);

                    // Process type if available
                    if (column_type)
                    {
                        json_t *column_type_names = json_object_get(column_type, "names");

                        // Check if column_type_names exists before using it
                        if (column_type_names && json_is_array(column_type_names))
                        {
                            // // logging: print the column_type_names
                            // log_json("COLUMN TYPE NAMES", column_type_names);

                            // if the length of the column_type_names is at least 2
                            if (json_array_size(column_type_names) >= 2)
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
                                    }
                                }
                            }
                        }
                        else
                        {
                            // // logging: print the column_type_names
                            // log_json("COLUMN TYPE NAMES", column_type_names);
                            // Set default type for columns without type info
                            parse_tree->stmts[i].create_stmt.columns[j].typeName = strdup("unknown");
                        }
                    }
                    else
                    {
                        // Handle columns with no type info at all
                        parse_tree->stmts[i].create_stmt.columns[j].typeName = strdup("unknown");
                    }
                }
                else
                {
                    // This element might be a constraint, not a column
                    // Just add placeholder data to avoid segfaults
                    parse_tree->stmts[i].create_stmt.columns[j].colname = strdup("constraint");
                    parse_tree->stmts[i].create_stmt.columns[j].typeName = strdup("constraint");

                    // // logging: print the column
                    // log_json("COLUMN", column);
                }
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
    if (!parse_tree)
        return;

    for (size_t i = 0; i < parse_tree->num_stmts; i++)
    {
        switch (parse_tree->stmts[i].stmt_type)
        {
        case STMT_CREATE_TABLE:
            // Free the table name
            free(parse_tree->stmts[i].create_stmt.relation.relname);

            // Free each column's fields
            for (size_t j = 0; j < parse_tree->stmts[i].create_stmt.num_columns; j++)
            {
                free(parse_tree->stmts[i].create_stmt.columns[j].colname);
                free(parse_tree->stmts[i].create_stmt.columns[j].typeName);
                // Free any other dynamically allocated fields in ColumnDef
            }

            // Free the columns array
            free(parse_tree->stmts[i].create_stmt.columns);
            break;

        case STMT_ALTER_TABLE:
            // Free the table name
            free(parse_tree->stmts[i].alter_stmt.relation.relname);

            // Free columns if any
            if (parse_tree->stmts[i].alter_stmt.columns)
            {
                for (size_t j = 0; j < parse_tree->stmts[i].alter_stmt.num_columns; j++)
                {
                    free(parse_tree->stmts[i].alter_stmt.columns[j].colname);
                    // Free any other fields in the columns
                }
                free(parse_tree->stmts[i].alter_stmt.columns);
            }
            break;

        default:
            // No special handling for unknown statement types
            break;
        }
    }

    // Free the statements array and the parse tree itself
    free(parse_tree->stmts);
    free(parse_tree);
}
