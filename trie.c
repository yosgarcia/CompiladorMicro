#include <stdio.h>
#include <stdlib.h>
#include "trie.h"



/*
 * Funcion: create_trie_node
 * Se encarga de crear un nuevo nodo del trie,
 * se pide memoria para el struct y se inicializan
 * los valores de los atributos. 
 */
struct trie_node* create_trie_node(char letter){
    struct trie_node* new_node = calloc(1, sizeof(struct trie_node));
    new_node->letter = letter;
    new_node->children = NULL;
    new_node->next = NULL;
    new_node->is_end_of_word = 0;
    new_node->children = NULL;
    //new_node->tokenType = NULL;
    return new_node;
}

struct linked_list* create_linked_list(){
    struct linked_list* new_list = calloc(1, sizeof(struct linked_list));
    new_list->start = NULL;
    return new_list;
}

struct trie_node* find_letter_on_list(struct linked_list* list, char letter){
    struct trie_node* current = list->start;
    while(current != NULL){
        if(current->letter == letter){
            return current;
        }
        current = current->next;
    }
    return NULL;
}



struct trie_node* add_letter_to_list(struct linked_list* list, char letter){
    struct trie_node* new_node = create_trie_node(letter);
    // Verifica si la lista esta vacia y agrega el nodo
    if(list->start == NULL){
        list->start = new_node;
        return new_node;
    } else {
        struct trie_node* current = list->start;
        // Inserta el nodo en la ultima posicion
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return new_node;
    }
}


void insert_word(struct trie_node* root, char* word){
    struct trie_node* current = root;
    for(int i = 0; word[i] != '\0'; i++){
        if(current->children == NULL){
            current->children = create_linked_list();
        }
        struct trie_node* letter = find_letter_on_list(current->children, word[i]);
        if(letter == NULL){
            letter = add_letter_to_list(current->children, word[i]);
        }
        current = letter;
    }
    //current->tokenType = tokenType;
    current->is_end_of_word = 1;
}

int find_word(struct trie_node* root, char* word){
    struct trie_node* current = root;
    for(int i = 0; word[i] != '\0'; i++){
        if(current->children == NULL){
            return NULL;
        }
        struct trie_node* letter = find_letter_on_list(current->children, word[i]);
        if(letter == NULL){
            return NULL;
        }
        current = letter;
    }
    return current->is_end_of_word;
}

void print_trie(struct trie_node* root, int level){
    if(root == NULL){
        return;
    }
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    if (level > 0) {
        printf("|-");
    }
    printf("%c\n", root->letter);
    if(root->children != NULL) {
        struct trie_node* current = root->children->start;
        while(current != NULL){
            print_trie(current, level + 1);
            current = current->next;
        }
    }
}

/*
int main(){
    struct trie_node* root = create_trie_node('\0');
    insert_word(root, "hello");
    insert_word(root, "world");
    insert_word(root, "helid");
    insert_word(root, "hel");
    insert_word(root, "word");
    insert_word(root, "wor");
    insert_word(root, "and");
    insert_word(root, "ant");
    insert_word(root, "begin");
    insert_word(root, "end");
    insert_word(root, "read");
    insert_word(root, "write");
    insert_word(root, "beginner");
    insert_word(root, "beg");
    printf("%d\n", find_word(root, "beg"));
    print_trie(root, 0);
    return 0;
}
*/