#include "../src/parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_create_table_simple() {
    const char *sql = "CREATE TABLE test (id int, name varchar(255));";
    ParseResult result = parse_query(sql);
    
    assert(result.error == NULL);
    assert(result.parse_tree != NULL);
    assert(result.parse_tree->num_stmts == 1);
    assert(result.parse_tree->stmts[0].stmt_type == STMT_CREATE_TABLE);
    assert(strcmp(result.parse_tree->stmts[0].create_stmt.relation.relname, "test") == 0);
    assert(result.parse_tree->stmts[0].create_stmt.num_columns == 2);
    assert(strcmp(result.parse_tree->stmts[0].create_stmt.columns[0].colname, "id") == 0);
    assert(strcmp(result.parse_tree->stmts[0].create_stmt.columns[0].typeName, "int4") == 0);
    
    free_parse_tree(result.parse_tree);
    printf("test_create_table_simple: PASSED\n");
}

int main() {
    test_create_table_simple();
    // TODO: Add more test cases
    return 0;
}