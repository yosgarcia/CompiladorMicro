#include <stdio.h>
#include <stdlib.h>
#include "trie.h"

/*
 * Funcion: Crear los nodos del arbol trie
 * @param letter: Letra que contiene el nodo
 * @return new_node: Nodo creado
 */
struct trie_node *create_trie_node(char letter)
{
    struct trie_node *new_node = calloc(1, sizeof(struct trie_node));
    new_node->letter = letter;
    new_node->children = NULL;
    new_node->next = NULL;
    new_node->is_end_of_word = 0;
    new_node->tokenType = ID;
    return new_node;
}

/*
 * Funcion: Crear la lista los hijos de un nodo
 * @return new_list: Lista de hijos creada
 */
struct linked_list *create_linked_list()
{
    struct linked_list *new_list = calloc(1, sizeof(struct linked_list));
    new_list->start = NULL;
    return new_list;
}


/*
 * Funcion: Buscar un nodo en la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a buscar
 * @return current: Nodo encontrado en la lista
 */
struct trie_node *find_letter_on_list(struct linked_list *list, char letter)
{
    struct trie_node *current = list->start;
    while (current != NULL)
    {
        if (current->letter == letter)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


/*
 * Funcion: Agregar un nodo a la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a agregar
 * @return new_node: Nodo agregado
 */
struct trie_node *add_letter_to_list(struct linked_list *list, char letter)
{
    struct trie_node *new_node = create_trie_node(letter);
    if (list->start == NULL)
    {
        list->start = new_node;
        return new_node;
    }
    else
    {
        struct trie_node *current = list->start;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
        return new_node;
    }
}


/*
 * Funcion: Insertar una palabra en el arbol trie
 * @param root: Raiz del arbol
 * @param word: Palabra a insertar en el arbol
 * @param tokenType: Tipo de token de la palabra
 */
void insert_word(struct trie_node *root, char *word, enum TokenType tokenType)
{
    struct trie_node *current = root;
    // Iteramos por cada una de las letras de la palabra
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (current->children == NULL)
        {
            // Si no tiene hijos, creamos la lista de hijos
            current->children = create_linked_list();
        }
        // Buscamos la letra en la lista de hijos
        struct trie_node *letter = find_letter_on_list(current->children, word[i]);
        // Si no se encuentra la letra, la agregamos a la lista
        if (letter == NULL)
        {
            letter = add_letter_to_list(current->children, word[i]);
        }
        current = letter;
    }
    // Asignamos el tipo de token a la ultima letra de la palabra
    current->tokenType = tokenType;
    current->is_end_of_word = 1;
}


/*
 * Funcion: Buscar una palabra en el arbol trie
 * @param root: Raiz del arbol
 * @param word: Palabra a buscar en el arbol
 * @return tokenType: Tipo de token de la palabra
 */
enum TokenType find_word(struct trie_node *root, char *word)
{
    struct trie_node *current = root;
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (current->children == NULL)
        {
            return ID;
        }
        struct trie_node *letter = find_letter_on_list(current->children, word[i]);
        if (letter == NULL)
        {
            return ID;
        }
        current = letter;
    }
    return current->tokenType;
}

/*
 * Funcion: Imprimir el arbol trie
 * @param root: Raiz del arbol
 * @param level: Nivel actual del arbol
 */
void print_trie(struct trie_node *root, int level)
{
    if (root == NULL)
    {
        return;
    }
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }
    if (level > 0)
    {
        printf("|-");
    }
    printf("%c\n", root->letter);
    if (root->children != NULL)
    {
        struct trie_node *current = root->children->start;
        while (current != NULL)
        {
            print_trie(current, level + 1); // Llamada recursiva para el siguient nivel
            current = current->next;
        }
    }
}


// <------- Funciones para la tabla de simbolos -------->

/*
 * Funcion: Crear los nodos de la tabla de simbolos
 * @param letter: Letra que contiene el nodo
 * @return new_node: Nodo creado   
*/
struct trie_node *create_symbol_node(char letter)
{
    struct trie_node *new_node = calloc(1, sizeof(struct trie_node));
    new_node->letter = letter;
    new_node->children = NULL;
    new_node->next = NULL;
    new_node->is_end_of_word = -1;
    return new_node;
}

/*
 * Funcion: Crear la tabla de simbolos
 * @return new_table: Tabla de simbolos creada
 */
struct symbol_table *create_symbol_table()
{
    struct symbol_table *new_table = calloc(1, sizeof(struct symbol_table));
    new_table->root = create_symbol_node(' '); // La raiz siempre es vacia
    new_table->max_i = -1;
    return new_table;
}

/*
 * Funcion: Agregar un nodo en la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a insertar
 * @return current: Nodo insertado en la lista
 */
struct trie_node *add_letter_to_table(struct linked_list *list, char letter)
{
    struct trie_node *new_node = create_symbol_node(letter);
    if (list->start == NULL)
    {
        list->start = new_node;
        return new_node;
    }
    else
    {
        struct trie_node *current = list->start;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
        return new_node;
    }
}

/*
 * Funcion: Insertar un simbolo en la tabla de simbolos
 * @param table: Tabla de simbolos
 * @param word: Simbolo a insertar
 * @param num: Numero del simbolo
 */
void insert_symbol(struct symbol_table *table, char *word, int num)
{
    struct trie_node *current = table->root;
    // Iteramos por cada una de las letras del simbolo
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (current->children == NULL)
        {
            current->children = create_linked_list();
        }
        // Buscamos la letra en la lista de hijos
        struct trie_node *letter = find_letter_on_list(current->children, word[i]);
        if (letter == NULL)
        {
            letter = add_letter_to_table(current->children, word[i]);
        }
        current = letter;
    }
    current->is_end_of_word = num;
    table->max_i += 1; // Aumentamos el contador de simbolos que se han insertado
}


/*
 * Funcion: Imprimir la tabla de simbolos
 * @param table: Tabla de simbolos
 * @param level: Palabra a encontra en la tabla
 */
int find_symbol(struct symbol_table *table, char *word)
{
    struct trie_node *current = table->root;
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (current->children == NULL)
        {
            return current->is_end_of_word;
        }
        struct trie_node *letter = find_letter_on_list(current->children, word[i]);
        if (letter == NULL)
        {
            return -1;
        }
        current = letter;
    }

    return current->is_end_of_word;
}