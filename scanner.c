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



struct double_linked_list* create_double_linked_list() {
    struct double_linked_list* new_list = calloc(1, sizeof(struct double_linked_list));
    new_list->start = NULL;
    return new_list;
}

struct double_linked_list* add_node_to_list(struct double_linked_list* list, struct content* content) {
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
    struct double_node* temp = list->start;
    list->start->next->prev = NULL;
    list->start = list->start->next;
    free(temp);
    return list;

}

struct content* create_content(enum TokenType token, char* lexema) {
    struct content* new_content = calloc(1, sizeof(struct content));
    new_content->token = token;
    new_content->lexema = lexema;
    return new_content;
}


struct trie_node *root;




enum TokenType scanner(FILE *file)
{
    char current_Char;
    enum TokenType tokenType = NULL_TOKEN;

    while ((current_Char = fgetc(file)) != EOF)
    {
        if (current_Char == ' ' || current_Char == '\t' || current_Char == '\n')
        {
            
            continue;
        }
        // Verificaciòn de si es -- o comment pues
        // Se fija si es alphanumerico, osea palabra
        else if (isalpha(current_Char))
        {
            char identifier[MAX_WORD_SIZE];
            int index = 0;
            identifier[index++] = current_Char;

            // Lee las letras contiguas

            while ((current_Char = fgetc(file)) != EOF && (isalnum(current_Char)))
            {
                identifier[index++] = current_Char;
            }
            // creo que esta tambien ocupa el index++, not sure. Aunque en teoria este
            //  codigo solo reconoce el tipo, tons no hace nada con eso
            //  por ende tampoco importa xddddd
            identifier[index] = '\0';
            ungetc(current_Char, file);
            
            return find_word(root, identifier);
        }
        // Ver si empieza con digito
        else if (isdigit(current_Char))
        {
            // Leer los digitos continuos
            while ((current_Char = fgetc(file)) != EOF && (isdigit(current_Char)))
            {
                // Avanzar en la palabra
            }
            ungetc(current_Char, file);
            return INT_LITERAL;
        }
        else if (current_Char == '(')
        {
            return LPAREN;
        }
        else if (current_Char == ')')
        {
            return RPAREN;
        }
        else if (current_Char == ';')
        {
            return SEMI_COLON;
        }
        else if (current_Char == ',')
        {
            return COMMA;
        }
        else if (current_Char == '+')
        {
            return PLUS_OP;
        }
        else if (current_Char == ':')
        {
            if ((current_Char = fgetc(file)) == '=')
            {
                return ASSIGN_OP;
            }
            ungetc(current_Char, file);

            return ERROR_LEX;
        }
        else if (current_Char == '-')
        {
            if ((current_Char = fgetc(file)) == '-')
            {
                while ((current_Char = fgetc(file)) != EOF && current_Char != '\n')
                    ;
                return scanner(file);
            }
            ungetc(current_Char, file);

            return MINUS_OP;
        }

        else
        {
            return ERROR_LEX;
        }
    }
    return tokenType;
}



struct content* scanner_v2(FILE *file) {
    char current_Char;
    enum TokenType tokenType = NULL_TOKEN;

    while ((current_Char = fgetc(file)) != EOF) {
        printf("Current char %c \n", current_Char);
        switch(current_Char) {
            case ' ':
            case '\t':
            case '\n':
                continue;
            case '(':
                return  create_content(LPAREN, current_Char);
            case ')':
                return  create_content(RPAREN, current_Char);
            case ';':
                return create_content(SEMI_COLON, current_Char);
            case ',':
                return create_content(COMMA, current_Char);
            case '+':
                return create_content(PLUS_OP, current_Char);
            case '-':
                if ((current_Char = fgetc(file)) == '-') {
                    while ((current_Char = fgetc(file)) != EOF && current_Char != '\n');
                    return scanner_v2(file);
                }
                ungetc(current_Char, file);
                return create_content(MINUS_OP, current_Char);
            case ':':
                if ((current_Char = fgetc(file)) == '=') {
                    return create_content(ASSIGN_OP, ":=");
                }
                
                ungetc(current_Char, file);
                char error[3];
                error[0] = '-';
                error[1] = current_Char;
                error[2] = '\0';
                return create_content(ERROR_LEX, error);
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
                    return create_content(INT_LITERAL, number);
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
                    return create_content(token, identifier);
                } else {
                    return create_content(ERROR_LEX, current_Char);
                }
        }
    }
    return create_content(EOF_SYM, "EOF");
}

void syntax_error(enum TokenType token) {
    printf("Syntax Error: Unexpected token %d\n", token);
    exit(1);
}

void match(enum TokenType expectedToken, FILE *file)
{
    struct content* token = scanner_v2(file);
    printf(" token: %d\n", token->token);
    if (token->token == expectedToken)
    {
        printf("Matched token: %d\n", token->token);
    }
    else
    {
        printf("Match function else\n");
       // printf("THis is the lexema %c \n ", token->lexema);
        syntax_error(token->token);
    }
}

void systema_goal(FILE *file)
{
    program(file);
    match(EOF_SYM, file);
}

void program(FILE *file)
{
    match(BEGIN_SYM, file);
    statement_list(file);
    match(END_SYM, file);
}


