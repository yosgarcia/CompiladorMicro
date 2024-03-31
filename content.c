#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "content.h"


/*
 * Funcion: Crear una lista doblemente enlazada
 * @return new_list: Lista creada   
*/
struct double_linked_list *create_double_linked_list()
{
    struct double_linked_list *new_list = calloc(1, sizeof(struct double_linked_list));
    new_list->start = NULL;
    return new_list;
}

/*
 * Función: Agregar un nodo a la lista doblemente enlazada
 * @param list: Lista a la que se le agregará el nodo
 * @param content: Contenido del nodo
 * @return list: Lista con el nodo agregado
*/
struct double_linked_list *add_node_to_list(struct double_linked_list *list, struct content *content)
{

    struct double_node *temp = list->start;

    struct double_node *new_node = calloc(1, sizeof(struct double_node));
    new_node->content = content;
    if (list == NULL)
    {
        return NULL;
    }
    if (list->start == NULL)
    {
        list->start = new_node;
        return list;
    }

    struct double_node *current = list->start;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    return list;
}


/*
 * Función: Eliminar el primer nodo de la lista doblemente enlazada
 * @param list: Lista a la que se le eliminara el nodo
 * @return list: Lista con el nodo eliminado
*/
struct double_linked_list *remove_start_list(struct double_linked_list *list)
{
    if (list == NULL)
    {
        return NULL;
    }
    if (list->start == NULL)
    {
        return list;
    }
    if (!list->start->next)
    {
        free(list->start);
        list->start = NULL;
        return list;
    }

    struct double_node *temp = list->start;
    list->start->next->prev = NULL;
    list->start = list->start->next;
    free(temp);
    return list;
}


/*
 * Función: Crear un nodo de contenido
 * @param token: Tipo token del nodo
 * @param lexema: Lexema del nodo
 * @return new_content: Nodo de contenido creado
*/
struct content *create_content(enum TokenType token, char *lexema)
{
    struct content *new_content = calloc(1, sizeof(struct content));

    new_content->token = token;
    new_content->lexema = strdup(lexema);

    return new_content;
}