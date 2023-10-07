#ifndef ACTIONS_H
#define ACTIONS_H
#include "generators.hpp"
#define local_or_global (Symbol::curr_scope) ? Symbol::Type::LOCAL_VARIABLE : Symbol::Type::GLOBAL_VARIABLE

namespace Actions {
    namespace _PROGRAM {
        void _STMTS();
    }
    namespace _STMTS {
        void _STMT(Stmt&, Stmt&);
        void _STMTS_STMT(Stmt&, Stmt&, Stmt&);
    }
    namespace _STMT {
        void _EXPR_SEMICOLON(Stmt&, Expr::Base*);
        void _IFSTMT(Stmt&, Stmt&);
        void _WHILESTMT(Stmt&);
        void _FORSTMT(Stmt&);
        void _RETURNSTMT(Stmt&);
        void _BREAK_S(Stmt&, Stmt&);
        void _CONTINUE_S(Stmt&, Stmt&);
        void _FUNCDEF(Stmt&);
        void _BLOCK(Stmt&, Stmt&);
        void _SEMICOLON(Stmt&);
    }
    namespace _BREAK_S {
        void _BREAK_SEMICOLON(Stmt&);
    }
    namespace _CONTINUE_S {
        void _CONTINUE_SEMICOLON(Stmt&);
    }
    namespace _LVALUE {
        void _IDENT(Expr::LValue*&, const char*);
        void _LOCAL_IDENT(Expr::LValue*&, const char*);
        void _DOUBLEDOTUPANDDOWN_IDENT(Expr::LValue*&, const char*);
        void _MEMBER(Expr::LValue*&, Expr::Table_Item*);
    }

    namespace _MEMBER {
        void _LVALUE_DOT_IDENT(Expr::Table_Item*&, Expr::LValue*, const char*);
        void _LVALUE_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET(Expr::Table_Item*&, Expr::LValue*, Expr::Base*);
        void _CALL_DOT_IDENT(Expr::Table_Item*&, Expr::Base*, const char*);
        void _CALL_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET(Expr::Table_Item*&, Expr::Base*, Expr::Base*);
    }

