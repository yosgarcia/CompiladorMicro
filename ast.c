#include <stdio.h>
#include <stdlib.h>
#include "ast.h"


/*
 * Funcion: Crear la lista de hijos de un nodo para el AST
 * @return new_list: Lista de hijos creada
*/
struct children_node_ast *create_children_list()
{
    struct children_node_ast *new_list = calloc(1, sizeof(struct children_node_ast));
    new_list->start = NULL;
    return new_list;
}

/*
 * Funcion: Crear un nodo del AST 
 * @param type: Tipo de nodo
 * @param lexema: Lexema del nodo
 * @return new_node: Nodo creado
*/
struct NodeAST *create_ast_node(enum ASTNodeType type, char *lexema)
{
    struct NodeAST *new_node = calloc(1, sizeof(struct NodeAST));
    new_node->parent = NULL;
    new_node->type = type;
    new_node->lexema = lexema;
    new_node->children = create_children_list();
    return new_node;
}

/*
 * Funcion: Agregar un hijo a un nodo del AST
 * @param parent: Nodo padre
 * @param child: Nodo hijo
*/
void add_child(struct NodeAST *parent, struct NodeAST *child)
{
    if (parent->children == NULL)
    {
        parent->children = create_children_list();
        parent->children->start = child;
        return;
    }
    if (parent->children->start == NULL)
    {
        parent->children->start = child;
        return;
    }
    struct NodeAST *temp = parent->children->start;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = child;
}


/*
 * Funcion: Crear un nodo y agregarlo como hijo de otro nodo
 * @param type: Tipo de nodo
 * @param lexema: Lexema del nodo a crear
 * @param parent: Nodo padre
 * @return new_node: Nodo creado
*/
struct NodeAST *create_and_add_node(enum ASTNodeType type, char *lexema, struct NodeAST *parent)
{
    struct NodeAST *new_node = create_ast_node(type, lexema);
    new_node->parent = parent;
    add_child(parent, new_node);
    return new_node;
}

/*
 * Funcion: Imprimir el arbol AST
 * @param root: Nodo raiz del arbol
 * @param depth: Profundidad actual del nodo
*/
void print_AST_tree(struct NodeAST *root, int depth)
{
    if (root == NULL)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    printf("|___%s\n", root->lexema);

    if (root->children != NULL)
    {
        struct NodeAST *current = root->children->start;
        while (current != NULL)
        {
            // Llama a print_AST_tree con depth + 1 para los hijos
            print_AST_tree(current, depth + 1);
            current = current->next;
        }
    }
}