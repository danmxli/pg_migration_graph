#ifndef PARSER_H
#define PARSER_H
#include <pg_query.h>
#include <stddef.h>
#include <jansson.h>

// enum for the type of statement
typedef enum
{
    STMT_CREATE_TABLE,
    STMT_ALTER_TABLE,
    STMT_UNKNOWN
} StmtType;

// struct for the range variable
typedef struct
{
    char *relname;
} RangeVar;

// struct for the column definition
typedef struct
{
    char *colname;
    char *typeName;
} ColumnDef;

// struct for the column reference
typedef struct
{
    char *colname;
    RangeVar relname;
} ColumnRef;

// struct for the create statement
typedef struct
{
    RangeVar relation;
    ColumnDef *columns;
    size_t num_columns;
} CreateStmt;

// struct for the alter table statement
typedef struct
{
    RangeVar relation;
    ColumnRef *columns;
    size_t num_columns;
} AlterTableStmt;

// struct for the statement
typedef struct
{
    StmtType stmt_type;
    union
    {
        CreateStmt create_stmt;
        AlterTableStmt alter_stmt;
    };
} PgQueryStmt;

// struct for the parse tree
typedef struct
{
    int version;
    PgQueryStmt *stmts;
    size_t num_stmts;
} PgQueryParseTree;

// struct for the parse result
typedef struct
{
    PgQueryParseTree *parse_tree;
    char *error;
} ParseResult;

/// @brief Parse a query into a parse tree
/// @param query The query to parse
/// @return The parse result
ParseResult parse_query(const char *query);

/// @brief Free a parse tree. To be called after the parse result is no longer needed.
/// @param parse_tree The parse tree to free
void free_parse_tree(PgQueryParseTree *parse_tree);

#endif
