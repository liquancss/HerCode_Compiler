#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

void generate_c_code(const char *c_header, ASTNode **nodes, int count, FILE *output);
void compile(char *c_filename, char *output_name);

#endif