#include "ast.h"
#include <stdio.h>
typedef struct
{
    char *name;
    ASTNode **body;
    int body_count;
} FunctionDef;

// 最大函数数量
#define MAX_FUNCTIONS 100
FunctionDef *find_function(const char *name, FunctionDef **functions, int function_count);
void generate_c_code(const char *c_header, ASTNode **nodes, int count, FILE *output);
void compile(char *c_filename, char *output_name);