#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_c_code(ASTNode **nodes, int count, FILE *output)
{
    fprintf(output, "#include <stdio.h>\n\nint main() {\n");
    for (int i = 0; i < count; i++)
    {
        if (nodes[i]->type == STMT_SAY)
            fprintf(output, "    printf(\"%%s\\n\", \"%s\");\n", nodes[i]->value);
    }
    fprintf(output, "    return 0;\n}\n");
}

void compile(char *c_filename, char *output_name)
{
    char cmd[256];
    sprintf(cmd, "gcc -o %s %s", output_name, c_filename);
    system(cmd);
}