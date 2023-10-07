%{
#include "actions.hpp"
#include<fstream>
#define yylex(x) alpha_yylex(nullptr)
#define local_or_global (Symbol::curr_scope) ? Symbol::Type::LOCAL_VARIABLE : Symbol::Type::GLOBAL_VARIABLE
#define print_rule(left, right) std::cout << "\033[1;32m LINE " << yylineno << ": " << #right  << " --> " << #left << "\033[0m" << std::endl
#undef print_rule
#define print_rule(left, right)
int yyerror(const char* yaccProvidedMessage){
   compile_error(yaccProvidedMessage);
   return 0;
 }
int alpha_yylex(void* ylval);
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
%}
%code requires { #include "types.hpp" }
%union{
    int intVal;
    unsigned unsVal;
    char strVal[200];
    double dblVal;
    Expr::LValue* exprLVal;
    Expr::New_Table* exprNewTable;
    EList* elistVal;
    Expr::Base* exprVal;
    Expr::Const* constVal;
    IndexedElem* indexedElemVal;
    IndexedList* indexedVal;
    Expr::Table_Item* memberVal;
    Expr::Assign_Expr* assignVal;
    Symbol* symVal;
    Call* callVal;
    For_Prefix forPreVal;
    Stmt stmtVal;
}

%start PROGRAM

%token IF
ELSE
WHILE
FOR
FUNCTION
RETURN
BREAK
CONTINUE
AND
NOT
OR
LOCAL
TRUE
FALSE
NIL
<strVal> IDENT
<intVal>INTCONST
<dblVal>REALCONST
EQUAL
PLUS
MINUS
MUL
DEVIDE
MOD
DOUBLEEQUAL
NOTEQUAL
PLUSPLUS
MINUSMINUS
LESSER
GREATER
LESSEROREQUAL
GREATEROREQUAL
SPACE
LEFTCURLYBRACKET
RIGHTCURLYBRACKET
LEFTPARENTHESIS
RIGHTPARENTHESIS
LEFTSQUAREBRACKET
RIGHTSQUAREBRACKET
SEMICOLON
KOMMA
DOTUPANDDOWN
DOUBLEDOTUPANDDOWN
DOT
DOUBLEDOT
ERROR
<strVal>STRING

%type<exprLVal> LVALUE
%type<exprNewTable> OBJECTDEF
%type<elistVal> ELIST
%type<exprVal> EXPR
%type<constVal> CONST
%type<exprVal> TERM
%type<exprVal> PRIMARY
%type<assignVal> ASSIGNEXPR
%type<indexedElemVal> INDEXEDELEM
%type<indexedVal> INDEXED
%type<memberVal> MEMBER
%type<unsVal> IFPREFIX
%type<unsVal> ELSEPREFIX
%type<unsVal> WHILESTART
%type<unsVal> WHILECOND
%type<strVal> FUNCNAME
%type<symVal> FUNCPREFIX
%type<unsVal> FUNCBODY
%type<symVal> FUNCDEF
%type<exprVal> CALL
%type<callVal> CALLSUFFIX
%type<callVal> NORMCALL
%type<callVal> METHODCALL
%type<forPreVal> FORPREFIX
%type<unsVal> M
%type<unsVal> N
%type<stmtVal> STMT
%type<stmtVal> LOOPSTMT
%type<stmtVal> STMTS
%type<stmtVal> CONTINUE_S
%type<stmtVal> BREAK_S
%type<stmtVal> BLOCK
%type<stmtVal> IFSTMT

%right  EQUAL
%left   OR
%left   AND
%nonassoc DOUBLEEQUAL NOTEQUAL
%nonassoc GREATER GREATEROREQUAL LESSER LESSEROREQUAL
%left   PLUS MINUS
%left   MUL DEVIDE MOD
%right  NOT PLUSPLUS MINUSMINUS
%left   DOT DOUBLEDOT
%left   LEFTSQUAREBRACKET RIGHTSQUAREBRACKET
%left   LEFTPARENTHESIS RIGHTPARENTHESIS 

%%

PROGRAM:    STMTS {print_rule(PROGRAM, STMTS);
                Actions::_PROGRAM::_STMTS();}

STMTS:      STMT {Actions::_STMTS::_STMT($$, $1);}
            | STMTS STMT {Actions::_STMTS::_STMTS_STMT($$, $1, $2);}


BREAK_S :   BREAK SEMICOLON {Actions::_BREAK_S::_BREAK_SEMICOLON($$);}

CONTINUE_S :    CONTINUE SEMICOLON {Actions::_CONTINUE_S::_CONTINUE_SEMICOLON($$);}

STMT:       EXPR SEMICOLON {print_rule(STMT, EXPR SEMICOLON);
                    Actions::_STMT::_EXPR_SEMICOLON($$, $1);}
            |IFSTMT {print_rule(STMT, IFSTMT);
                    Actions::_STMT::_IFSTMT($$, $1); }
            |WHILESTMT {print_rule(STMT, WHILESTMT);
                    Actions::_STMT::_WHILESTMT($$); }
            |FORSTMT {print_rule(STMT, FORSTMT);
                    Actions::_STMT::_FORSTMT($$); }
            |RETURNSTMT {print_rule(STMT, RETURNSTMT);
                    Actions::_STMT::_RETURNSTMT($$); }
            |BREAK_S {print_rule(STMT, BREAK SEMICOLON);
                    Actions::_STMT::_BREAK_S($$, $1); }
            |CONTINUE_S {print_rule(STMT, CONTINUE SEMICOLON);
                    Actions::_STMT::_CONTINUE_S($$, $1); }
            |BLOCK {print_rule(STMT, BLOCK);
                    Actions::_STMT::_BLOCK($$, $1); }
            |FUNCDEF {print_rule(STMT, FUNCDEF);
                    Actions::_STMT::_FUNCDEF($$); }
            |SEMICOLON {print_rule(STMT, SEMICOLON);
                    Actions::_STMT::_SEMICOLON($$);}


EXPR:       ASSIGNEXPR {print_rule(EXPR, ASSIGNEXPR);
                        Actions::_EXPR::_TERM($$, $1); }
            |EXPR PLUS EXPR {print_rule(EXPR, EXPR PLUS EXPR);
                            Actions::_EXPR::_EXPR_PLUS_EXPR($$, $1, $3); }
            |EXPR MINUS EXPR {print_rule(EXPR, EXPR MINUS EXPR);
                            Actions::_EXPR::_EXPR_MINUS_EXPR($$, $1, $3);}
            |EXPR MUL EXPR {print_rule(EXPR, EXPR MUL EXPR);
                            Actions::_EXPR::_EXPR_MUL_EXPR($$, $1, $3);}
            |EXPR DEVIDE EXPR {print_rule(EXPR, EXPR DIVIDE EXPR);
                            Actions::_EXPR::_EXPR_DEVIDE_EXPR($$, $1, $3);}
            |EXPR MOD EXPR {print_rule(EXPR, EXPR MOD EXPR);
                            Actions::_EXPR::_EXPR_MOD_EXPR($$, $1, $3);}
            |EXPR GREATER EXPR {print_rule(EXPR, EXPR GREATER EXPR);
                            Actions::_EXPR::_EXPR_GREATER_EXPR($$, $1, $3);}
            |EXPR GREATEROREQUAL EXPR {print_rule(EXPR, EXPR GREATEROREQUAL EXPR);
                            Actions::_EXPR::_EXPR_GREATEROREQUAL_EXPR($$, $1, $3);}
            |EXPR LESSER EXPR {print_rule(EXPR, EXPR LESSER EXPR);
                            Actions::_EXPR::_EXPR_LESSER_EXPR($$, $1, $3);}
            |EXPR LESSEROREQUAL EXPR {print_rule(EXPR, EXPR LESSEROREQUAL EXPR);
                            Actions::_EXPR::_EXPR_LESSEROREQUAL_EXPR($$, $1, $3);}
            |EXPR DOUBLEEQUAL EXPR {print_rule(EXPR, EXPR DOUBLEEQUAL EXPR);
                            Actions::_EXPR::_EXPR_DOUBLEEQUAL_EXPR($$, $1, $3);}
            |EXPR NOTEQUAL EXPR {print_rule(EXPR, EXPR NOTEQUAL EXPR);
                            Actions::_EXPR::_EXPR_NOTEQUAL_EXPR($$, $1, $3);}
            |EXPR AND {
                    Actions::_EXPR::_EXPR_BOOLOP($1);
            } M EXPR {print_rule(EXPR, EXPR AND EXPR);
                            Actions::_EXPR::_EXPR_AND_M_EXPR($$, $1, $4, $5);}
            |EXPR OR {
                    Actions::_EXPR::_EXPR_BOOLOP($1);
            } M EXPR {print_rule(EXPR, EXPR OR EXPR);
                            Actions::_EXPR::_EXPR_OR_M_EXPR($$, $1, $4, $5);}
            |TERM {print_rule(EXPR, TERM);
                    Actions::_EXPR::_TERM($$, $1);}


TERM:       LEFTPARENTHESIS EXPR RIGHTPARENTHESIS {print_rule(TERM, LEFTPARENTHESIS EXPR RIGHTPARENTHESIS);
                                                Actions::_TERM::_LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS($$, $2);}
            |MINUS EXPR {print_rule(TERM, MINUS EXPR);
                        Actions::_TERM::_MINUS_EXPR($$, $2);}
            |NOT EXPR   {print_rule(TERM, NOT EXPR);
                        Actions::_TERM::_NOT_EXPR($$, $2);}
            |PLUSPLUS LVALUE {
            print_rule(TERM, PLUSPLUS);
            Actions::_TERM::_PLUSPLUS_LVALUE($$, $2);}
            |LVALUE PLUSPLUS 
            {
            print_rule(TERM, LVALUE PLUSPLUS);
            Actions::_TERM::_LVALUE_PLUSPLUS($$, $1);}
            |MINUSMINUS LVALUE {
            print_rule(TERM, MINUSMINUS LVALUE);
            Actions::_TERM::_MINUSMINUS_LVALUE($$, $2);}
            |LVALUE MINUSMINUS {
                print_rule(TERM, LVALUE MINUSMINUS);
                Actions::_TERM::_LVALUE_MINUSMINUS($$, $1);}
            |PRIMARY {print_rule(TERM, PRIMARY); Actions::_TERM::_PRIMARY($$, $1); }

ASSIGNEXPR: LVALUE EQUAL EXPR {
print_rule(ASSIGNEXPR, LVALUE EQUAL EXPR);
            Actions::_ASSIGNEXPR::_LVALUE_EQUAL_EXPR($$, $1, $3);}

PRIMARY:    LVALUE {print_rule(PRIMARY, LVALUE);
                    Actions::_PRIMARY::_LVALUE($$, $1); }
            | CALL {print_rule(PRIMARY, CALL);
                    Actions::_PRIMARY::_CALL($$, $1);}
            | OBJECTDEF {print_rule(PRIMARY, OBJECTDEF);
            Actions::_PRIMARY::_OBJECTDEF($$, $1);}
            | LEFTPARENTHESIS FUNCDEF RIGHTPARENTHESIS {print_rule(PRIMARY, LEFTPARENTHESIS FUNCDEF RIGHTPARENTHESIS);
            Actions::_PRIMARY::_LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS($$, $2);}
            | CONST {print_rule(PRIMARY, CONST);
                Actions::_PRIMARY::_CONST($$, $1);}
            

LVALUE:     IDENT { print_rule(LVALUE, IDENT);
                    Actions::_LVALUE::_IDENT($$, $1);
                    }

            | LOCAL IDENT {
                            print_rule(LVALUE, LOCAL IDENT);
                            Actions::_LVALUE::_LOCAL_IDENT($$, $2);
                            }
            | DOUBLEDOTUPANDDOWN IDENT {
                                        print_rule(LVALUE, DOUBLEDOTUPANDDOWN IDENT);
                                        Actions::_LVALUE::_DOUBLEDOTUPANDDOWN_IDENT($$, $2);
                                    }
            | MEMBER { Actions::_LVALUE::_MEMBER($$, $1);}

MEMBER:     LVALUE DOT IDENT {print_rule(MEMBER, LVALUE DOT IDENT);
                            Actions::_MEMBER::_LVALUE_DOT_IDENT($$, $1, $3);}
            | LVALUE LEFTSQUAREBRACKET EXPR RIGHTSQUAREBRACKET  {print_rule(MEMBER, LVALUE LEFTSQUAREBRACKET EXPR RIGHTSQUAREBRACKET);
                                                            Actions::_MEMBER::_LVALUE_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET($$, $1, $3);}
            | CALL DOT IDENT {print_rule(MEMBER, CALL DOT IDENT);
                Actions::_MEMBER::_CALL_DOT_IDENT($$, $1, $3);}
            | CALL LEFTSQUAREBRACKET EXPR RIGHTSQUAREBRACKET {print_rule(MEMBER, CALL LEFTSQUAREBRACKET EXPR RIGHTSQUAREBRACKET);
                Actions::_MEMBER::_CALL_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET($$, $1, $3);}

CALL :      CALL LEFTPARENTHESIS ELIST RIGHTPARENTHESIS {print_rule(CALL, CALL LEFTPARENTHESIS ELIST RIGHTPARENTHESIS);
                                                    Actions::_CALL::_CALL_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS($$, $1, $3);}
            | LVALUE CALLSUFFIX {print_rule(CALL, LVALUE CALLSUFFIX);
                                Actions::_CALL::_LVALUE_CALLSUFFIX($$, $1, $2);}
            | LEFTPARENTHESIS FUNCDEF RIGHTPARENTHESIS LEFTPARENTHESIS ELIST RIGHTPARENTHESIS
            {print_rule(CALL, LEFTPARENTHESIS FUNCDEF RIGHTPARENTHESIS LEFTPARENTHESIS ELIST RIGHTPARENTHESIS);
                                Actions::_CALL::_LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS($$, $2, $5); }

CALLSUFFIX : NORMCALL
            {print_rule(CALLSUFFIX, NORMCALL);
            Actions::_CALLSUFFIX::_NORMCALL($$, $1);}
            | METHODCALL
            {print_rule(CALLSUFFIX, METHODCALL);
            Actions::_CALLSUFFIX::_METHODCALL($$, $1);}

NORMCALL :  LEFTPARENTHESIS ELIST RIGHTPARENTHESIS {print_rule(NORMCALL, LEFTPARENTHESIS ELIST RIGHTPARENTHESIS);
                                                Actions::_NORMCALL::_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS($$, $2);}

METHODCALL : DOUBLEDOT IDENT LEFTPARENTHESIS ELIST RIGHTPARENTHESIS {print_rule(METHODCALL, DOUBLEDOT IDENT LEFTPARENTHESIS ELIST RIGHTPARENTHESIS);
                                                Actions::_METHODCALL::_DOUBLEDOT_IDENT_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS($$, $2, $4);}

ELIST:      { Actions::_ELIST::_($$); }
            |EXPR KOMMA ELIST {print_rule(ELIST, EXPR KOMMA ELIST);
                                Actions::_ELIST::_EXPR_KOMMA_ELIST($$, $1, $3); }
            |EXPR {print_rule(ELIST, EXPR);
                    Actions::_ELIST::_EXPR($$, $1); }

OBJECTDEF : LEFTSQUAREBRACKET ELIST RIGHTSQUAREBRACKET {print_rule(OBJECTDEF, LEFTSQUAREBRACKET ELIST RIGHTSQUAREBRACKET);
                    Actions::_OBJECTDEF::_LEFTSQUAREBRACKET_ELIST_RIGHTSQUAREBRACKET($$, $2);}
            |LEFTSQUAREBRACKET INDEXED RIGHTSQUAREBRACKET {print_rule(OBJECTDEF, LEFTSQUAREBRACKET INDEXED RIGHTSQUAREBRACKET );
                                                        Actions::_OBJECTDEF::_LEFTSQUAREBRACKET_INDEXED_RIGHTSQUAREBRACKET($$, $2);}

INDEXED:    INDEXEDELEM KOMMA INDEXED {print_rule(INDEXED, INDEXEDELEM KOMMA INDEXED );
                                    Actions::_INDEXED::_INDEXEDELEM_KOMMA_INDEXED($$, $1, $3);}
            |INDEXEDELEM {print_rule(INDEXED, INDEXEDELEM);
                        Actions::_INDEXED::_INDEXEDELEM($$, $1);}

INDEXEDELEM : LEFTCURLYBRACKET EXPR DOTUPANDDOWN
                {Actions::_INDEXEDELEM::_LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN($2);} EXPR RIGHTCURLYBRACKET
                {print_rule(INDEXEDELEM, LEFTCURLYBRACKET EXPR DOTUPANDDOWN EXPR RIGHTCURLYBRACKET);
                Actions::_INDEXEDELEM::_LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN_EXPR_RIGHTCURLYBRACKET($$, $2, $5);}

BLOCK:      LEFTCURLYBRACKET {++Symbol::curr_scope;} STMTS RIGHTCURLYBRACKET {
                        print_rule(BLOCK, LEFTCURLYBRACKET STMTS RIGHTCURLYBRACKET);
                        Symbol::Hide(Symbol::curr_scope--);
                        Actions::_BLOCK::_LEFTCURLYBRACKET_STMTS_RIGHTCURLYBRACKET($$, $3);}
            | LEFTCURLYBRACKET RIGHTCURLYBRACKET {Quad::make_stmt($$);}

FUNCNAME :  {Actions::_FUNCNAME::_($$);}
            | IDENT {Actions::_FUNCNAME::_IDENT($$, $1);}

FUNCPREFIX :    FUNCTION FUNCNAME {Actions::_FUNCPREFIX::_FUNCTION_FUNCNAME($$, $2);}

FUNCARGS :  LEFTPARENTHESIS {++Symbol::curr_scope;} IDLIST RIGHTPARENTHESIS {--Symbol::curr_scope;Actions::_FUNCARGS::_LEFTPARENTHESIS_IDLIST_RIGHTPARENTHESIS();} 

FUNCBLOCKSTART :     { Actions::_FUNCBLOCKSTART::_(); }

FUNCBLOCKEND :  { Actions::_FUNCBLOCKEND::_(); }

FUNCBODY :  FUNCBLOCKSTART BLOCK FUNCBLOCKEND {Actions::_FUNCBODY::_FUNCBLOCKSTART_BLOCK_FUNCBLOCKEND($$);}


FUNCDEF :   FUNCPREFIX FUNCARGS FUNCBODY {print_rule(FUNCDEF, FUNCTION IDENT LEFTPARENTHESIS IDLIST RIGHTPARENTHESIS BLOCK);
                                        Actions::_FUNCDEF::_FUNCPREFIX_FUNCARGS_FUNCBODY($$, $1, $3);
                                        }

CONST :     INTCONST {print_rule(CONST, INTCONST);
                        Actions::_CONST::_INTCONST($$, $1);}
            |REALCONST {print_rule(CONST, REALCONST);
                        Actions::_CONST::_REALCONST($$, $1);}
            | STRING  {print_rule(CONST, STRING);
                        Actions::_CONST::_STRING($$, $1);}
            | NIL {print_rule(CONST, NIL);
                    Actions::_CONST::_NIL($$);}
            | TRUE  {print_rule(CONST, TRUE);
                    Actions::_CONST::_TRUE($$);}
            | FALSE {print_rule(CONST, FALSE);
                    Actions::_CONST::_FALSE($$);}

IDLIST:     {Actions::_IDLIST::_();}
            |IDENT KOMMA IDLIST {
                print_rule(IDLIST, IDENT KOMMA IDLIST);
                Actions::_IDLIST::_IDENT_KOMMA_IDLIST($1);}
            |IDENT {
                print_rule(IDLIST, IDENT);    
                Actions::_IDLIST::_IDENT($1);}

IFPREFIX :  IF LEFTPARENTHESIS EXPR RIGHTPARENTHESIS {Actions::_IFPREFIX::_IF_LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS($$, $3);}

ELSEPREFIX :   ELSE {Actions::_ELSEPREFIX::_ELSE($$);}

IFSTMT :    IFPREFIX STMT {print_rule(IFSTMT, IF LEFTPARENTHESIS EXPR RIGHTPARENTHESIS STMT);
                        Actions::_IFSTMT::_IFPREFIX_STMT($$, $1, $2);}
            | IFPREFIX STMT ELSEPREFIX STMT {print_rule(IFSTMT, IF LEFTPARENTHESIS EXPR RIGHTPARENTHESIS STMT ELSE STMT);
                                            Actions::_IFSTMT::_IFPREFIX_STMT_ELSEPREFIX_STMT($$, $1, $2, $3, $4);}


LOOPSTART : {Actions::_LOOPSTART::_();}

LOOPEND :   {Actions::_LOOPEND::_();}

LOOPSTMT :  LOOPSTART STMT LOOPEND {Actions::_LOOPSTMT::_LOOPSTART_STMT_LOOPEND($$, $2);}

WHILESTART : WHILE {Actions::_WHILESTART::_WHILE($$);}

WHILECOND : LEFTPARENTHESIS EXPR RIGHTPARENTHESIS {Actions::_WHILECOND::_LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS($$, $2);}

WHILESTMT : WHILESTART WHILECOND LOOPSTMT {print_rule(WHILESTMT, WHILE LEFTPARENTHESIS EXPR RIGHTPARENTHESIS STMT);
                                            Actions::_WHILESTMT::_WHILESTART_WHILECOND_LOOPSTMT($1, $2, $3);}

N : {Actions::_N::_($$);}

M : {Actions::_M::_($$);}

FORPREFIX : FOR LEFTPARENTHESIS ELIST M SEMICOLON EXPR SEMICOLON {Actions::_FORPREFIX::_FOR_LEFTPARENTHESIS_ELIST_SEMICOLON_M_EXPR_SEMICOLON($$, $4, $6);}

FORSTMT :   FORPREFIX N ELIST RIGHTPARENTHESIS N LOOPSTMT N {print_rule(FORSTMT, FOR LEFTPARENTHESIS ELIST SEMICOLON EXPR SEMICOLON ELIST RIGHTPARENTHESIS STMT);
                                                    Actions::_FORSTMT::_FORPREFIX_N_ELIST_RIGHTPARENTHESIS_N_LOOPSTMT_N($1, $2, $5, $6, $7);}

RETURNSTMT : RETURN EXPR SEMICOLON {print_rule(RETURNSTMT, RETURN EXPR SEMICOLON);
            Actions::_RETURNSTMT::_RETURN_EXPR_SEMICOLON($2);}
            |RETURN SEMICOLON {print_rule(RETURNSTMT,RETURN SEMICOLON);
                            Actions::_RETURNSTMT::_RETURN_SEMICOLON();}


%%

int main(int argc, char** argv)
{
    --argc;
    ++argv;
    std::ofstream abc_file;
    if(argc > 0){
        yyin = fopen(argv[0], "r");
    }else{
        yyin = stdin;
    }
    Function::initialize_library_functions();
    yyparse();
    if(argc > 0){
        fclose(yyin);
    }
    // Symbol::print_all_symbols();
    Quad::Base::print_all();
    Generate::generate_all();
    abc_file.open((argc > 1) ? argv[1] : "alpha.abc", std::ios::out | std::ios::binary);
    TCODE::print_target_code(abc_file);
    abc_file.close();
    free(Function::dummy);
    clean_up();
}