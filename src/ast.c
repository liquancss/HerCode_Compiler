#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *create_say_node(char *str)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = STMT_SAY;
    node->value = strdup(str);
    return node;
}

ASTNode *create_function_def_node(char *name, ASTNode **body, int body_count)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = STMT_FUNCTION_DEF;
    node->value = strdup(name);
    node->body = malloc(sizeof(ASTNode *) * body_count);
    node->body_count = body_count;

    for (int i = 0; i < body_count; i++)
    {
        node->body[i] = body[i];
    }

    return node;
}

ASTNode *create_function_call_node(char *name)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = STMT_FUNCTION_CALL;
    node->value = strdup(name);
    node->body = NULL;
    node->body_count = 0;
    return node;
}

void free_node(ASTNode *node)
{
    if (node)
    {
        free(node->value);
        free(node);
    }
}