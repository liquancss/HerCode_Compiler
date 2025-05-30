#ifndef AST_H
#define AST_H

typedef enum
{
    STMT_SAY
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    char *value;
} ASTNode;

ASTNode *create_say_node(char *str);
void free_node(ASTNode *node);

#endif