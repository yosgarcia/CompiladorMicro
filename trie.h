#ifndef trie_h
#define trie_h
#include <stdio.h>
#include <stdlib.h>


// Enumeración de los tipos de tokens
enum TokenType
{
    BEGIN_SYM,
    END_SYM,
    READ_SYM,
    WRITE_SYM,
    ID,
    INT_LITERAL,
    LPAREN,
    RPAREN,
    SEMI_COLON,
    COMMA,
    ASSIGN_OP,
    PLUS_OP,
    MINUS_OP,
    EOF_SYM,
    NULL_TOKEN,
    ERROR_LEX
};

// Estructura de un nodo del árbol trie
struct trie_node
{
    char letter;
    struct linked_list *children;
    struct trie_node *next;
    int is_end_of_word;
    enum TokenType tokenType;
};

// Estructura de la lista de hijos de un nodo
struct linked_list
{
    struct trie_node *start;
};

// Estructura de la tabla de símbolos
struct symbol_table
{
    struct trie_node *root;
    int max_i; // Cantidad de símbolos ingresados
};

/*
 * Función: Crear los nodos del arbol trie
 * @param letter: Letra que contiene el nodo
 * @return new_node: Nodo creado
 */
struct trie_node *create_trie_node(char letter);

/*
 * Función: Crear la lista los hijos de un nodo
 * @return new_list: Lista de hijos creada
 */
struct linked_list *create_linked_list();

/*
 * Función: Buscar un nodo en la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a buscar
 * @return current: Nodo encontrado en la lista
 */
struct trie_node *find_letter_on_list(struct linked_list *list, char letter);

/*
 * Función: Agregar un nodo a la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a agregar
 * @return new_node: Nodo agregado
 */
struct trie_node *add_letter_to_list(struct linked_list *list, char letter);

/*
 * Función: Insertar una palabra en el árbol trie
 * @param root: Raíz del árbol
 * @param word: Palabra a insertar en el árbol
 * @param tokenType: Tipo de token de la palabra
 */
void insert_word(struct trie_node *root, char *word, enum TokenType tokenType);

/*
 * Funcion: Buscar una palabra en el árbol trie
 * @param root: Raíz del árbol
 * @param word: Palabra a buscar en el árbol
 * @return tokenType: Tipo de token de la palabra
 */
enum TokenType find_word(struct trie_node *root, char *word);

/*
 * Funcion: Imprimir el árbol trie
 * @param root: Raíz del árbol
 * @param level: Nivel actual del árbol
 */
void print_trie(struct trie_node *root, int level);

/*
 * Función: Crear los nodos de la tabla de símbolos
 * @param letter: Letra que contiene el nodo
 * @return new_node: Nodo creado   
*/
struct trie_node *create_symbol_node(char letter);

/*
 * Función: Crear la tabla de símbolos
 * @return new_table: Tabla de símbolos creada
 */
struct symbol_table *create_symbol_table();

/*
 * Funcion: Insertar un símbolo en la tabla de símbolos
 * @param table: Tabla de símbolos
 * @param word: Símbolo a insertar
 * @param num: Número del símbolo
 */
void insert_symbol(struct symbol_table *table, char *word, int num);

/*
 * Funcion: Imprimir la tabla de simbolos
 * @param table: Tabla de símbolos
 * @param level: Palabra a encontra en la tabla
 */
int find_symbol(struct symbol_table *table, char *word);

/*
 * Funcion: Agregar un nodo en la lista de hijos
 * @param list: Lista de hijos
 * @param letter: Letra a insertar
 * @return current: Nodo insertado en la lista
 */
struct trie_node *add_letter_to_table(struct linked_list *list, char letter);

#endif