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

enum ASTNodeType
{
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

struct NodeAST
{
    struct NodeAST *parent;
    enum ASTNodeType type;
    char *lexema;
    struct children_node_ast *children;
    struct NodeAST *next;
};

struct children_node_ast
{
    struct NodeAST *start;
};

enum SemanticToken
{
    LITERAL_SEMANTIC,
    ID_SEMANTIC,
    TEMP_RESULT_SEMANTIC,
    EXPRESSION_RESULT_SEMANTIC
};
struct SemanticRecord
{
    enum SemanticToken semantic_record;
    int semantic_info;
};
struct content
{
    enum TokenType token;
    char *lexema;
};

struct double_node
{
    struct content *content;
    struct double_node *next;
    struct double_node *prev;
};

struct double_linked_list
{
    struct double_node *start;
};

struct SemanticRecord *process_expression(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols);
struct SemanticRecord *genInfix(struct SemanticRecord *first_record, enum ASTNodeType operation_sign, struct SemanticRecord *second_record, FILE *file, struct symbol_table *symbols);
void expression(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void add_op(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void primary(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void expression_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void id_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void statement(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
void statement_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
struct NodeAST *program(FILE *file, struct double_linked_list *tokens);
struct NodeAST *system_goal(FILE *file, struct double_linked_list *tokens);
int process_id(char *var_name, struct symbol_table *symbols);
struct SemanticRecord *create_record(enum SemanticToken, int semantic_info);
struct trie_node *root;

struct SemanticRecord *create_record(enum SemanticToken token, int semantic_info)
{
    struct SemanticRecord *new_record = calloc(1, sizeof(struct SemanticRecord));
    new_record->semantic_record = token;
    new_record->semantic_info = semantic_info;
    return new_record;
}

struct children_node_ast *create_children_list()
{
    struct children_node_ast *new_list = calloc(1, sizeof(struct children_node_ast));
    new_list->start = NULL;
    return new_list;
}

struct NodeAST *create_ast_node(enum ASTNodeType type, char *lexema)
{
    struct NodeAST *new_node = calloc(1, sizeof(struct NodeAST));
    new_node->parent = NULL;
    new_node->type = type;
    new_node->lexema = lexema;
    new_node->children = create_children_list();
    return new_node;
}

void add_child(struct NodeAST *parent, struct NodeAST *child)
{
    if (parent->children == NULL)
    {
        parent->children = create_children_list();
        parent->children->start = child;
        return;
    }
    if (parent->children->start == NULL)
    {
        parent->children->start = child;
        return;
    }
    struct NodeAST *temp = parent->children->start;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = child;
}

struct NodeAST *create_and_add_node(enum ASTNodeType type, char *lexema, struct NodeAST *parent)
{
    struct NodeAST *new_node = create_ast_node(type, lexema);
    new_node->parent = parent;
    add_child(parent, new_node);
    return new_node;
}

void print_AST_tree(struct NodeAST *root, int depth)
{
    if (root == NULL)
    {
        return;
    }

    // Imprime espacios al principio de la línea para indicar la profundidad del nodo
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    printf("|___%s\n", root->lexema);

    if (root->children != NULL)
    {
        struct NodeAST *current = root->children->start;
        while (current != NULL)
        {
            // Llama a print_AST_tree con depth + 1 para los hijos
            print_AST_tree(current, depth + 1);
            current = current->next;
        }
    }
}

struct double_linked_list *create_double_linked_list()
{
    struct double_linked_list *new_list = calloc(1, sizeof(struct double_linked_list));
    new_list->start = NULL;
    return new_list;
}


int get_stack_pos(struct symbol_table *symbols, int id_var)
{
    return (8 * symbols->max_i) - 8 * id_var;
}


struct double_linked_list *add_node_to_list(struct double_linked_list *list, struct content *content)
{

    struct double_node *temp = list->start;
    while (temp)
    {
        temp = temp->next;
    }

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

struct content *create_content(enum TokenType token, char *lexema)
{
    struct content *new_content = calloc(1, sizeof(struct content));

    new_content->token = token;
    new_content->lexema = strdup(lexema);

    return new_content;
}

char *char_to_string(char c)
{
    char *str = calloc(2, sizeof(char));
    str[0] = c;
    str[1] = '\0';
    return str;
}

struct content *scanner(FILE *file)
{
    char current_Char;
    enum TokenType tokenType = NULL_TOKEN;

    while ((current_Char = fgetc(file)) != EOF)
    {
        switch (current_Char)
        {
        case ' ':
        case '\t':
        case '\n':
            continue;
        case '(':
            return create_content(LPAREN, char_to_string(current_Char));
        case ')':
            return create_content(RPAREN, char_to_string(current_Char));
        case ';':
            return create_content(SEMI_COLON, char_to_string(current_Char));
        case ',':
            return create_content(COMMA, char_to_string(current_Char));
        case '+':
            return create_content(PLUS_OP, char_to_string(current_Char));
        case '-':
            if ((current_Char = fgetc(file)) == '-')
            {
                while ((current_Char = fgetc(file)) != EOF && current_Char != '\n')
                    ;
                return scanner(file);
            }
            ungetc(current_Char, file);
            return create_content(MINUS_OP, strdup("-"));
        case ':':
            if ((current_Char = fgetc(file)) == '=')
            {
                // posible error
                return create_content(ASSIGN_OP, strdup(":="));
            }
            ungetc(current_Char, file);
            char error[3];
            error[0] = '-';
            error[1] = current_Char;
            error[2] = '\0';
            return create_content(ERROR_LEX, strdup(error));
        default:
            if (isdigit(current_Char))
            {
                char number[MAX_WORD_SIZE];
                int index = 0;
                number[index++] = current_Char;

                while ((current_Char = fgetc(file)) != EOF && isdigit(current_Char))
                {
                    number[index++] = current_Char;
                }
                number[index] = '\0';
                ungetc(current_Char, file);
                return create_content(INT_LITERAL, strdup(number));
            }
            else if (isalpha(current_Char))
            {
                char identifier[MAX_WORD_SIZE];
                int index = 0;
                identifier[index++] = current_Char;

                while ((current_Char = fgetc(file)) != EOF && (isalnum(current_Char) || current_Char == '_'))
                {
                    identifier[index++] = current_Char;
                }

                identifier[index] = '\0';
                ungetc(current_Char, file);
                enum TokenType token = find_word(root, identifier);

                return create_content(token, strdup(identifier));
            }
            else
            {
                return create_content(ERROR_LEX, char_to_string(current_Char));
            }
        }
    }
    return create_content(EOF_SYM, "EOF");
}

void syntax_error(enum TokenType token)
{
    printf("Syntax Error: Unexpected token %d\n", token);
    exit(1);
}

void next_token(FILE *file, struct double_linked_list *tokens)
{

    add_node_to_list(tokens, scanner(file));
}

void match(enum TokenType expectedToken, struct double_linked_list *tokens)
{
    if (expectedToken == tokens->start->content->token)
    {
        remove_start_list(tokens);
    }
    else
    {
        syntax_error(tokens->start->content->token);
    }
}

void add_op(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    next_token(file, tokens);
    switch (tokens->start->content->token)
    {
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

void primary(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{

    next_token(file, tokens);
    switch (tokens->start->content->token)
    {
    case MINUS_OP:
        create_and_add_node(MINUS_OP_AST, "-", parent);
        match(MINUS_OP, tokens);
        struct NodeAST *primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
        primary(file, tokens, primary_node);
        break;

    case LPAREN:
        match(LPAREN, tokens);
        struct NodeAST *expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
        expression(file, tokens, expression_node);
        next_token(file, tokens);
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

void expression(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    struct NodeAST *new_primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
    primary(file, tokens, new_primary_node);
    next_token(file, tokens);

    if (tokens->start->content->token == PLUS_OP || tokens->start->content->token == MINUS_OP)
    {
        struct NodeAST *add_node = create_and_add_node(ADD_OP_AST, "ADD_OP", parent);
        add_op(file, tokens, add_node);

        struct NodeAST *primary_node = create_and_add_node(PRIMARY_AST, "PRIMARY", parent);
        primary(file, tokens, primary_node);
    }
    else
    {
        return;
    }
}

void expression_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    struct NodeAST *expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
    expression(file, tokens, expression_node);
    while (1)
    {
        next_token(file, tokens);
        if (tokens->start->content->token == COMMA)
        {
            match(COMMA, tokens);
            struct NodeAST *new_expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
            expression(file, tokens, new_expression_node);
        }
        else
        {
            return;
        }
    }
}

void id_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    next_token(file, tokens);
    create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
    match(ID, tokens);
    while (1)
    {
        next_token(file, tokens);
        if (tokens->start->content->token == COMMA)
        {
            match(COMMA, tokens);
            next_token(file, tokens);
            create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
            match(ID, tokens);
        }
        else
        {
            return;
        }
    }
}

void statement(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    switch (tokens->start->content->token)
    {
    case ID:
        create_and_add_node(ID_AST, tokens->start->content->lexema, parent);
        match(ID, tokens);

        next_token(file, tokens);
        create_and_add_node(ASSIGN_OP_AST, ":=", parent);
        match(ASSIGN_OP, tokens);

        struct NodeAST *expression_node = create_and_add_node(EXPRESSION_AST, "EXPRESSION", parent);
        expression(file, tokens, expression_node);
        next_token(file, tokens);
        match(SEMI_COLON, tokens);
        break;

    case READ_SYM:
        match(READ_SYM, tokens);
        create_and_add_node(READ_SYM_AST, "read", parent);
        next_token(file, tokens);
        match(LPAREN, tokens);

        struct NodeAST *id_list_node = create_and_add_node(ID_LIST_AST, "ID_LIST", parent);
        id_list(file, tokens, id_list_node);
        next_token(file, tokens);

        match(RPAREN, tokens);
        next_token(file, tokens);
        match(SEMI_COLON, tokens);
        break;

    case WRITE_SYM:

        create_and_add_node(WRITE_SYM_AST, "write", parent);
        match(WRITE_SYM, tokens);
        next_token(file, tokens);
        match(LPAREN, tokens);
        struct NodeAST *expression_list_node = create_and_add_node(EXPRESSION_LIST_AST, "EXPRESSION_LIST", parent);
        expression_list(file, tokens, expression_list_node);
        next_token(file, tokens);
        match(RPAREN, tokens);
        next_token(file, tokens);
        match(SEMI_COLON, tokens);
        break;

    default:
        syntax_error(tokens->start->content->token);
    }
}

void statement_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent)
{
    next_token(file, tokens);
    struct NodeAST *statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
    statement(file, tokens, statement_node);

    while (1)
    {
        next_token(file, tokens);
        switch (tokens->start->content->token)
        {
        case ID:
            struct NodeAST *id_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
            statement(file, tokens, id_statement_node);
            break;
        case READ_SYM:
            struct NodeAST *read_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
            statement(file, tokens, read_statement_node);
            break;
        case WRITE_SYM:
            struct NodeAST *write_statement_node = create_and_add_node(STATEMENT_AST, "STATEMENT", parent);
            statement(file, tokens, write_statement_node);
            break;
        default:
            return;
        }
    }
}

struct NodeAST *program(FILE *file, struct double_linked_list *tokens)
{
    next_token(file, tokens);
    match(BEGIN_SYM, tokens);
    struct NodeAST *root = create_ast_node(PROGRAM_AST, "program");
    statement_list(file, tokens, root);
    print_AST_tree(root, 0);
    next_token(file, tokens);
    match(END_SYM, tokens);
    return root;
}

struct NodeAST *system_goal(FILE *file, struct double_linked_list *tokens)
{
    struct NodeAST *root = program(file, tokens);
    next_token(file, tokens);
    match(EOF_SYM, tokens);
    return root;
}
int get_temp_int()
{
    static int counter = -1;
    return ++counter;
}

void gen_prefix(FILE *file_asm)
{
    const char *prefix =
        "%macro change 2\n"
        "   mov edi, %2\n"
        "   mov rax, %1\n"
        "   mov ebx, 10\n"
        "   xor ecx, ecx\n"
        "   call repeat\n"
        "   mov byte [edi], 0  \n"
        "%endmacro\n"
        "%macro imprimeEnPantalla 2\n"
        "   mov     eax, 4\n"
        "   mov     ebx, 1\n"
        "   mov     ecx, %1\n"
        "   mov     edx, %2\n"
        "   int     0x80\n"
        "%endmacro\n"
        "%macro leeTeclado 0\n"
        "   mov     eax,     3\n"
        "   mov     ebx,     0\n"
        "   mov     ecx,     entrada\n"
        "   mov     edx,     100\n"
        "   int     0x80\n"
        "%endmacro\n"
        "%macro suma 2\n"
        "   mov rax, %1\n"
        "   add rax, %2\n"
        "%endmacro\n"
        "%macro resta 2\n"
        "   mov rax, %1\n"
        "   sub rax, %2\n"
        "%endmacro\n"
        "%macro jumpline 0\n"
        "   mov eax, 4\n"
        "   mov ebx, 1\n"
        "   mov ecx, newline\n"
        "   mov edx, lngnewline\n"
        "   int 0x80\n"
        "%endmacro\n";
    const char *prefix2 = "section .bss\n"
                          " numero resb 22\n"
                          " entrada resb 100\n"
                          "section .data\n"
                          " msgError db \"Not a number\", 0\n"
                          " msgErrorlen equ $ - msgError\n"
                          " msg db \"-\", 0\n"
                          " msglen equ $ - msg\n"
                          " newline db 10\n"
                          " lngnewline equ $-newline\n"
                          "section .text\n"
                          "global _start\n"
                          "process_entrada:\n"
                          " mov rbx, 0\n"
                          " mov rdx,0\n"
                          " mov rax,0\n"
                          " mov rcx,0\n"
                          " mov r9,0\n"
                          " mov r8,0\n"
                          " mov al,[esi]\n"
                          " cmp al,45\n"
                          " je entrada_negativa\n"
                          " mov rax, 66\n"
                          " push rax\n";
    const char *prefix3 =
        "   jmp iterate_entrada\n"
        "entrada_negativa:\n"
        "   mov rax, 67\n"
        "   push rax\n"
        "   inc esi\n"
        "   iterate_entrada:\n"
        "   mov rax,0\n"
        "   mov al, [esi]\n"
        "   test al,al\n"
        "   jz endloop\n"
        "   cmp al,10\n"
        "   je endloop\n"
        "   cmp al,48\n"
        "   jl imprimir_error_entrada\n"
        "   cmp al, 57\n"
        "   jg imprimir_error_entrada\n"
        "   sub rax, 48\n";
    const char *prefix4 =
        "   push rax \n"
        "   mov rax, rcx \n"
        "   mov rbx,10 \n"
        "   mul rbx\n"
        "   mov rcx, rax\n"
        "   pop rax \n"
        "   add rcx, rax\n"
        "   inc esi\n"
        "   jmp iterate_entrada\n"
        "endloop:\n"
        "   pop rax\n"
        "   cmp rax,67\n"
        "   jne no_negativo\n"
        "   mov rax, 0\n"
        "   sub rax, rcx\n"
        "   mov rcx, rax\n"
        "no_negativo:\n"
        "   ret\n"
        "imprimir_error_entrada:\n"
        "   imprimeEnPantalla msgError,msgErrorlen\n"
        "   jumpline\n"
        "   jmp exit\n"
        "print_number:\n"
        "   cmp rax, 0\n"
        "   jl negative\n"
        "   change rax, numero\n"
        "   call print\n"
        "   ret\n"
        "negative:\n"
        "   push rax\n"
        "   mov eax, 4\n"
        "   mov ebx, 1\n"
        "   mov ecx, msg\n"
        "   mov edx, msglen\n"
        "   int 0x80\n"
        "   pop rax\n"
        "   mov ebx, 0\n"
        "   sub rbx, rax\n"
        "   mov rax, rbx\n"
        "   change rbx, numero\n"
        "   call print\n"
        "   ret\n"
        "print:\n"
        "   mov eax, 4\n"
        "   mov ebx, 1\n"
        "   mov ecx, numero\n"
        "   mov edx, 22\n"
        "   int 0x80\n"
        "   jumpline\n"
        "   ret\n"
        "repeat:\n"
        "   xor edx, edx\n"
        "   div ebx\n"
        "   add dl, '0'\n"
        "   push dx\n"
        "   inc ecx\n"
        "   test rax, rax\n"
        "   jnz repeat\n"
        "   jmp pop_chars\n"
        "pop_chars:\n"
        "   pop dx\n"
        "   mov [edi], dl\n"
        "   inc edi\n"
        "   loop pop_chars\n"
        "   ret\n"
        "_start:\n";

    fputs(prefix, file_asm);

    fputs(prefix2, file_asm);

    fputs(prefix3, file_asm);
    fputs(prefix4, file_asm);
}


void gen_sufix(FILE *file_asm)
{
    const char *sufix =
        "exit:\n "
        "   mov eax,1\n"
        "   mov ebx, 0\n"
        "   int 80h\n";

    fputs(sufix, file_asm);
    fclose(file_asm);
    printf("ASM Code generated correctly :D \n");
}


const char *create_var_from_literal_string = "  mov rax, %d \n   push rax\n";
char *create_var_from_literal(int literal)
{
    int length = snprintf(NULL, 0, create_var_from_literal_string, literal) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {
        return NULL;
    }
    snprintf(formatted_string, length, create_var_from_literal_string, literal);

    return formatted_string;
}


const char *create_var_from_stack_string = "    mov rax, qword [rsp + %d] \n   push rax\n";
char *create_var_from_stack(int position)
{

    int length = snprintf(NULL, 0, create_var_from_stack_string, position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, create_var_from_stack_string, position);

    return formatted_string;
}


const char *mov_literal_to_stack_string = "     mov qword [rsp + %d], %d \n";
char *mov_literal_to_stack(int position, int literal)
{
    int stack_position = position;
    int length = snprintf(NULL, 0, mov_literal_to_stack_string, stack_position, literal) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, mov_literal_to_stack_string, stack_position, literal);

    return formatted_string;
}


const char *mov_stack_to_stack_string = "    mov rax, qword [rsp + %d] \n   mov qword [rsp + %d], rax \n";

char *mov_stack_to_stack(int position_origin, int position_destination)
{
    int stack_position_origin = position_origin;
    int stack_position_destination = position_destination;
    int length = snprintf(NULL, 0, mov_stack_to_stack_string, stack_position_origin, stack_position_destination) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {
        return NULL;
    }
    snprintf(formatted_string, length, mov_stack_to_stack_string, stack_position_origin, stack_position_destination);

    return formatted_string;
}


struct SemanticRecord *assign_id(struct SemanticRecord *expression_location, FILE *file_asm, struct symbol_table *symbols, int stack_position)
{
    if (expression_location->semantic_record == LITERAL_SEMANTIC)
    {
        char *assign_literal_to_id = (mov_literal_to_stack(get_stack_pos(symbols, stack_position), expression_location->semantic_info));
        fputs(assign_literal_to_id, file_asm);
        return create_record(ID_SEMANTIC, stack_position);
    }
    int expression_stack_position = expression_location->semantic_info;
    char *assign_stack_to_stack = (mov_stack_to_stack(get_stack_pos(symbols, expression_stack_position), get_stack_pos(symbols, stack_position)));
    fputs(assign_stack_to_stack, file_asm);
    return create_record(ID_SEMANTIC, stack_position);
}

struct SemanticRecord *create_id(struct SemanticRecord *expression_location, FILE *file_asm, struct symbol_table *symbols, int stack_position, char *var_name)
{
    if (expression_location->semantic_record == LITERAL_SEMANTIC)
    {
        char *create_from_literal = (create_var_from_literal(expression_location->semantic_info));
        int temp_pos = get_temp_int();
        insert_symbol(symbols, var_name, temp_pos);
        fputs(create_from_literal, file_asm);
        return create_record(ID_SEMANTIC, temp_pos);
    }
    int expression_stack_position = expression_location->semantic_info;
    char *create_var_with_stack = (create_var_from_stack(get_stack_pos(symbols, expression_stack_position)));
    int temp_pos = get_temp_int();
    insert_symbol(symbols, var_name, temp_pos);
    fputs(create_var_with_stack, file_asm);
    return create_record(ID_SEMANTIC, temp_pos);
}


void process_id_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{

    int stack_position = process_id(ast_Node->lexema, symbols);

    if (ast_Node->next->type != ASSIGN_OP_AST)
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES.\n Process ID STatement\n");
        return;
    }
    if (!ast_Node->next->next)
    {
        perror("EXPRESSION MISSING AFTER ASSIGN\n");
        return;
    }

    struct SemanticRecord *expression_information = process_expression(ast_Node->next->next, file_asm, symbols);
    if (stack_position == -1)
    {
        struct SemanticRecord *assign_record = create_id(expression_information, file_asm, symbols, stack_position, ast_Node->lexema);
    }
    else
    {
        struct SemanticRecord *assign_record = assign_id(expression_information, file_asm, symbols, stack_position);
    }
}


int process_id(char *var_name, struct symbol_table *symbols)
{
    int var_id = find_symbol(symbols, var_name);
    if (var_id == -1)
    {
        return -1;
    }

    return var_id;
}


int process_id_primary(char *var_name, struct symbol_table *symbols)
{
    int var_id = find_symbol(symbols, var_name);
    if (var_id == -1)
    {
        perror("ID has not been define, before its use on a primary expression");
        return -1;
    }

    return var_id;
}

const char *read_new_id_string = "  leeTeclado\n"
                                 "  mov esi,entrada\n"
                                 "  call process_entrada \n "
                                 "  push rcx\n";

const char *read_old_id_string = "  leeTeclado\n"
                                 "  mov esi,entrada\n"
                                 "  call process_entrada \n"
                                 "  mov [rsp + %d], rcx\n";
char *read_old_id(int position)
{
    int stack_position = position;
    int length = snprintf(NULL, 0, read_old_id_string, stack_position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, read_old_id_string, stack_position);

    return formatted_string;
}


void generate_read_id(FILE *file_asm, int pos_id, struct symbol_table *symbols, char *var_name)
{

    if (pos_id = -1)
    {
        insert_symbol(symbols, var_name, get_temp_int());
        fputs(read_new_id_string, file_asm);
    }
    else
    {
        int stack_pos = get_stack_pos(symbols, pos_id);
        fputs(read_old_id(stack_pos), file_asm);
    }
}


void process_id_list(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!(ast_Node->type == ID_AST))
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS ID LIST");
        return;
    }
    int var_id = process_id(ast_Node->lexema, symbols);
    if (var_id == -1)
    {
        perror("UNKNOWN ERROR\n");
    }

    generate_read_id(file_asm, var_id, symbols, ast_Node->lexema);

    struct NodeAST *next_id = ast_Node->next;

    while (next_id != NULL)
    {

        var_id = process_id(next_id->lexema, symbols);
        generate_read_id(file_asm, var_id, symbols, ast_Node->lexema);

        next_id = next_id->next;
    }
}


void process_read_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{

    if (ast_Node->next->type != ID_LIST_AST)
    {

        perror("AST TREE DOES NOT FOLLOW CFG RULES\n READ STATEMENT PROCESS");
        return;
    }

    process_id_list(ast_Node->next->children->start, file_asm, symbols);
}


struct SemanticRecord *process_primary(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    struct SemanticRecord *record;
    if (!(ast_Node->type == PRIMARY_AST))
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS PRIMARY");
        return NULL;
    }
    enum ASTNodeType primary_type = ast_Node->children->start->type;
    switch (primary_type)
    {
    case ID_AST:

        int stack_position = process_id_primary(ast_Node->children->start->lexema, symbols);
        if (stack_position == -1)
        {
            perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS PRIMARY");
            return NULL;
        }

        record = create_record(ID_SEMANTIC, stack_position);
        return record;
        break;
    case INT_LITERAL_AST:
        record = create_record(LITERAL_SEMANTIC, atoi(ast_Node->children->start->lexema));
        return record;

        break;
    case EXPRESSION_AST:
        return process_expression(ast_Node->children->start, file_asm, symbols);
        break;

    default:
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS PRIMARY DEFAULT SWITCH");
        break;
        return NULL;
    }
    return NULL;
}


struct SemanticRecord *process_expression(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!(ast_Node->type == EXPRESSION_AST))
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS EXPRESSION");
        return NULL;
    }
    struct SemanticRecord *actual_record = process_primary(ast_Node->children->start, file_asm, symbols);

    if (ast_Node->children->start->next != NULL)
    {
        
        if (ast_Node->children->start->next->type != ADD_OP_AST)
        {
            perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS EXPRESION ADD OP IF");
        }

        if (ast_Node->children->start->next->next != NULL)
        {
            if (ast_Node->children->start->next->next->type != PRIMARY_AST)
            {
                perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS EXPRESION NEXT NEXT ADD OP IF");
            }
        }
        enum ASTNodeType operation_sign = ast_Node->children->start->next->children->start->type;

        struct SemanticRecord *second_record = process_primary(ast_Node->children->start->next->next, file_asm, symbols);

        return genInfix(actual_record, operation_sign, second_record, file_asm, symbols);
    }
    return actual_record;
}


char *int_to_string(int num)
{
    int length = snprintf(NULL, 0, "%d", num);

    char *str = (char *)malloc((length + 1) * sizeof(char));
    if (str == NULL)
    {

        return NULL;
    }

    snprintf(str, length + 1, "%d", num);
    return str;
}

const char *mov_stack_to_ebx_string = "     mov rbx, qword [rsp + %d] \n";
char *mov_stack_to_ebx(int position)
{
    int stack_position = position;
    int length = snprintf(NULL, 0, mov_stack_to_ebx_string, stack_position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }

    snprintf(formatted_string, length, mov_stack_to_ebx_string, stack_position);

    return formatted_string;
}

const char *mov_int_to_ebx_string = "   mov rbx, %d \n ";
char *mov_int_to_ebx(int position)
{

    int length = snprintf(NULL, 0, mov_int_to_ebx_string, position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {
        return NULL;
    }
    snprintf(formatted_string, length, mov_int_to_ebx_string, position);

    return formatted_string;
}

const char *mov_int_to_ecx_string = "   mov rcx, %d \n ";
char *mov_int_to_ecx(int position)
{

    int length = snprintf(NULL, 0, mov_int_to_ecx_string, position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, mov_int_to_ecx_string, position);
    return formatted_string;
}

const char *get_temp_string = "TEMP&%d";
char *get_temp_name(int stack_position)
{
    int temp = stack_position;
    int length = snprintf(NULL, 0, get_temp_string, temp) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }

    snprintf(formatted_string, length, get_temp_string, temp);
    return formatted_string;
}


const char *mov_stack_to_ecx_string = "     mov rcx, qword [rsp + %d] \n";
char *mov_stack_to_ecx(int position)
{
    int stack_position = position;
    int length = snprintf(NULL, 0, mov_stack_to_ecx_string, stack_position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, mov_stack_to_ecx_string, stack_position);

    return formatted_string;
}

struct SemanticRecord *genInfix(struct SemanticRecord *first_record, enum ASTNodeType operation_sign, struct SemanticRecord *second_record, FILE *file, struct symbol_table *symbols)
{