    namespace _CALL {
        void _CALL_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Expr::Base*&, Expr::Base*, EList*);
        void _LVALUE_CALLSUFFIX(Expr::Base*&, Expr::LValue*&, const Call*);
        void _LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Expr::Base*&, Symbol*, EList*);
    }

    namespace _CALLSUFFIX {
        void _NORMCALL(Call*&, Call*);
        void _METHODCALL(Call*&, Call*);
    }

    namespace _NORMCALL {
        void _LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Call*&, EList*);
    }

    namespace _METHODCALL {
        void _DOUBLEDOT_IDENT_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Call*&, char*, EList*);
    }

    namespace _FUNCNAME {
        void _(char*);
        void _IDENT(char*, char*);
    }

    namespace _FUNCPREFIX {
        void _FUNCTION_FUNCNAME(Symbol*&, const char*);
    }

    namespace _FUNCARGS {
        void _LEFTPARENTHESIS_IDLIST_RIGHTPARENTHESIS();
    }

    namespace _FUNCBLOCKSTART {
        void _();
    }

    namespace _FUNCBLOCKEND {
        void _();
    }

    namespace _FUNCBODY {
        void _FUNCBLOCKSTART_BLOCK_FUNCBLOCKEND(unsigned&);
    }

    namespace _FUNCDEF {
        void _FUNCPREFIX_FUNCARGS_FUNCBODY(Symbol*&, Symbol*, const unsigned);
    }
    namespace _OBJECTDEF {
        void _LEFTSQUAREBRACKET_ELIST_RIGHTSQUAREBRACKET(Expr::New_Table*&, EList*);
        void _LEFTSQUAREBRACKET_INDEXED_RIGHTSQUAREBRACKET(Expr::New_Table*&, IndexedList*);
    }

    namespace _INDEXED {
        void _INDEXEDELEM_KOMMA_INDEXED(IndexedList*&, IndexedElem*, IndexedList*);
        void _INDEXEDELEM(IndexedList*&, IndexedElem*);
    }

    namespace _INDEXEDELEM {
        void _LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN(Expr::Base*);
        void _LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN_EXPR_RIGHTCURLYBRACKET(IndexedElem*&, Expr::Base*, Expr::Base*);
    }
    namespace _BLOCK {
        void _LEFTCURLYBRACKET_STMTS_RIGHTCURLYBRACKET(Stmt&, Stmt&);
    }
    namespace _ELIST {
        void _(EList*&);
        void _EXPR(EList*&, Expr::Base*);
        void _EXPR_KOMMA_ELIST(EList*&, Expr::Base*, EList*);
    }

    namespace _TERM {
        void _LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(Expr::Base*&, Expr::Base*);
        void _MINUS_EXPR(Expr::Base*&, Expr::Base*);
        void _NOT_EXPR(Expr::Base*&, Expr::Base*);
        void _PLUSPLUS_LVALUE(Expr::Base*&, Expr::LValue*);
        void _LVALUE_PLUSPLUS(Expr::Base*&, Expr::LValue*);
        void _MINUSMINUS_LVALUE(Expr::Base*&, Expr::LValue*);
        void _LVALUE_MINUSMINUS(Expr::Base*&, Expr::LValue*);
        void _PRIMARY(Expr::Base*&, Expr::Base*);
    }

    namespace _PRIMARY {
        void _LVALUE(Expr::Base*&, Expr::LValue*);
        void _CALL(Expr::Base*&, Expr::Base*);
        void _OBJECTDEF(Expr::Base*&, Expr::New_Table*);
        void _LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS(Expr::Base*&, Symbol*);
        void _CONST(Expr::Base*&, Expr::Const*);
    }
    namespace _CONST {
        void _INTCONST(Expr::Const*&, int);
        void _REALCONST(Expr::Const*&, double);
        void _STRING(Expr::Const*&, const char*);
        void _NIL(Expr::Const*&);
        void _TRUE(Expr::Const*&);
        void _FALSE(Expr::Const*&);
    }

    namespace _ASSIGNEXPR {
        void _LVALUE_EQUAL_EXPR(Expr::Assign_Expr*&, Expr::LValue*, Expr::Base*);
    }
    namespace _S {
        void _(unsigned&);
    }
    namespace _EXPR {
        void _ASSIGNEXPR(Expr::Base*&, Expr::Base*);
        void _EXPR_PLUS_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_MINUS_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_MUL_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_DEVIDE_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_MOD_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_GREATER_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_GREATEROREQUAL_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_LESSER_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_LESSEROREQUAL_EXPR (Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_DOUBLEEQUAL_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_NOTEQUAL_EXPR(Expr::Base*&, Expr::Base*, Expr::Base*);
        void _EXPR_BOOLOP(Expr::Base*&);
        void _EXPR_AND_M_EXPR(Expr::Base*&, Expr::Base*, unsigned, Expr::Base*);
        void _EXPR_OR_M_EXPR(Expr::Base*&, Expr::Base*, unsigned, Expr::Base*);
        void _TERM(Expr::Base*&, Expr::Base*);
    }
    namespace _IDLIST {
        void _();
        void _IDENT_KOMMA_IDLIST(const char*);
        void _IDENT(const char*);
    }
    namespace _IFPREFIX {
        void _IF_LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(unsigned&, Expr::Base*);
    }
    namespace _ELSEPREFIX {
        void _ELSE(unsigned&);
    }
    namespace _IFSTMT {
       void _IFPREFIX_STMT(Stmt&, const unsigned, Stmt&);
       void _IFPREFIX_STMT_ELSEPREFIX_STMT(Stmt&, const unsigned, Stmt&, const unsigned, Stmt&);
    }
    namespace _LOOPSTART {
        void _();
    }
    namespace _LOOPEND {
        void _();
    }
    namespace _LOOPSTMT {
        void _LOOPSTART_STMT_LOOPEND(Stmt&, Stmt&);
    }
    namespace _WHILESTART {
        void _WHILE(unsigned&);
    }
    namespace _WHILECOND {
        void _LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(unsigned&, Expr::Base*);
    }
    namespace _WHILESTMT {
        void _WHILESTART_WHILECOND_LOOPSTMT(const unsigned, const unsigned, Stmt&);
    }
    namespace _N {
        void _(unsigned&);
    }
    namespace _M {
        void _(unsigned&);
    }
    namespace _FORPREFIX {
        void _FOR_LEFTPARENTHESIS_ELIST_SEMICOLON_M_EXPR_SEMICOLON(For_Prefix&, unsigned, Expr::Base*);
    }
    namespace _FORSTMT {
        void _FORPREFIX_N_ELIST_RIGHTPARENTHESIS_N_LOOPSTMT_N(const For_Prefix&, const unsigned, const unsigned, Stmt&, const unsigned);
    }
    namespace _RETURNSTMT {
        void _RETURN_EXPR_SEMICOLON(Expr::Base*);
        void _RETURN_SEMICOLON();
    }
}
#endif