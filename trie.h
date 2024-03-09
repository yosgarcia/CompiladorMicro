#ifndef trie_h
#define trie_h
#include <stdio.h>
#include <stdlib.h>



struct trie_node {
    char letter;
    struct linked_list* children;
    struct trie_node* next;
    int is_end_of_word;
    //enum TokenType tokenType;
};


struct linked_list {
    struct trie_node* start;
};


struct trie_node* create_trie_node(char letter);

struct linked_list* create_linked_list();

struct trie_node* find_letter_on_list(struct linked_list* list, char letter);

struct trie_node* add_letter_to_list(struct linked_list* list, char letter);

void insert_word(struct trie_node* root, char* word);

int find_word(struct trie_node* root, char* word);

void print_trie(struct trie_node* root, int level);


#endif