#ifndef ast_h
#define ast_h
#include <stdio.h>
#include <stdlib.h>

// Enumeración de los tipos de nodos del AST
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

// Estructura de la lista de hijos de un nodo del AST
struct children_node_ast
{
    struct NodeAST *start;
};

/*
 * Función: Crear la lista de hijos de un nodo para el AST
 * @return new_list: Lista de hijos creada
*/
struct children_node_ast *create_children_list();

/*
 * Función: Crear un nodo del AST 
 * @param type: Tipo de nodo
 * @param lexema: Lexema del nodo
 * @return new_node: Nodo creado
*/
struct NodeAST *create_ast_node(enum ASTNodeType type, char *lexema);

/*
 * Función: Agregar un hijo a un nodo del AST
 * @param parent: Nodo padre
 * @param child: Nodo hijo
*/
void add_child(struct NodeAST *parent, struct NodeAST *child);

/*
 * Función: Crear un nodo y agregarlo como hijo de otro nodo
 * @param type: Tipo de nodo
 * @param lexema: Lexema del nodo a crear
 * @param parent: Nodo padre
 * @return new_node: Nodo creado
*/
struct NodeAST *create_and_add_node(enum ASTNodeType type, char *lexema, struct NodeAST *parent);

/*
 * Función: Imprimir el arbol AST
 * @param root: Nodo raiz del arbol
 * @param depth: Profundidad actual del nodo
*/
void print_AST_tree(struct NodeAST *root, int depth);



#endif