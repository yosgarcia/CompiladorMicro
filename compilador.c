/*
 * Integrantes: - Yosward Garcia Tellez
 *              - Yarmna Charpentier Castellón
 * 
 * Ejecutar el código sin ejecutar el ensamblador:
 *      make run LOCATION=<nombre del archivo a compilar> -f MakeFile 
 * Ejecutar el código y ejecutar el ensamblador:
 *      make run_asm LOCATION=<nombre del archivo a compilar> -f MakeFile
 * 
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"
#include "ast.h"
#include "content.h"

#define MAX_WORD_SIZE 32


//Enumeracion que indica el tipo de información de un record semántico
enum SemanticToken
{
    LITERAL_SEMANTIC,
    ID_SEMANTIC,
    TEMP_RESULT_SEMANTIC,
    EXPRESSION_RESULT_SEMANTIC
};
//Estructura que contiene un ENUM que indica su tipo de record semántico
//En el caso de expresiones y IDs, tiene en su entero su posición en el stack
//En el caso de los literales, contiene su valor
struct SemanticRecord
{
    enum SemanticToken semantic_record;
    int semantic_info;
};

/*
 * Funcion: Procesa una expresión y devuelve un record semántico con su posición en la pila
 * @param ast_Node: Nodo del arbol ast con la información de la expresión
 * @param file_asm: Archivo donde se está escribiendo el codigo asm
 * @param symbols: Tabla de símbolos 
 * @return SemanticRecord: recordSemántico con la información de la expresión
*/
struct SemanticRecord *process_expression(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols);
/*
 * Funcion: Función que procesa 2 records semánticos y genera el código para operarlos
 * @param first_record: Primer record a procesar
 * @param operation_sign: Enum de suma o resta, con el cual se decide que operación generar.
 * @param second_record: Segundo record a procesar 
 * @return SemanticRecord: recordSemántico con la posición de donde quedó el resultado de la operación en el stack.
*/
struct SemanticRecord *genInfix(struct SemanticRecord *first_record, enum ASTNodeType operation_sign, struct SemanticRecord *second_record, FILE *file, struct symbol_table *symbols);


/*
 * Funcion: Regla de producción Expression con la que se parsean las expresiones y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void expression(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);

/*
 * Funcion: Regla de producción addop con la que se parsean las expresiones y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void add_op(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);

/*
 * Funcion: Regla de producción primary con la que se parsean las sumas y restas y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void primary(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
/*
 * Funcion: Regla de producción expression list con la que se parsean los primarios (literales, ids y sumas) y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void expression_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);

/*
 * Funcion: Regla de producción id_list con la que se parsean las listas de identificadores y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void id_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
/*
 * Funcion: Regla de producción statement con la que se parsean los statement y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/

void statement(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);
/*
 * Funcion: Regla de producción statement_list con la que se parsean los statement list y se genera su registro en el arbol ast
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática
 * @param parent: Nodo padre del arbol ast.
*/
void statement_list(FILE *file, struct double_linked_list *tokens, struct NodeAST *parent);

/*
 * Funcion: Regla de producción program la cual inicia el parseo despues de un begin
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática

*/
struct NodeAST *program(FILE *file, struct double_linked_list *tokens);
/*
 * Funcion: Regla de producción program la cual inicia el parseo de tokens
 * @param file: Archivo con el código de micro
 * @param tokens : Lista que contiene los tokens por procesar, para validar la gramática

*/
struct NodeAST *system_goal(FILE *file, struct double_linked_list *tokens);
/*
 * Funcion: Procesa un id, en una operación de asignación. Asigna una posición del stack o devuelve 
 * se el id ya posee una
 * @param var_name: Nombre de la variable
 * @param symbols: Tabla de simbolos
 * @param return: El id nuevo de la variable, o su id propio en caso de que ya existiera.
*/
int process_id(char *var_name, struct symbol_table *symbols);
/*
 * Funcion: Crea un struct de record semántico
 * @param semanticToken: El tipo de Semantica del record a crear, ID, TEMP o LITERAL
 * @param semantic_info: Informacion del record, valor literal en caso de LITERALES, o posición en el stack
 *  en los demas casos
 * @param return: La estructura de record semántico.
*/
struct SemanticRecord *create_record(enum SemanticToken, int semantic_info);

//Variable globar del arbol trie de las palabras reservadas/
struct trie_node *root;

/*
 * Funcion: Crea un struct de record semántico
 * @param semanticToken: El tipo de Semantica del record a crear, ID, TEMP o LITERAL
 * @param semantic_info: Informacion del record, valor literal en caso de LITERALES, o posición en el stack
 *  en los demas casos
 * @param return: La estructura de record semántico.
*/
struct SemanticRecord *create_record(enum SemanticToken token, int semantic_info)
{
    struct SemanticRecord *new_record = calloc(1, sizeof(struct SemanticRecord));
    new_record->semantic_record = token;
    new_record->semantic_info = semantic_info;
    return new_record;
}


