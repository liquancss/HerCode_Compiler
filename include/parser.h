#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

// parser.h
typedef struct Parser
{
    Lexer *lexer;
    Token *current_token;
    int current_indent; // 当前缩进级别
} Parser;

Parser *new_parser(Lexer *lexer);
void free_parser(Parser *parser);
void free_token(Token *token);
void free_token(Token *token);
ASTNode *parse_statement(Parser *parser);
ASTNode *parse_block(Parser *parser, int *count);
ASTNode **parse_program(Parser *parser, int *count);

#endif