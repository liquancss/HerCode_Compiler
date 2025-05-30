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

void free_node(ASTNode *node)
{
    if (node)
    {
        free(node->value);
        free(node);
    }
}