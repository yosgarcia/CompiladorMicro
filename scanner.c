#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"

#define MAX_WORD_SIZE 32

/*
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

*/

struct content {
    enum TokenType token;
    char* lexema;
};

struct double_node {
    struct content* content;
    struct double_node* next;
    struct double_node* prev;
};

struct double_linked_list {
    struct double_node* start;
};

void expression(FILE* file, struct double_linked_list* tokens);
void plus_op(FILE* file, struct double_linked_list* tokens);
void primary(FILE* file, struct double_linked_list* tokens);
void expression_list(FILE* file ,struct double_linked_list* tokens );
void id_list(FILE* file,struct double_linked_list* tokens);
void statement(FILE* file, struct double_linked_list* tokens);
void statement_list(FILE* file,struct double_linked_list* tokens );
void program(FILE *file,struct double_linked_list* tokens);
void system_goal(FILE* file, struct double_linked_list* tokens);


struct trie_node *root;

struct double_linked_list* create_double_linked_list() {
    struct double_linked_list* new_list = calloc(1, sizeof(struct double_linked_list));
    new_list->start = NULL;
    return new_list;
}

struct double_linked_list* add_node_to_list(struct double_linked_list* list, struct content* content) {
    
    struct double_node* temp= list->start;
    while (temp)
    {
        temp=temp->next;
    }
    
    struct double_node* new_node = calloc(1, sizeof(struct double_node));
    new_node->content = content;
    if (list == NULL) {
        return NULL;
    }
    if (list->start == NULL) {
        list->start = new_node;
        return list;
    }
    
    struct double_node* current = list->start;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    return list;
}

struct double_linked_list* remove_start_list(struct double_linked_list* list) {
    if(list == NULL) {
        return NULL;
    }
    if(list->start == NULL) {
        return list;
    }
    if (!list->start->next)
    {
        free(list->start);
        list->start=NULL;
        return list;
    }
    
    struct double_node* temp = list->start;
    list->start->next->prev = NULL;
    list->start = list->start->next;
    free(temp);
    return list;
}


struct content* create_content(enum TokenType token, char* lexema) {
    struct content* new_content = calloc(1, sizeof(struct content));

    new_content->token = token;
    new_content->lexema = strdup(lexema);
    
    return new_content;
}


char* char_to_string(char c){
    char* str= calloc(2,sizeof(char));
    str[0]=c;
    str[1]='\0';
    return str;
}


struct content* scanner(FILE *file) {
    char current_Char;
    enum TokenType tokenType = NULL_TOKEN;

    while ((current_Char = fgetc(file)) != EOF) {
        switch(current_Char) {
            case ' ':
            case '\t':
            case '\n':
                continue;
            case '(':
                return  create_content(LPAREN, char_to_string(current_Char));
            case ')':
                return  create_content(RPAREN, char_to_string(current_Char));
            case ';':
                return create_content(SEMI_COLON, char_to_string(current_Char));
            case ',':
                return create_content(COMMA, char_to_string(current_Char));
            case '+':
                return create_content(PLUS_OP, char_to_string(current_Char));
            case '-':
                if ((current_Char = fgetc(file)) == '-') {
                    while ((current_Char = fgetc(file)) != EOF && current_Char != '\n');
                    return scanner(file);
                }
                ungetc(current_Char, file);
                return create_content(MINUS_OP, strdup("-"));
            case ':':
                if ((current_Char = fgetc(file)) == '=') {
                    //posible error
                    return create_content(ASSIGN_OP, strdup(":="));
                }
                ungetc(current_Char, file);
                char error[3];
                error[0] = '-';
                error[1] = current_Char;
                error[2] = '\0';
                return create_content(ERROR_LEX, strdup(error));
            default:
                if (isdigit(current_Char)) {
                    char number[MAX_WORD_SIZE];
                    int index = 0;
                    number[index++] = current_Char;

                    while ((current_Char = fgetc(file)) != EOF && isdigit(current_Char)){
                        number[index++] = current_Char;
                    }
                    number[index] = '\0';
                    ungetc(current_Char, file);
                    return create_content(INT_LITERAL, strdup(number));
                } else if (isalpha(current_Char)) {
                    char identifier[MAX_WORD_SIZE];
                    int index = 0;
                    identifier[index++] = current_Char;

                    while ((current_Char = fgetc(file)) != EOF && isalnum(current_Char)) {
                        identifier[index++] = current_Char;
                    }

                    identifier[index] = '\0';
                    ungetc(current_Char, file);
                    enum TokenType token = find_word(root, identifier);
                 
                    return create_content(token, strdup(identifier));
                } else {
                    return create_content(ERROR_LEX, char_to_string(current_Char));
                }
        }
    }
    return create_content(EOF_SYM, "EOF");
}