/*
 * Funcion: Devuelve la posición de una variable en el stack, en función de la cantidad de variables creadas y su id referencial
 * @param symbols: Tabla de simbolos
 * @param id_var: Id referencial de la variable en el stack
 * @param return: La posición exacta de la variable en el stack.
*/
int get_stack_pos(struct symbol_table *symbols, int id_var)
{
    return (8 * symbols->max_i) - 8 * id_var;
}


/*
 * Funcion: Convierte un caracter a puntero
 * @param c: El caracter a convertir
 * @param return: El puntero al caracter
*/
char *char_to_string(char c)
{
    char *str = calloc(2, sizeof(char));
    str[0] = c;
    str[1] = '\0';
    return str;
}
/*
 * Funcion: Función que scannea el siguiente token de el archivo de código micro
 * @param file: El puntero al filestream del archivo micro
 * @param return: Un struct content con la información del token detectado
*/
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


/*
 * Funcion: FUnción que indica un error de syntaxys y sale de las llamadas con un codigo 1
 * @param token: El token que llegó de manera inesperada
*/
void syntax_error(enum TokenType token)
{
    printf("Syntax Error: Unexpected token %d\n", token);
    exit(1);
}

void semantic_error(char *message)
{
    printf("Semantic Error: %s\n", message);
    exit(1);
}

/*
 * Funcion: Funcion que añade un token a la lista de tokens
 * @param file: Filestream del archivo con el código micro.
 * @param tokens: Lista de tokens detectados
*/
void next_token(FILE *file, struct double_linked_list *tokens)
{

    add_node_to_list(tokens, scanner(file));
}
/*
 * Funcion: Función que realiza un match de un token esperado con el primero de la lista de tokens
 * @param expectedToken: El token que se espera recibir
 * @param tokens: La lista de tokens
*/
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

/*
 * Funcion: Funcion que procesa un nodoAST de addOp
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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

/*
 * Funcion: Funcion que procesa una expresión primaria y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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
/*
 * Funcion: Funcion que procesa una expresión y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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
/*
 * Funcion: Funcion que procesa una lista de expresiones y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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

/*
 * Funcion: Funcion que procesa una lista de ids y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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
/*
 * Funcion: Funcion que procesa un statement primaria y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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

/*
 * Funcion: Funcion que procesa una lista de statement y las añade al arbol ast
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param parent: El padre del arbol ast
*/
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
/*
 * Funcion: Funcion que inicia la creación del arbol ast.
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param return La raiz del arbol ast.
*/
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

/*
 * Funcion: Funcion que llama a la iniciacion del arbol ast, y despues verifica que no hayan mas tokens
 * @param file: El filestream del programa en micro
 * @param tokens: Lista de tokens
 * @param return La raiz del arbol ast.
*/
struct NodeAST *system_goal(FILE *file, struct double_linked_list *tokens)
{
    struct NodeAST *root = program(file, tokens);
    next_token(file, tokens);
    match(EOF_SYM, tokens);
    return root;
}
/*
 * Funcion: Funcion que devuelva el proximo id de variable a crear. Este id representa que es la id+1 variable creada. Ademas 
 * este id se utiliza para calcular su posicion en en el stack

 * @return El siguiente id a ser utlizado
*/
int get_temp_int()
{
    static int counter = -1;
    return ++counter;
}
/*
 * Funcion: Funcion que escribe el prefijo en el archivo de código ensamblador.
 * @param file: El filestream del archivo del programa en micro
*/
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

/*
 * Funcion: Funcion que escribe el sufijo en el archivo de código ensamblador.
 * @param file: El filestream del archivo del programa en micro
*/
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

/*
 * Funcion: Funcion que genera el string para crear una variable en el stack a partir de un literal.
 * @param literal: El valor literal de la variable
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para crear una variable en el stack a partir de otra posición en el stack.
 * @param literal: La posición en el stack de la variable que servirá de referencia
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para asigar una variable en el stack un valor literal.
 * @param literal: El valor literal de la variable
 * @param position: La posición en el stack de la variable que va a ser asignada.
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para asignar una variable en el stack el valor de otra variable.
 * @param position_origin: La posición en el stack del valor de origen
 * @param position_destination: La posición en el stack del valor de destino
 * @return El string formateado
*/
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


/*
 * Funcion: Funcion que escribe en un archivo el código para asignar un valor a una variable.
 * @param expression_location: El record semantico con la información del valor que será asignado
 * @param file_asm: El filestream donde se escribirá el codigo
 * @param symbols: La tabla de simbolos
 * @param stack_position: La posicion del stack donde irá este valor.
 * @return El string formateado
*/
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