    enum SemanticToken first_token = first_record->semantic_record;
    enum SemanticToken second_token = second_record->semantic_record;

    switch (first_token)
    {
    case ID_SEMANTIC:
    case TEMP_RESULT_SEMANTIC:
        fputs(mov_stack_to_ebx(get_stack_pos(symbols, first_record->semantic_info)), file);
        break;
    case LITERAL_SEMANTIC:
        fputs(mov_int_to_ebx(first_record->semantic_info), file);
        break;

    default:
        break;
    }

    switch (second_token)
    {
    case ID_SEMANTIC:
    case TEMP_RESULT_SEMANTIC:

        fputs(mov_stack_to_ecx(get_stack_pos(symbols, second_record->semantic_info)), file);

        break;
    case LITERAL_SEMANTIC:
        fputs(mov_int_to_ecx(second_record->semantic_info), file);
        break;

    default:
        break;
    }
    if (operation_sign == PLUS_OP_AST)
    {
        fputs("     suma rbx,rcx \n     push rax \n", file);

        int new_stack_position = get_temp_int();

        insert_symbol(symbols, get_temp_name(new_stack_position), new_stack_position);
        return create_record(TEMP_RESULT_SEMANTIC, new_stack_position);
    }
    if (operation_sign == MINUS_OP_AST)
    {
        fputs("     resta rbx, rcx \n   push rax \n", file);
        int new_stack_position = get_temp_int();
        insert_symbol(symbols, get_temp_name(new_stack_position), new_stack_position);
        return create_record(TEMP_RESULT_SEMANTIC, new_stack_position);
    }
    printf("%d\n", operation_sign);
    return NULL;
}

const char *write_literal_string = "    mov edi, numero\n"
                                   "    mov ecx, 22\n"
                                   "    xor al, al\n"
                                   "    rep stosb\n"
                                   "    mov rax, %d\n"
                                   "    call print_number\n";

char *write_literal(int literal)
{

    int length = snprintf(NULL, 0, write_literal_string, literal) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }

