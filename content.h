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

/*
 * Funcion: Crear una lista doblemente enlazada
 * @return new_list: Lista creada   
*/
struct double_linked_list *create_double_linked_list();

/*
 * Función: Agregar un nodo a la lista doblemente enlazada
 * @param list: Lista a la que se le agregará el nodo
 * @param content: Contenido del nodo
 * @return list: Lista con el nodo agregado
*/
struct double_linked_list *add_node_to_list(struct double_linked_list *list, struct content *content);

/*
 * Función: Eliminar el primer nodo de la lista doblemente enlazada
 * @param list: Lista a la que se le eliminara el nodo
 * @return list: Lista con el nodo eliminado
*/
struct double_linked_list *remove_start_list(struct double_linked_list *list);

/*
 * Función: Crear un nodo de contenido
 * @param token: Tipo token del nodo
 * @param lexema: Lexema del nodo
 * @return new_content: Nodo de contenido creado
*/
struct content *create_content(enum TokenType token, char *lexema);


#endif