void syntax_error(enum TokenType token) {
    printf("Syntax Error: Unexpected token %d\n", token);
    exit(1);
}


void next_token(FILE *file, struct double_linked_list* tokens){
    
    add_node_to_list(tokens,scanner(file));

}


void match(enum TokenType expectedToken, struct double_linked_list* tokens)
{
    if (expectedToken==tokens->start->content->token)
    {
        remove_start_list(tokens);
    }
    else{
        syntax_error(tokens->start->content->token);
    }
    
}


void plus_op(FILE* file, struct double_linked_list* tokens){
    next_token(file,tokens);
    switch(tokens->start->content->token){
        case PLUS_OP:
            match(PLUS_OP, tokens);
            break;
        case MINUS_OP:
            match(MINUS_OP, tokens);
            break;
        default:
            syntax_error(tokens->start->content->token);
    }
}


void primary(FILE* file , struct double_linked_list* tokens){
    
    next_token(file,tokens);
    switch(tokens->start->content->token){
        case MINUS_OP:
            match(MINUS_OP, tokens);
            primary(file,tokens);
            break;

        case LPAREN:
            match(LPAREN, tokens);
            expression(file,tokens);
            next_token(file,tokens);
            match(RPAREN, tokens);
            break;

        case ID:
            match(ID, tokens);
            break;

        case INT_LITERAL:
            match(INT_LITERAL, tokens);
            break;

        default:
            syntax_error(tokens->start->content->token);
    }
}


void expression(FILE* file, struct double_linked_list* tokens){

    primary(file,tokens);
    next_token(file,tokens);

    if(tokens->start->content->token == PLUS_OP || tokens->start->content->token == MINUS_OP){
        plus_op(file,tokens);
        primary(file,tokens);
    }
    else{
        return;
    }
}


void expression_list(FILE* file ,struct double_linked_list* tokens ){
    expression(file,tokens);
    while(1){
        next_token(file,tokens);
        if(tokens->start->content->token == COMMA){
            match(COMMA, tokens);
            expression(file,tokens);
        }else{
      
            return;
        }
    }
}


void id_list(FILE* file,struct double_linked_list* tokens){
    next_token(file,tokens);
    match(ID, tokens);
    
    while(1){
        next_token(file,tokens);
        if(tokens->start->content->token == COMMA){
            match(COMMA, tokens);
            next_token(file,tokens);
            match(ID, tokens);
        }else{
            return;
        }
    }
}


void statement(FILE* file, struct double_linked_list* tokens){
    switch(tokens->start->content->token){
        case ID:
            match(ID,tokens);
            next_token(file,tokens);
            match(ASSIGN_OP, tokens);
            expression(file,tokens);
            next_token(file,tokens);
            match(SEMI_COLON, tokens);
            break;

        case READ_SYM:
            match(READ_SYM,tokens);
            next_token(file,tokens);
            match(LPAREN, tokens);
            id_list(file,tokens);
            next_token(file,tokens);
            match(RPAREN, tokens);
            next_token(file,tokens);
            match(SEMI_COLON, tokens);
            break;

        case WRITE_SYM:
            match(WRITE_SYM,tokens);
            next_token(file,tokens);
            match(LPAREN,tokens);
            expression_list(file,tokens);
            next_token(file,tokens);
            match(RPAREN, tokens);
            next_token(file,tokens);
            match(SEMI_COLON,tokens);
            break;

        default:
            syntax_error(tokens->start->content->token);
    }
}


void statement_list(FILE *file,struct double_linked_list* tokens )
{
    next_token(file,tokens);
    statement(file, tokens);
   
    while (1)
    {
        next_token(file,tokens);
        switch(tokens->start->content->token) {
            case ID:
            case READ_SYM:
            case WRITE_SYM:
               statement(file, tokens);
               break;
            default:
                return;
        }
    }
}


void program(FILE *file,struct double_linked_list* tokens)
{
    next_token(file,tokens);
    match(BEGIN_SYM, tokens);
    statement_list(file,tokens);
    next_token(file,tokens);
    match(END_SYM, tokens);
}


void system_goal(FILE* file, struct double_linked_list* tokens){
    program(file,tokens);
    next_token(file,tokens);
    match(EOF_SYM,tokens);
}



int main()
{
    
    root = create_trie_node('\0');
    insert_word(root, "begin", BEGIN_SYM);
    insert_word(root, "end", END_SYM);
    insert_word(root, "read", READ_SYM);
    insert_word(root, "write", WRITE_SYM);
    FILE *file;
    char word[MAX_WORD_SIZE];
    struct content* tokenName;

    // Open the file
    file = fopen("input.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    struct double_linked_list* tokens = create_double_linked_list();
    
    system_goal(file, tokens);
    fclose(file);

    return 0;
}