void statement_list(FILE *file)
{
    struct content* token = scanner_v2(file);
    
    statement(file, token);
    
    while (1)
    {
        struct content* token = scanner_v2(file);
        printf("Statement list %s \n",token->lexema);
        switch(token->token) {
            case ID:
            case READ_SYM:
            case WRITE_SYM:
               statement(file, token);
               break;
                
            default:
            // REEMPLAZAR POR CUSTOM UNGET
            printf("Unget in while statement list\n %s the lexema\n",token->lexema);
                ungetc(token->lexema, file);
                return;
        }
    }
}


void statement(FILE* file, struct content* token){
    //struct content* token = scanner_v2(file);
    printf("statement token id: %d\n", token->token);
    
     printf("statement token lexema: %s\n", token->lexema);

    switch(token->token){
        case ID:
            match(ASSIGN_OP, file);
            expression(file);
            match(SEMI_COLON, file);
            break;
        case READ_SYM:
            printf("hola");
            match(LPAREN, file);
            id_list(file);
            printf("holiis");
            match(RPAREN, file);
            match(SEMI_COLON, file);
            break;
        case WRITE_SYM:
            match(LPAREN, file);
            expression_list(file);
            match(RPAREN, file);
            match(SEMI_COLON, file);
            break;
        default:
            printf("%s is the lexema \n",token->lexema);
            
            syntax_error(token->token);
    }
}

void id_list(FILE* file){
    match(ID, file);
    printf("Matchee un id en id list\n");
    // verificar si el loop debe tener otra condicion
    while(1){
        struct content* token = scanner_v2(file);
        if(token->token == COMMA){
            //match(COMMA, file);
            match(ID, file);
        }else{
            printf("Aquiiii mi token %d\n", token->token);
            printf("Prueba de ungetc %c\n", token->lexema);
            ungetc(token->lexema, file);

            return;
        }
    }
}

void expression_list(FILE* file){
    expression(file);
    while(1){
        struct content* token = scanner_v2(file);
        if(token->token == COMMA){
            match(COMMA, file);
            expression(file);
        }else{
            ungetc(token->lexema, file);
            return;
        }
    }
}


void expression(FILE* file){
    printf("Expression function \n");
    primary(file);
    struct content* token = scanner_v2(file);
    if(token->token == PLUS_OP){
        plus_op(file);
        primary(file);
    }
    else{
        //cambiar por custom unget
        ungetc(token->lexema, file);
        return;
    }
}

void primary(FILE* file){
    printf("Primary function \n");
    struct content* token = scanner_v2(file);
    switch(token->token){
        case MINUS_OP:
            //match(MINUS_OP, file);
            primary(file);
            break;
        case LPAREN:
            //match(LPAREN, file);
            expression(file);
            match(RPAREN, file);
            break;
        case ID:
            //match(ID, file);
            break;
        case INT_LITERAL:
            //match(INT_LITERAL, file);
            break;
        default:
            syntax_error(token->token);
    }
}


void plus_op(FILE* file){
    struct content* token = scanner_v2(file);
    switch(token->token){
        case PLUS_OP:
            match(PLUS_OP, file);
            break;
        case MINUS_OP:
            match(MINUS_OP, file);
            break;
        default:
            syntax_error(token->token);
    }
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

    systema_goal(file);

    while ((tokenName = scanner_v2(file))->token != NULL_TOKEN)
    {
        switch (tokenName->token)
        {
        case INT_LITERAL:
            printf("INT\n");
            break;
        case ID:
            printf("ID\n");
            break;
        case SEMI_COLON:
            printf("Semi Colon\n");
            break;
        case BEGIN_SYM:
            printf("BEGIN_SYM\n");
            break;
        case END_SYM:
            printf("END_SYM\n");
            break;
        case READ_SYM:
            printf("READ_SYM\n");
            break;
        case WRITE_SYM:
            printf("WRITE_SYM\n");
            break;
        case LPAREN:
            printf("LPAREN\n");
            break;
        case RPAREN:
            printf("RPAREN\n");
            break;
        case ASSIGN_OP:
            printf("ASSIGN_OP\n");
            break;
        case PLUS_OP:
            printf("PLUS_OP\n");
            break;
        case MINUS_OP:
            printf("MINUS_OP\n");
            break;
        case EOF_SYM:
            printf("EOF_SYM\n");
            break;
        case NULL_TOKEN:
            printf("NULL_TOKEN\n");
            break;
        case ERROR_LEX:
            printf("ERROR_LEX\n");
            break;
        case COMMA:
            printf("COMMA\n");
            break;
        default:
            printf("Others\n");
            break;
            
        }
    }
    /*
    struct derivation_tree *program = create_derivation_tree(BEGIN_SYM, NULL);
    struct derivation_tree *current_node = program;
    while ((current_node->right = parse_statement(file)) != NULL) {
        current_node = current_node->right;
    }
    current_node->right = create_derivation_tree(END_SYM, NULL);

    // Print derivation tree
    printf("Derivation Tree:\n");
    print_derivation_tree(program, 0);
    */

    // Close the file
    fclose(file);

    return 0;
}