/*
 * Funcion: Funcion que escribe en un archivo el código paracrear una variable.
 * @param expression_location: El record semantico con la información del valor que será asignado
 * @param file_asm: El filestream donde se escribirá el codigo
 * @param symbols: La tabla de simbolos
 * @param stack_position: La posicion del stack donde irá este valor.
 * @param var_name: El nombre de la variabel
 * @return El string formateado
*/
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

/*
 * Funcion: Funcion que procesa un statement de asignacion de id.
 * @param ast_Node: Nodo del arbol ast con la infomacion necesaria 
 * @param file_asm: El filestream donde se escribirá el codigo
 * @param symbols: La tabla de simbolos

*/
void process_id_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{

    int stack_position = process_id(ast_Node->lexema, symbols);

    if (ast_Node->next->type != ASSIGN_OP_AST)
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return;
    }
    if (!ast_Node->next->next)
    {
        semantic_error("EXPRESSION MISSING AFTER ASSIGN\n");
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

/*
 * Funcion: Funcion busca un nombre en la tabla de simbolos y devuelve su posición
 * @return La posición relativa de la variable en el stack, o -1 si esta no existe.
*/
int process_id(char *var_name, struct symbol_table *symbols)
{
    int var_id = find_symbol(symbols, var_name);
    if (var_id == -1)
    {
        return -1;
    }

    return var_id;
}

/*
 * Funcion: Funcion que procesa un id en una expresión primaria. 
 * @param var_name : Nombre de la variable a buscar en la table
 * @param symbols: Tabla de simbolos
 * @return La posición relativa de la variable en el stack, o -1 si esta no existe y por ende la expresión primaria es incorrecta.
*/
int process_id_primary(char *var_name, struct symbol_table *symbols)
{
    int var_id = find_symbol(symbols, var_name);
    if (var_id == -1)
    {
        semantic_error("ID has not been defined, before its use on a primary expression");
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
/*
 * Funcion: Funcion que genera el string para leer un input y asignarlo a una variable.
 * @param literal: Valor del stack donde estará esta variable
 * @return El string formateado
*/
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

/*
 * Funcion: Funcion que genera el el codigo en asm para leer un id de un input.
 * @param file_asm: Filestream donde se está escribiendo el código
 * @param pos_id: Posicion relativa del stack, de donde se introducira el valor de la variable
 * @symbols: Tabla de simbolos
 * @var_name : Nombre de la variable a leer
*/
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

/*
 * Funcion: Funcion que procesa una lista de ids desde el arbol ast
 * @param ast_node: Nodo del arbol
 * @param file_asm: Filestream donde se escribe el codigo
 * @symbols: Tabla de simbolos
*/
void process_id_list(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!(ast_Node->type == ID_AST))
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return;
    }
    int var_id = process_id(ast_Node->lexema, symbols);
    if (var_id == -1)
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
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


/*
 * Funcion: Funcion que procesa un statement de lectura
 * @param ast_node: Nodo del arbol
 * @param file_asm: Filestream donde se escribe el codigo
 * @symbols: Tabla de simbolos
*/
void process_read_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{

    if (ast_Node->next->type != ID_LIST_AST)
    {

        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return;
    }

    process_id_list(ast_Node->next->children->start, file_asm, symbols);
}


/*
 * Funcion: Funcion que procesa un primary
 * @param ast_node: Nodo del arbol
 * @param file_asm: Filestream donde se escribe el codigo
 * @symbols: Tabla de simbolos
 * @return El record semantico con la posición del valor obtenido del primary
*/
struct SemanticRecord *process_primary(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    struct SemanticRecord *record;
    if (!(ast_Node->type == PRIMARY_AST))
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return NULL;
    }
    enum ASTNodeType primary_type = ast_Node->children->start->type;
    switch (primary_type)
    {
    case ID_AST:

        int stack_position = process_id_primary(ast_Node->children->start->lexema, symbols);
        if (stack_position == -1)
        {
            semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
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
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        break;
        return NULL;
    }
    return NULL;
}


/*
 * Funcion: Funcion que procesa una expresion
 * @param ast_node: Nodo del arbol
 * @param file_asm: Filestream donde se escribe el codigo
 * @symbols: Tabla de simbolos
 * @return El record semantico con la posición del valor obtenido de la expresion
*/
struct SemanticRecord *process_expression(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!(ast_Node->type == EXPRESSION_AST))
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return NULL;
    }
    struct SemanticRecord *actual_record = process_primary(ast_Node->children->start, file_asm, symbols);

    if (ast_Node->children->start->next != NULL)
    {
        
        if (ast_Node->children->start->next->type != ADD_OP_AST)
        {
            semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        }

        if (ast_Node->children->start->next->next != NULL)
        {
            if (ast_Node->children->start->next->next->type != PRIMARY_AST)
            {
                semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
            }
        }
        enum ASTNodeType operation_sign = ast_Node->children->start->next->children->start->type;

        struct SemanticRecord *second_record = process_primary(ast_Node->children->start->next->next, file_asm, symbols);

        return genInfix(actual_record, operation_sign, second_record, file_asm, symbols);
    }
    return actual_record;
}

