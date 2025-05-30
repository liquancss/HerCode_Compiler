#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_c_code(const char *c_header, ASTNode **nodes, int count, FILE *output)
{
    // 写入C头文件部分
    fprintf(output, "#include <stdio.h>\n\n");

    // 主函数开始
    fprintf(output, "int main() {\n");
    // 如果有外部C代码头文件，写入它
    if (c_header != NULL)
    {
        fprintf(output, "    %s\n", c_header);
    }
    // 生成语句代码
    for (int i = 0; i < count; i++)
    {
        if (nodes[i]->type == STMT_SAY)
        {
            fprintf(output, "    printf(\"%%s\\n\", \"%s\");\n", nodes[i]->value);
        }
        // 可以添加对其他语句类型的支持
    }

    // 主函数结束
    fprintf(output, "    return 0;\n}\n");
}

void compile(char *c_filename, char *output_name)
{
    char cmd[256];
    sprintf(cmd, "gcc -o %s %s", output_name, c_filename);
    system(cmd);
}