/* scanner for alpha */

%{
#include "parser.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#define YY_DECL int alpha_yylex(void *ylval)
#include "alpha_token_t.hpp"
std::list<alpha_token_t*> alpha_token_list;
#define add_and_print(keyword) alpha_token_t *temp_token = static_cast<alpha_token_t*>(ylval);\
                temp_token = new alpha_token_t(yylineno, alpha_token_list.size() + 1, (std::string)yytext, (std::string)#keyword);\
                alpha_token_list.push_front(temp_token);\
            temp_token->print_token();
#define add_and_print_nested(keyword) temp_token2 = new alpha_token_t(yylineno, alpha_token_list.size() + 1, (std::string)yytext, (std::string)#keyword);\
                alpha_token_list.push_front(temp_token2);\
            temp_token2->print_token();
#undef add_and_print
#define add_and_print(x) return x;
#undef add_and_print_nested
#define add_and_print_nested(x)
%}
%option noyywrap
%option yylineno
%option outfile = "scanner.cpp"

IF                  "if"
ELSE                "else"
WHILE               "while"
FOR                 "for"
FUNCTION            "function"
RETURN              "return"
BREAK               "break"
CONTINUE            "continue"
AND                 "and"
NOT                 "not"
OR                  "or"
LOCAL               "local"
TRUE                "true" 
FALSE               "false"
NIL                 "nil"
IDENT             [a-zA-Z][a-zA-Z_0-9]*
INTCONST       (0[xX][0-9A-Fa-f]+)|([0-9]+)
REALCONST       [0-9]*\.[0-9](e|E)?[0-9]*
/*OPERATOR        "="|"+"|"-"|"/"|"=="|"!="|"++"|"--"|"<"|">"|"<="|">="|"*"|"%"*/
EQUAL               "="
PLUS                "+"
MINUS               "-"
MUL                 "*"
DEVIDE              "/"
MOD                 "%"
DOUBLEEQUAL         "==" 
NOTEQUAL            "!=" 
PLUSPLUS            "++" 
MINUSMINUS          "--" 
LESSER              "<"
GREATER             ">"
LESSEROREQUAL       "<=" 
GREATEROREQUAL      ">=" 
SPACE           [\r\n \t\v]
COMMENT1        "//".*
LEFTCURLYBRACKET     "{"
RIGHTCURLYBRACKET    "}"
LEFTPARENTHESIS     "("
RIGHTPARENTHESIS    ")"
LEFTSQUAREBRACKET   "["
RIGHTSQUAREBRACKET  "]"
SEMICOLON           ";"
KOMMA               ","
DOTUPANDDOWN        ":"
DOUBLEDOTUPANDDOWN  "::"
DOT                 "."
DOUBLEDOT           ".."
ERROR               .
%%

"/*" {
    alpha_token_t *temp_token2 = static_cast<alpha_token_t*>(ylval);
    std::stack<std::string> lines;
    int curr_char;
    std::string temp = std::to_string(yylineno), nested_temp;
    unsigned short int counter = 1;
    temp += " - ";
    while(counter) {
        while((curr_char = yyinput()) != '*' && curr_char != '/' && curr_char != '\0');
        if(curr_char == '*'){
            if((curr_char = yyinput()) == '/'){ 
                if(--counter){
                    nested_temp = lines.top() + " - " +  std::to_string(yylineno);
                    lines.pop();
                    strcpy(yytext, nested_temp.c_str());
                    add_and_print_nested(NESTEDCOMMENT);
                }
            }
            else unput(curr_char);
        }
        else if(curr_char == '/'){
            if((curr_char = yyinput()) == '*') {
                ++counter;
                lines.push(std::to_string(yylineno));
            }
            else unput(curr_char);
        } else if(curr_char == '\0'){
            std::cerr << "UNCLOSED COMMENT AT LINE: " << yylineno << std::endl;
            exit(-1);
        }
    }
    temp += std::to_string(yylineno);
    strcpy(yytext, temp.c_str());
}

\" {
    std::string temp = "\"";
    char curr_char;
    while((curr_char = yyinput()) != '\"'){
        if(curr_char == '\0'){
            std::cerr << "UNCLOSED STRING AT LINE: " << yylineno << std::endl;
            exit(-1);
        }
        if(curr_char != '\n') temp += curr_char;
    }
    temp += '\"';
    replace_backslash(temp);
    strcpy(yytext, temp.c_str());
    strcpy(yylval.strVal, temp.c_str());
    add_and_print(STRING);
}


