#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "ast.h"

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("File opening failed");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <sourcefile>\n", argv[0]);
        return 1;
    }

    char *source = read_file(argv[1]);
    if (!source)
        return 1;

    Lexer *lexer = new_lexer(source);

    Parser *parser = new_parser(lexer);
    int stmt_count = 0;
    ASTNode **program = parse_program(parser, &stmt_count);

    char *output_c = "output.c";
    FILE *c_file = fopen(output_c, "w");
    generate_c_code(program, stmt_count, c_file);
    fclose(c_file);

    compile(output_c, "output");

    for (int i = 0; i < stmt_count; i++)
        free_node(program[i]);
    free(program);
    free_parser(parser);
    free(source);

    printf("Compilation successful. Output: ./output\n");
    return 0;
}