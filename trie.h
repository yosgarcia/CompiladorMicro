#ifndef trie_h
#define trie_h
#include <stdio.h>
#include <stdlib.h>

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

struct trie_node {
    char letter;
    struct linked_list* children;
    struct trie_node* next;
    int is_end_of_word;
    enum TokenType tokenType;
};


struct linked_list {
    struct trie_node* start;
};


struct trie_node* create_trie_node(char letter);

struct linked_list* create_linked_list();

struct trie_node* find_letter_on_list(struct linked_list* list, char letter);

struct trie_node* add_letter_to_list(struct linked_list* list, char letter);

void insert_word(struct trie_node* root, char* word, enum TokenType tokenType);

enum TokenType find_word(struct trie_node* root, char* word);

void print_trie(struct trie_node* root, int level);


#endif