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

enum ASTNodeType{
    PROGRAM_AST,
    STATEMENT_LIST_AST,
    STATEMENT_AST,
    EXPRESSION_AST,
    PRIMARY_AST,
    EXPRESSION_LIST_AST,
    ID_LIST_AST,
    PLUS_OP_AST,
    MINUS_OP_AST,
    ASSIGN_OP_AST,
    READ_SYM_AST,
    WRITE_SYM_AST,
    ID_AST,
    INT_LITERAL_AST,
    ADD_OP_AST
};

struct NodeAST{
    struct NodeAST* parent;
    enum ASTNodeType type;
    char* lexema;
    struct children_node_ast* children;
    struct NodeAST* next;
};

struct children_node_ast{
    struct NodeAST* start;
};


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

void expression(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent);
void add_op(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent);
void primary(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent);
void expression_list(FILE* file ,struct double_linked_list* tokens, struct NodeAST* parent);
void id_list(FILE* file,struct double_linked_list* tokens, struct NodeAST* parent);
void statement(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent);
void statement_list(FILE* file,struct double_linked_list* tokens, struct NodeAST* parent);
struct NodeAST* program(FILE *file,struct double_linked_list* tokens);
struct NodeAST* system_goal(FILE* file, struct double_linked_list* tokens);


struct trie_node *root;



struct children_node_ast* create_children_list(){
    struct children_node_ast* new_list = calloc(1, sizeof(struct children_node_ast));
    new_list->start = NULL;
    return new_list;
}

struct NodeAST* create_ast_node(enum ASTNodeType type, char* lexema){
    struct NodeAST* new_node = calloc(1, sizeof(struct NodeAST));
    new_node->parent = NULL;
    new_node->type = type;
    new_node->lexema = lexema;
    new_node->children = create_children_list();
    return new_node;
}

void add_child(struct NodeAST* parent, struct NodeAST* child){
    if(parent->children == NULL){
        parent->children = create_children_list();
        parent->children->start = child;
        return;
    }
    if(parent->children->start == NULL){
        parent->children->start = child;
        return;
    }
    struct NodeAST* temp = parent->children->start;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = child;
}



struct NodeAST* create_and_add_node(enum ASTNodeType type, char* lexema, struct NodeAST* parent){
    struct NodeAST* new_node = create_ast_node(type, lexema);
    new_node->parent = parent;
    add_child(parent, new_node);
    return new_node;
}


void print_AST_tree(struct NodeAST* root, int depth){
    if(root == NULL){
        return;
    }

    // Imprime espacios al principio de la línea para indicar la profundidad del nodo
    for(int i = 0; i < depth; i++){
        printf("  ");
    }

    printf("|___%s\n", root->lexema);

    if(root->children != NULL){
        struct NodeAST* current = root->children->start;
        while(current != NULL){
            // Llama a print_AST_tree con depth + 1 para los hijos
            print_AST_tree(current, depth + 1);
            current = current->next;
        }
    }
}


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


void add_op(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent){
    next_token(file,tokens);
    switch(tokens->start->content->token){
        case PLUS_OP:
            create_and_add_node(PLUS_OP_AST, "+", parent);
            match(PLUS_OP, tokens);
            break;

        case MINUS_OP:
            create_and_add_node(MINUS_OP_AST, "-", parent);
            match(MINUS_OP, tokens);
            break;
        default:
            syntax_error(tokens->start->content->token);
    }
}


void primary(FILE* file , struct double_linked_list* tokens, struct NodeAST* parent){
    
    next_token(file,tokens);
    switch(tokens->start->content->token){
        case MINUS_OP:
            create_and_add_node(MINUS_OP_AST, "-", parent);
            match(MINUS_OP, tokens);
            struct NodeAST* primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
            primary(file,tokens, primary_node);
            break;

        case LPAREN:
            match(LPAREN, tokens);
            struct NodeAST* expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
            expression(file,tokens, expression_node);
            next_token(file,tokens);
            match(RPAREN, tokens);
            break;

        case ID:
            create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
            match(ID, tokens);
            break;

        case INT_LITERAL:
            create_and_add_node(INT_LITERAL_AST, tokens->start->content->lexema, parent);
            match(INT_LITERAL, tokens);
            break;

        default:
            syntax_error(tokens->start->content->token);
    }
}


