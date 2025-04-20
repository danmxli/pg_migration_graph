// helper.h
#ifndef HELPER_H
#define HELPER_H

#include <jansson.h>
#include "parser.h"

// Function to print the entire parse tree in a formatted way
void print_parse_tree(PgQueryParseTree *parse_tree);

// Function to print details of a specific statement
void print_statement(PgQueryStmt *stmt);

// Function to print create table statement details
void print_create_table(CreateStmt *create_stmt);

// Function to print alter table statement details
void print_alter_table(AlterTableStmt *alter_stmt);

#endif