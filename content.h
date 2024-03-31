#ifndef content_h
#define content_h
#include "trie.h"


// Estrucutra del contenido
struct content
{
    enum TokenType token; // Tipo del token leido
    char *lexema; // valor del token
};

// Estructura de un nodo de la lista doblemente enlazada
struct double_node
{
    struct content *content;
    struct double_node *next;
    struct double_node *prev;
};

// Estructura de la lista doblemente enlazada
struct double_linked_list
{
    struct double_node *start;
};

struct double_linked_list *create_double_linked_list();

struct double_linked_list *add_node_to_list(struct double_linked_list *list, struct content *content);

struct double_linked_list *remove_start_list(struct double_linked_list *list);

struct content *create_content(enum TokenType token, char *lexema);


#endif