void expression(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent){
    struct NodeAST* new_primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
    primary(file,tokens, new_primary_node);
    next_token(file,tokens);

    if(tokens->start->content->token == PLUS_OP || tokens->start->content->token == MINUS_OP){
        struct NodeAST* add_node = create_and_add_node(ADD_OP_AST, "ADD_OP", parent);
        add_op(file, tokens, add_node);

        struct NodeAST* primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
        primary(file, tokens, primary_node);
    }
    else{
        return;
    }
}


void expression_list(FILE* file ,struct double_linked_list* tokens, struct NodeAST* parent){
    struct NodeAST* expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
    expression(file,tokens, expression_node);
    while(1){
        next_token(file,tokens);
        if(tokens->start->content->token == COMMA){
            match(COMMA, tokens);
            struct NodeAST* new_expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
            expression(file,tokens, new_expression_node);
        }else{
      
            return;
        }
    }
}


void id_list(FILE* file,struct double_linked_list* tokens, struct NodeAST* parent){
    next_token(file,tokens);
    create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
    match(ID, tokens);
    while(1){
        next_token(file,tokens);
        if(tokens->start->content->token == COMMA){
            match(COMMA, tokens);
            next_token(file,tokens);
            create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
            match(ID, tokens);
            
        }else{
            return;
        }
    }
}


void statement(FILE* file, struct double_linked_list* tokens, struct NodeAST* parent){
    switch(tokens->start->content->token){
        case ID:
        // Falta validar de que el ID ya este definido previamente
            create_and_add_node(ID_AST, tokens->start->content->lexema, parent); 
            match(ID,tokens);
            
            next_token(file,tokens);
            create_and_add_node(ASSIGN_OP_AST, ":=", parent); // Puede que se pueda quitar
            match(ASSIGN_OP, tokens);
            
            struct NodeAST* expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
            expression(file,tokens, expression_node);
            next_token(file,tokens);
            match(SEMI_COLON, tokens);
            break;

        case READ_SYM:
            match(READ_SYM,tokens);
            create_and_add_node(READ_SYM_AST, "read", parent);
            next_token(file,tokens);
            match(LPAREN, tokens);

            struct NodeAST* id_list_node = create_and_add_node(ID_LIST_AST, "ID_LIST", parent);
            id_list(file,tokens, id_list_node);
            next_token(file,tokens);

            match(RPAREN, tokens);
            next_token(file,tokens);
            match(SEMI_COLON, tokens);
            break;

        case WRITE_SYM:

            create_and_add_node(WRITE_SYM_AST, "write", parent);
            match(WRITE_SYM,tokens);
            next_token(file,tokens);
            match(LPAREN,tokens);
            struct NodeAST* expression_list_node = create_and_add_node(EXPRESSION_LIST_AST, "EXPRESSION_LIST", parent);
            expression_list(file,tokens, expression_list_node);
            next_token(file,tokens);
            match(RPAREN, tokens);
            next_token(file,tokens);
            match(SEMI_COLON,tokens);
            break;

        default:
            syntax_error(tokens->start->content->token);
    }
}


void statement_list(FILE *file,struct double_linked_list* tokens, struct NodeAST* parent)
{
    next_token(file,tokens);
    struct NodeAST* statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
    statement(file, tokens, statement_node);
    
    while (1)
    {
        next_token(file,tokens);
        switch(tokens->start->content->token) {
            case ID:
                struct NodeAST* id_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
                statement(file, tokens, id_statement_node);
                break;
            case READ_SYM:
                struct NodeAST* read_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
                statement(file, tokens, read_statement_node);
                break;
            case WRITE_SYM:
                struct NodeAST* write_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
                statement(file, tokens, write_statement_node);
                break;
            default:
                return;
        }
    }
}


struct NodeAST* program(FILE *file,struct double_linked_list* tokens)
{
    next_token(file,tokens);
    match(BEGIN_SYM, tokens);
    struct NodeAST* root = create_ast_node(PROGRAM_AST, "program");
    statement_list(file,tokens, root);
    print_AST_tree(root,0);
    next_token(file,tokens);
    match(END_SYM, tokens);
    return root;
}


struct NodeAST* system_goal(FILE* file, struct double_linked_list* tokens){
     struct NodeAST* root = program(file,tokens);
    next_token(file,tokens);
    match(EOF_SYM,tokens);
    return root;
}

void translator (
    struct NodeAST* ast_Tree)

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

    
    file = fopen("input.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    struct double_linked_list* tokens = create_double_linked_list();
    
    struct NodeAST* ast_Tree= system_goal(file, tokens);
    fclose(file);



    return 0;
}