    snprintf(formatted_string, length, write_literal_string, literal);

    return formatted_string;
}

const char *write_id_string = "     mov edi, numero\n"
                              "     mov ecx, 22\n"
                              "     xor al, al\n"
                              "     rep stosb\n"
                              "     mov rax, [rsp + %d]\n"
                              "     call print_number\n";

char *write_id(int position)
{
    int length = snprintf(NULL, 0, write_id_string, position) + 1;

    char *formatted_string = (char *)calloc(length, sizeof(char));
    if (formatted_string == NULL)
    {

        return NULL;
    }
    snprintf(formatted_string, length, write_id_string, position);

    return formatted_string;
}

void write_on_screen(FILE *file_asm, struct symbol_table *symbols, struct SemanticRecord *semanticRecord)
{
    if (semanticRecord->semantic_record = LITERAL_SEMANTIC)
    {
        fputs(write_literal(semanticRecord->semantic_info), file_asm);
    }
    else
    {
        int stack_position = get_stack_pos(symbols, semanticRecord->semantic_info);

        fputs(write_id(stack_position), file_asm);
    }
}

void process_expression_list(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!ast_Node->type == EXPRESSION_LIST_AST)
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n EXPRESSION LIST IF\n ");
        return;
    }

    struct SemanticRecord *record_to_write = process_expression(ast_Node->children->start, file_asm, symbols);
    write_on_screen(file_asm, symbols, record_to_write);

    struct NodeAST *next_expression = ast_Node->children->start->next;

    while (next_expression)
    {
        record_to_write = process_expression(next_expression, file_asm, symbols);
        write_on_screen(file_asm, symbols, record_to_write);

        next_expression = next_expression->next;
    }
}