/*
 * Funcion: Funcion que convierte un entero a un string
 * @param num: El entero a convertir
 * @return el puntero al string creado
*/
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

/*
 * Funcion: Funcion que genera el string para mover un valor del stack al registro ebx.
 * @param position: Valor del stack donde estará esta variable
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para mover un valor entero al registro ebx.
 * @param position: Valor entero
 * @return El string formateado
*/
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

/*
 * Funcion: Funcion que genera el string para mover un valor entero al registro ecx.
 * @param position: Valor entero
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera para dar un nombre a un valor temporal
 * @param position: position relativa del stack
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para mover un valor del stack al registro ecx.
 * @param position: Valor entero
 * @return El string formateado
*/
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
/*
 * Funcion: Función que procesa 2 records semánticos y genera el código para operarlos
 * @param first_record: Primer record a procesar
 * @param operation_sign: Enum de suma o resta, con el cual se decide que operación generar.
 * @param second_record: Segundo record a procesar 
 * @return SemanticRecord: recordSemántico con la posición de donde quedó el resultado de la operación en el stack.
*/
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

/*
 * Funcion: Funcion que genera el string para imprimir un literal en pantalla.
 * @param position: Valor entero a imprimir
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que genera el string para imprimir un id en pantalla.
 * @param position: Posicion en el stack
 * @return El string formateado
*/
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
/*
 * Funcion: Funcion que hace el procesamiento para imprimir un valor en pantalla
 * @param file_asm: Filestream del archivo en asm
 * @param symbols : Tabla de simbolos
 * @param semantic_record: Record semantico con la informacion a escribir

*/
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

/*
 * Funcion: Funcion que hace el procesamiento para un lista de expresion lista
 * @param ast_Node: Nodo ast con la informacion de la expresion
 * @param symbols: Tabla de simbolos
 * @param file_Asm : Filestream del codigo a escribir

*/
void process_expression_list(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (!ast_Node->type == EXPRESSION_LIST_AST)
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
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
/*
 * Funcion: Funcion que hace el procesamiento para un statemetn de escritura 
 * @param ast_Node: Nodo ast con la informacion de la expresion
 * @param symbols: Tabla de simbolos
 * @param file_Asm : Filestream del codigo a escribir
*/
void process_write_statement(struct NodeAST *ast_Node, FILE *file_asm, struct symbol_table *symbols)
{
    if (ast_Node->next->type != EXPRESSION_LIST_AST)
    {
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        return;
    }
    process_expression_list(ast_Node->next, file_asm, symbols);
}
/*
 * Funcion: Funcion que hace el procesamiento para un statement
 * @param ast_Node: Nodo ast con la informacion de la expresion
 * @param symbols: Tabla de simbolos
  * @param file_Asm : Filestream del codigo a escribir
*/
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
        semantic_error("AST TREE DOES NOT FOLLOW THE CFG RULES");
        break;
    }
}
/*
 * Funcion: Funcion que hace el procesamiento para una lista de statement
 * @param ast_Node: Nodo ast con la informacion de la expresion
 * @param symbols: Tabla de simbolos
 * @param file_Asm : Filestream del codigo a escribir
*/
void process_statement_list(FILE *file_asm, struct NodeAST *ast_Tree, struct symbol_table *symbols)
{
    struct NodeAST *statement_temp = ast_Tree->children->start;
    while (statement_temp)
    {
        process_statement(statement_temp, file_asm, symbols);

        statement_temp = statement_temp->next;
    }
}
/*
 * Funcion: Funcion que inicia la traduccion del arbol ast a codigo ensamblador
 * @param ast_Tree: arbol ast del codigo de input
*/
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

int main(int argc, char *argv[])
{   

    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    root = create_trie_node('\0');
    insert_word(root, "begin", BEGIN_SYM);
    insert_word(root, "end", END_SYM);
    insert_word(root, "read", READ_SYM);
    insert_word(root, "write", WRITE_SYM);
    FILE *file;
    char word[MAX_WORD_SIZE];
    struct content *tokenName;

    file = fopen(filename, "r");
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

    fclose(file);

    return 0;
}
