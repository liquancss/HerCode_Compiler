#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Lexer *new_lexer(char *source)
{
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->pos = 0;
    lexer->current_char = source[0];
    lexer->current_indent = 0;
    lexer->indent_stack[0] = 0; // 初始化缩进栈（第0级=0）
    lexer->indent_top = 0;
    lexer->pending_dedents = 0;
    return lexer;
}

void advance(Lexer *lexer)
{
    lexer->pos++;
    lexer->current_char = lexer->source[lexer->pos];
}

Token *new_token(TokenType type, const char *value)
{
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->value = value ? strdup(value) : NULL; // 允许NULL值
    return token;
}
Token *next_token(Lexer *lexer)
{
    printf("Current char: %c, pos: %d\n", lexer->current_char, lexer->pos);

    // 处理待生成的DEDENT
    if (lexer->pending_dedents > 0)
    {
        lexer->pending_dedents--;
        printf("[LEXER] Generating pending DEDENT (%d left)\n", lexer->pending_dedents);
        return new_token(TOKEN_DEDENT, NULL);
    }

    // 处理文件结束情况
    if (lexer->current_char == '\0')
    {
        // 文件结束时处理剩余缩进
        if (lexer->indent_top > 0)
        {
            printf("[LEXER] End of file, generating DEDENT for remaining indent\n");
            lexer->indent_top--;
            lexer->pending_dedents = lexer->indent_top;
            return new_token(TOKEN_DEDENT, NULL);
        }
        printf("[LEXER] End of file, returning EOF token\n");
        return new_token(TOKEN_EOF, NULL);
    }

    while (lexer->current_char != '\0')
    {
        if (lexer->current_char == '\n')
        {
            Token *t = handle_newline_and_indent(lexer);
            if (t)
                return t;
            continue;
        }

        if (isspace(lexer->current_char))
        {
            advance(lexer);
            continue;
        }

        if (isalpha(lexer->current_char))
        {
            char buffer[256];
            int i = 0;
            while (isalnum(lexer->current_char) && i < 255)
            {
                buffer[i++] = lexer->current_char;
                advance(lexer);
            }
            buffer[i] = '\0';
            printf("Identifier: %s\n", buffer);
            if (strcmp(buffer, "say") == 0)
                return new_token(TOKEN_SAY, "say");
            if (strcmp(buffer, "start") == 0)
                return new_token(TOKEN_START, "start");
            if (strcmp(buffer, "end") == 0)
                return new_token(TOKEN_END, "end");
            return new_token(TOKEN_UNKNOWN, buffer);
        }

        if (lexer->current_char == '"')
        {
            advance(lexer);
            char buffer[256];
            int i = 0;
            while (lexer->current_char != '"' && lexer->current_char != '\0')
            {
                buffer[i++] = lexer->current_char;
                advance(lexer);
            }
            if (lexer->current_char == '"')
                advance(lexer);
            buffer[i] = '\0';
            return new_token(TOKEN_STRING, buffer);
        }

        Token *unknown = new_token(TOKEN_UNKNOWN, (char[]){lexer->current_char, '\0'});
        advance(lexer);
        return unknown;
    }

    // 文件结束时处理剩余缩进
    if (lexer->indent_top > 0)
    {
        lexer->indent_top--;
        lexer->pending_dedents = lexer->indent_top;
        return new_token(TOKEN_DEDENT, NULL);
    }
    return new_token(TOKEN_EOF, "");
}

// 处理换行和缩进
Token *handle_newline_and_indent(Lexer *lexer)
{
    // 跳过当前换行符
    if (lexer->current_char == '\n')
    {
        advance(lexer);
    }

    // 检查是否到达EOF
    if (lexer->current_char == '\0')
    {
        printf("[LEXER] End of file after newline, no token generated\n");
        return NULL;
    }

    int new_indent = 0;

    // 计算当前行的缩进（只考虑空格）
    while (lexer->current_char == ' ')
    {
        new_indent++;
        advance(lexer);

        // 检查是否到达行尾或文件尾
        if (lexer->current_char == '\0')
        {
            printf("[LEXER] End of file during indentation calculation, no token generated\n");
            return NULL;
        }
    }

    // 添加调试信息
    printf("[LEXER] Newline: new_indent=%d, current_indent_stack=%d\n",
           new_indent, lexer->indent_stack[lexer->indent_top]);

    // 如果遇到连续换行符或文件结束
    if (lexer->current_char == '\n' || lexer->current_char == '\0')
    {
        printf("[LEXER] Newline without content, returning NEWLINE token\n");
        return new_token(TOKEN_NEWLINE, NULL);
    }

    int current_indent = lexer->indent_stack[lexer->indent_top];

    // 处理缩进级别变化
    if (new_indent > current_indent)
    {
        lexer->indent_top++;
        lexer->indent_stack[lexer->indent_top] = new_indent;
        return new_token(TOKEN_INDENT, NULL);
    }
    else if (new_indent < current_indent)
    {
        // 计算需要退出的缩进层级数
        int levels_to_dedent = 0;
        while (lexer->indent_top > 0 && new_indent < lexer->indent_stack[lexer->indent_top])
        {
            lexer->indent_top--;
            levels_to_dedent++;
        }

        // 设置待生成的DEDENT数量
        lexer->pending_dedents = levels_to_dedent;

        // 如果还有更多DEDENT需要生成，留待后续处理
        if (levels_to_dedent > 1)
        {
            lexer->pending_dedents = levels_to_dedent - 1;
        }

        return new_token(TOKEN_DEDENT, NULL);
    }
    else
    {
        return new_token(TOKEN_NEWLINE, NULL);
    }
}