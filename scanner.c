#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"

#define MAX_WORD_SIZE 32

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



struct trie_node *root;

int is_keyword(char *word)
{
    return 0;
}


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
            // Creo que esta linea me la puedo clappear// Poner lo de fijarme si es keyword
            
            /*
            tokenType = find_word(root, identifier);
            switch (tokenType) {
                case BEGIN_SYM:
                    return BEGIN_SYM;
                case END_SYM:
                    return END_SYM;
                case READ_SYM:
                    return READ_SYM;
                case WRITE_SYM:
                    return WRITE_SYM;
                default:
                    return ID;
            }
            */
            if (is_keyword(identifier))
            {
                if (!strcmp(identifier, "begin"))
                {
                    return BEGIN_SYM;
                }
                else if (!strcmp(identifier, "end"))
                {
                    return END_SYM;
                }
                else if (!strcmp(identifier, "read"))
                {
                    return READ_SYM;
                }
                else if (!strcmp(identifier, "write"))
                {
                    return WRITE_SYM;
                }
                else
                continue; // keyword
            }
            else
            {
                //printf("Identifier: %s\n", identifier);
                return ID;
            }
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
int main()
{
    /*
    root = create_trie_node('\0');
    insert_word(root, "begin", BEGIN_SYM);
    insert_word(root, "end", END_SYM);
    insert_word(root, "read", READ_SYM);
    insert_word(root, "write", WRITE_SYM);
    */

    FILE *file;
    char word[MAX_WORD_SIZE];
    enum TokenType tokenName;

    // Open the file
    file = fopen("input.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    while ((tokenName = scanner(file)) != NULL_TOKEN)
    {
        switch (tokenName)
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
        default:
            printf("Others\n");
            break;
            
        }
    }

    // Close the file
    fclose(file);

    return 0;
}
