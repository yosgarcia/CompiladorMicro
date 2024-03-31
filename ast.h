#ifndef ast_h
#define ast_h
#include <stdio.h>
#include <stdlib.h>

// Enumeracion de los tipos de nodos del AST
enum ASTNodeType
{
    PROGRAM_AST,
    STATEMENT_LIST_AST,
    STATEMENT_AST,
    EXPRESSION_AST,
    PRIMARY_AST,
    EXPRESSION_LIST_AST,
    ID_LIST_AST,
    PLUS_OP_AST,
    MINUS_OP_AST,
    ASSIGN_OP_AST,
    READ_SYM_AST,
    WRITE_SYM_AST,
    ID_AST,
    INT_LITERAL_AST,
    ADD_OP_AST
};

// Estructura de un nodo del AST
struct NodeAST
{
    struct NodeAST *parent;
    enum ASTNodeType type;
    char *lexema;
    struct children_node_ast *children;
    struct NodeAST *next;
};

struct children_node_ast
{
    struct NodeAST *start;
};


struct children_node_ast *create_children_list();

struct NodeAST *create_ast_node(enum ASTNodeType type, char *lexema);

void add_child(struct NodeAST *parent, struct NodeAST *child);

struct NodeAST *create_and_add_node(enum ASTNodeType type, char *lexema, struct NodeAST *parent);

void print_AST_tree(struct NodeAST *root, int depth);



#endif