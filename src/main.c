#include <stdlib.h>
#include <string.h>
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

void separate_header(const char *source, const char *magic_string,
                     char **c_header, char **hercode_source)
{
    *c_header = NULL;
    *hercode_source = NULL;

    char *magic_pos = strstr(source, magic_string);
    if (magic_pos == NULL)
    {
        return; // 没有找到特殊字符串
    }

    // 确保特殊字符串在行首
    if (magic_pos != source)
    {
        char *prev_char = magic_pos - 1;
        if (*prev_char != '\n' && *prev_char != '\r')
        {
            return; // 不在行首
        }
    }

    // 查找行结束位置
    char *line_end = strchr(magic_pos, '\n');
    if (line_end == NULL)
    {
        // 如果没有换行符，特殊字符串后没有内容
        size_t header_size = magic_pos - source;
        *c_header = malloc(header_size + 1);
        if (*c_header)
        {
            strncpy(*c_header, source, header_size);
            (*c_header)[header_size] = '\0';
        }
        *hercode_source = ""; // 空字符串
        return;
    }

    // 计算C头部分的大小
    size_t header_size = magic_pos - source;
    *c_header = malloc(header_size + 1);
    if (*c_header)
    {
        strncpy(*c_header, source, header_size);
        (*c_header)[header_size] = '\0';
    }

    // HerCode部分从下一行开始
    *hercode_source = line_end + 1;

    // 特殊处理CRLF换行
    if (*line_end == '\n' && line_end > magic_pos && *(line_end - 1) == '\r')
    {
        // 如果前面有CR，跳过它
        *hercode_source = line_end;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <source_file> [output_name]\n", argv[0]);
        return 1;
    }

    // 读取整个文件
    char *source = read_file(argv[1]);
    if (!source)
    {
        fprintf(stderr, "Error reading file: %s\n", argv[1]);
        return 1;
    }

    // 尝试分离C头部分
    char *c_header = NULL;
    char *hercode_source = NULL;
    separate_header(source, "Hello! Her World", &c_header, &hercode_source);
    printf("C Code:\n%s\n", c_header);
    // 验证分离结果
    if (hercode_source == NULL)
        hercode_source = source; // 如果分离失败，使用整个文件

    // 输出分离结果用于调试
    printf("HerCode Source to Parse:\n%s\n", hercode_source);

    // 创建词法分析器和解析器
    Lexer *lexer = new_lexer(hercode_source);
    Parser *parser = new_parser(lexer);

    // 解析程序
    int node_count;
    ASTNode **nodes = parse_program(parser, &node_count);
    printf("Parsed %d nodes\n", node_count);

    // 生成C代码
    FILE *c_file = fopen("temp.c", "w");
    if (!c_file)
    {
        perror("Error creating C file");
        return 1;
    }
    generate_c_code(c_header, nodes, node_count, c_file);
    fclose(c_file);

    // 编译
    char output_name[256] = "a.out";
    if (argc >= 3)
    {
        strncpy(output_name, argv[2], sizeof(output_name) - 1);
    }
    compile("temp.c", output_name);

    // 清理
    if (c_header)
        free(c_header);
    free(source);
    free_parser(parser);

    for (int i = 0; i < node_count; i++)
    {
        free_node(nodes[i]);
    }
    free(nodes);

    printf("Successfully generated: %s\n", output_name);
    return 0;
}