void process_write_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (ast_Node->next->type != EXPRESSION_LIST_AST)
    {
        perror("AST TREE DOES NOT FOLLOW CFG RULES\n PROCESS WRITE STATEMENT \n");
        return;
    }
    process_expression_list(ast_Node->next, file_asm, symbols);
}

void process_statement(struct NodeAST *statement, FILE *file_asm, struct symbol_table *symbols)
{
    enum ASTNodeType first_token = statement->children->start->type;
    switch (first_token)
    {
    case ID_AST:
        process_id_statement(statement->children->start, file_asm, symbols);
        break;
    case READ_SYM_AST:
        process_read_statement(statement->children->start, file_asm, symbols);
        break;
    case WRITE_SYM_AST:
        process_write_statement(statement->children->start, file_asm, symbols);
        break;
    default:
        perror("AST TREE DOES NOT FOLLOW THE CFG RULES\n PROCESS STATEMENT DEFAULT \n");
        break;
    }
}

void process_statement_list(FILE *file_asm, struct NodeAST *ast_Tree, struct symbol_table *symbols)
{
    struct NodeAST *statement_temp = ast_Tree->children->start;
    while (statement_temp)
    {
        process_statement(statement_temp, file_asm, symbols);

        statement_temp = statement_temp->next;
    }
}

void translator(struct NodeAST *ast_Tree)
{
    struct symbol_table *symbols = create_symbol_table();
    FILE *file_asm = fopen("asm_code.asm", "w");
    if (file_asm == NULL)
    {
        perror("Error opening file \n");
        return;
    }

    gen_prefix(file_asm);

    process_statement_list(file_asm, ast_Tree, symbols);

    gen_sufix(file_asm);
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
    struct content *tokenName;

    file = fopen("input.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    struct double_linked_list *tokens = create_double_linked_list();

    struct NodeAST *ast_Tree = system_goal(file, tokens);
    translator(ast_Tree);
    system("nasm -f elf64 asm_code.asm -o asm_code.o");
    system("ld asm_code.o -m elf_x86_64 -o asm_code");
    system("./asm_code");

    fclose(file);

    return 0;
}