{IF} {
            add_and_print(IF);
            return IF;
    }

{ELSE} {
            add_and_print(ELSE);
    }

{WHILE} {
            add_and_print(WHILE);
    }

{FOR} {
            add_and_print(FOR);
    }

{FUNCTION} {
            add_and_print(FUNCTION);
    }

{RETURN} {
            add_and_print(RETURN);
    }

{BREAK} {
            add_and_print(BREAK);
    }

{CONTINUE} {
            add_and_print(CONTINUE);
    }

{AND} {
            add_and_print(AND);
    }

{NOT} {
            add_and_print(NOT);
    }

{OR} {
            add_and_print(OR);
    }

{LOCAL} {
            add_and_print(LOCAL);
    }

{TRUE} {
            add_and_print(TRUE);
    }

{FALSE} {
            add_and_print(FALSE);
    }

{NIL} {
            add_and_print(NIL);
    }


{IDENT} {
            strcpy(yylval.strVal, yytext);
            add_and_print(IDENT);
    }

{INTCONST} {
            yylval.intVal = atoi(yytext);
            add_and_print(INTCONST);
    }
{REALCONST} {
            yylval.dblVal = atof(yytext);
            add_and_print(REALCONST);
    }

{EQUAL} {
            add_and_print(EQUAL);
            return '=';
    }

{PLUS} {
            add_and_print(PLUS);
            return '+';
    }

{MINUS} {
            add_and_print(MINUS);
            return '-';
    }

{MUL} {
            add_and_print(MUL);
            return '*';
    }

{DEVIDE} {
            add_and_print(DEVIDE);
            return '/';
    }

{MOD} {
            add_and_print(MOD);
            return '%';
    }

{DOUBLEEQUAL} {
            add_and_print(DOUBLEEQUAL);
    }

{NOTEQUAL} {
            add_and_print(NOTEQUAL);
    }

{PLUSPLUS} {
            add_and_print(PLUSPLUS);
    }

{MINUSMINUS} {
            add_and_print(MINUSMINUS);
    }

{LESSER} {
            add_and_print(LESSER);
            return '<';
    }

{GREATER} {
            add_and_print(GREATER);
            return '>';
    }

{LESSEROREQUAL} {
            add_and_print(LESSEROREQUAL);
    }

{GREATEROREQUAL} {
            add_and_print(GREATEROREQUAL);
    }

{SPACE} {
    }


{COMMENT1} {
    }


{LEFTCURLYBRACKET} {
            add_and_print(LEFTCURLYBRACKET);
            return '{';
    } 

{RIGHTCURLYBRACKET} {
            add_and_print(RIGHTCURLYBRACKET);
            return '}';
    } 

{LEFTPARENTHESIS} {
            add_and_print(LEFTPARENTHESIS);
            return '(';
    } 

{RIGHTPARENTHESIS} {
            add_and_print(RIGHTPARENTHESIS);
            return ')';
    } 

{LEFTSQUAREBRACKET} {
            add_and_print(LEFTSQUAREBRACKET);
            return '[';
    } 

{RIGHTSQUAREBRACKET} {
            add_and_print(RIGHTSQUAREBRACKET);
            return ']';
    } 

{SEMICOLON} {
            add_and_print(SEMICOLON);
            return ';';
    }

{KOMMA} {
            add_and_print(KOMMA);
            return ',';
    }

{DOTUPANDDOWN} {
            add_and_print(DOTUPANDDOWN);
            return ':';
    }

{DOUBLEDOTUPANDDOWN} {
            add_and_print(DOUBLEDOTUPANDDOWN);
    }

{DOT} {
            add_and_print(DOT);
            return '.';
    }

{DOUBLEDOT} {
            add_and_print(DOUBLEDOT);
    }

{ERROR} {
    std::cerr << "UNRECOGNISED SYMBOL AT LINE: " << yylineno << " \"" << yytext << "\"" << std::endl;
    exit(1);
    }
