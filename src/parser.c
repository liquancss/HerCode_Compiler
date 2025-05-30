#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STATEMENTS 100
// 添加此函数以获取token类型的字符串表示
const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_UNKNOWN:
        return "UNKNOWN";
    case TOKEN_SAY:
        return "SAY";
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_SEMI:
        return "SEMI";
    case TOKEN_START:
        return "START";
    case TOKEN_END:
        return "END";
    case TOKEN_NEWLINE:
        return "NEWLINE";
    case TOKEN_INDENT:
        return "INDENT";
    case TOKEN_DEDENT:
        return "DEDENT";
    default:
        return "UNRECOGNIZED";
    }
}

Parser *new_parser(Lexer *lexer)
{
    Parser *parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = next_token(lexer);
    parser->current_indent = 0; // 初始缩进深度为0
    return parser;
}

void free_parser(Parser *parser)
{
    free_token(parser->current_token);
    free(parser->lexer);
    free(parser);
}

void free_token(Token *token)
{
    if (token)
    {
        free(token->value);
        free(token);
    }
}

void eat(Parser *parser, TokenType type)
{
    if (parser->current_token->type == type)
    {
        free_token(parser->current_token);
        parser->current_token = next_token(parser->lexer);
    }
    else
    {
        printf("Syntax error: Unexpected token\n");
        exit(1);
    }
}

ASTNode *parse_statement(Parser *parser)
{
    // 记录起始缩进级别
    int initial_indent = parser->current_indent;

    // 跳过无关token
    while (parser->current_token->type == TOKEN_DEDENT ||
           parser->current_token->type == TOKEN_NEWLINE ||
           parser->current_token->type == TOKEN_INDENT)
    {

        // 更新缩进状态
        if (parser->current_token->type == TOKEN_INDENT)
        {
            parser->current_indent++;
        }
        else if (parser->current_token->type == TOKEN_DEDENT)
        {
            parser->current_indent--;

            // 检查是否退出到父级代码块
            if (parser->current_indent <= initial_indent)
            {
                return NULL; // 当前语句块结束
            }
        }

        eat(parser, parser->current_token->type);
    }

    // 确保当前token是有效语句开始
    if (parser->current_token->type != TOKEN_SAY)
    {
        fprintf(stderr, "Syntax error: Expected statement. Got token type %d (%s)\n",
                parser->current_token->type,
                token_type_to_string(parser->current_token->type));
        exit(1);
    }

    // 解析say语句
    if (parser->current_token->type == TOKEN_SAY)
    {
        eat(parser, TOKEN_SAY);

        if (parser->current_token->type != TOKEN_STRING)
        {
            fprintf(stderr, "Syntax error: Expected string after 'say'\n");
            exit(1);
        }

        char *str_value = parser->current_token->value ? strdup(parser->current_token->value) : strdup("");

        eat(parser, TOKEN_STRING);

        return create_say_node(str_value);
    }

    // 未知语句类型
    fprintf(stderr, "Syntax error: Unknown statement type %d\n",
            parser->current_token->type);
    exit(1);
}

ASTNode *parse_block(Parser *parser, int *count)
{
    *count = 0;
    ASTNode **nodes = malloc(MAX_STATEMENTS * sizeof(ASTNode *));

    while (1)
    {
        // 处理行内Token
        while (parser->current_token->type == TOKEN_NEWLINE ||
               parser->current_token->type == TOKEN_INDENT)
        { // 添加对缩进Token的处理
            eat(parser, parser->current_token->type);
        }

        // 块结束检查
        if (parser->current_token->type == TOKEN_DEDENT ||
            parser->current_token->type == TOKEN_END)
        {
            break;
        }

        nodes[*count] = parse_statement(parser);
        (*count)++;
    }
    return *nodes;
}

ASTNode **parse_program(Parser *parser, int *count)
{
    *count = 0;
    ASTNode **nodes = malloc(MAX_STATEMENTS * sizeof(ASTNode *));
    // 跳过开头的所有换行符（可能包括由注释和空行产生的）
    while (parser->current_token->type == TOKEN_NEWLINE)
    {
        free_token(parser->current_token); // 释放当前token
        parser->current_token = next_token(parser->lexer);
    }
    // 程序必须以start开始
    if (parser->current_token->type != TOKEN_START)
    {
        fprintf(stderr, "Syntax error: Program must start with 'start'\n");
        exit(1);
    }
    eat(parser, TOKEN_START);

    // 处理可能的换行
    if (parser->current_token->type == TOKEN_NEWLINE)
    {
        eat(parser, TOKEN_NEWLINE);
    }

    // 必须有缩进
    if (parser->current_token->type != TOKEN_INDENT)
    {
        fprintf(stderr, "Syntax error: Expected indentation after 'start'\n");
        exit(1);
    }
    eat(parser, TOKEN_INDENT);
    parser->current_indent++;

    // 解析程序主体
    while (parser->current_token->type != TOKEN_EOF &&
           parser->current_token->type != TOKEN_END)
    {
        // 处理语句间换行
        if (parser->current_token->type == TOKEN_NEWLINE)
        {
            eat(parser, TOKEN_NEWLINE);
            continue;
        }

        // 处理缩出
        if (parser->current_token->type == TOKEN_DEDENT)
        {
            eat(parser, TOKEN_DEDENT);
            parser->current_indent--;

            // 检查是否回到程序开始缩进级别
            if (parser->current_indent == 0)
            {
                break;
            }
            continue;
        }

        // 确保不超过最大语句数
        if (*count >= MAX_STATEMENTS)
        {
            fprintf(stderr, "Too many statements\n");
            exit(1);
        }

        // 解析语句
        nodes[(*count)++] = parse_statement(parser);
    }

    // 处理程序结束逻辑
    if (parser->current_token->type == TOKEN_DEDENT)
        eat(parser, TOKEN_DEDENT);

    // 检查是否在文件结束前找到了end关键字
    if (parser->current_token->type == TOKEN_EOF)
    {
        // 如果缩进级别正确，允许无end的程序
        if (parser->current_indent == 0)
        {
            printf("[PARSER] Program ends without explicit 'end', but indent level is correct\n");
            *count = 0; // 返回空程序
            return nodes;
        }
        fprintf(stderr, "Syntax error: Unexpected end of file. Expected 'end' at end of program\n");
        exit(1);
    }

    // 处理end关键字
    if (parser->current_token->type == TOKEN_END)
    {
        eat(parser, TOKEN_END);
    }
    else
    {
        fprintf(stderr, "Syntax error: Expected 'end' at end of program. Got token type %d (%s)\n",
                parser->current_token->type,
                token_type_to_string(parser->current_token->type));
        exit(1);
    }

    // 确保缩进级别回到0
    if (parser->current_indent != 0)
    {
        fprintf(stderr, "Syntax error: Missing dedent at end of program (indent level=%d)\n",
                parser->current_indent);
        exit(1);
    }

    return nodes;
}