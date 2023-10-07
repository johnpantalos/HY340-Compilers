#include"actions.hpp"
#include <cstring>

namespace Actions {
    namespace _PROGRAM {
        void _STMTS() {}
    }
    namespace _STMTS {
        void _STMT(Stmt& STMTS, Stmt& STMT) {
            STMTS = STMT;
        }
        void _STMTS_STMT(Stmt& STMTS, Stmt& STMTS_OLD, Stmt& STMT) {
            STMTS.set_contlist(Quad::mergelist(STMTS_OLD.get_contlist(), STMT.get_contlist()));
            STMTS.set_breaklist(Quad::mergelist(STMTS_OLD.get_breaklist(), STMT.get_breaklist()));
        }
    }
    namespace _STMT {
        void _EXPR_SEMICOLON(Stmt& STMT, Expr::Base* EXPR) {
            Quad::backpatch_ifneeded(EXPR);
            Quad::make_stmt(STMT);
            Temp_Variable::reset_temp();
        }
        void _WHILESTMT(Stmt& STMT) {
            Quad::make_stmt(STMT);
        }
        void _FORSTMT(Stmt& STMT) {
            Quad::make_stmt(STMT);
        }
        void _RETURNSTMT(Stmt& STMT) {
            Quad::make_stmt(STMT);
        }
        void _IFSTMT(Stmt& STMT, Stmt& IFSTMT) {
            STMT = IFSTMT;
        }
        void _BREAK_S(Stmt& STMT, Stmt& BREAK_S) {
            STMT = BREAK_S;
        }
        void _CONTINUE_S(Stmt& STMT, Stmt& CONTINUE_S) {
            STMT = CONTINUE_S;
        }
        void _FUNCDEF(Stmt& STMT) {
            Quad::make_stmt(STMT);
        }
        void _BLOCK(Stmt& STMT, Stmt& BLOCK) {
            STMT = BLOCK;
        }
        void _SEMICOLON(Stmt& STMT) {
            Quad::make_stmt(STMT);
            Temp_Variable::reset_temp();
        }
    }
    namespace _BREAK_S {
        void _BREAK_SEMICOLON(Stmt& BREAK_S) {
            if(Stmt::loop_counter) {
                Quad::make_stmt(BREAK_S);
                Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
                BREAK_S.set_breaklist(Quad::newlist(Quad::Base::curr_quad_n));
            } else {
                compile_error("BREAK STATEMENT OUTSIDE OF LOOP", true);
            }
            Temp_Variable::reset_temp();
        }
    }
    namespace _CONTINUE_S {
        void _CONTINUE_SEMICOLON(Stmt& CONTINUE_S) {
            if(Stmt::loop_counter) {
                Quad::make_stmt(CONTINUE_S);
                Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
                CONTINUE_S.set_contlist(Quad::newlist(Quad::Base::curr_quad_n));
            } else {
                compile_error("CONTINUE STATEMENT OUTSIDE OF LOOP", true);
            }
            Temp_Variable::reset_temp();
        }
    }
    namespace _LVALUE {
        void _IDENT(Expr::LValue*& LVALUE, const char* IDENT){
            assert(LVALUE && IDENT);
            Symbol* sym = nullptr;
            if(!(sym = Symbol::Lookup_range(IDENT, 0, Symbol::curr_scope)) && 
                !Symbol::is_library_function(IDENT)){
                Symbol::Insert(IDENT, sym = new Variable(IDENT, local_or_global));
            } else {
                if(!Symbol::is_library_function(IDENT) && !Symbol::is_accessable(IDENT)) {
                    compile_error(static_cast<std::string>(IDENT) + " IS INACCESSABLE");
                }
            }
            LVALUE = Expr::to_lvalue(sym);
        }

        void _LOCAL_IDENT(Expr::LValue*& LVALUE, const char* IDENT){
            assert(LVALUE && IDENT);
            Symbol* sym;
            if(!Symbol::is_library_function(IDENT)){
                sym = Symbol::Lookup(IDENT, Symbol::curr_scope);
                if(!sym){
                    Symbol::shadow_key(IDENT);
                    Symbol::Insert(IDENT, sym = new Variable(IDENT, local_or_global));
                }
            } else {
                compile_error("REDECLARATION OF LIBRARY FUNCTION " + static_cast<std::string>(IDENT));
                LVALUE = nullptr;
                return;
            }
            LVALUE = Expr::to_lvalue(sym);
        }

        void _DOUBLEDOTUPANDDOWN_IDENT(Expr::LValue*& LVALUE, const char* IDENT){
            assert(LVALUE && IDENT);
            Symbol* sym = nullptr;
            if(sym = Symbol::Lookup_global(IDENT)) LVALUE = Expr::to_lvalue(sym);
            else compile_error(static_cast<std::string>(IDENT) + " NOT FOUND IN GLOBAL SCOPE", true);
        }
        void _MEMBER(Expr::LValue*& LVALUE, Expr::Table_Item* MEMBER) {
            Symbol::curr_is_func = false;
            LVALUE = MEMBER;
        }
    }

    namespace _MEMBER {
        void _LVALUE_DOT_IDENT(Expr::Table_Item*& MEMBER, Expr::LValue* LVALUE, const char* IDENT) {
            IDENT = ("\"" + static_cast<std::string>(IDENT) + "\"").c_str();
            MEMBER = Quad::member_item(LVALUE, IDENT);
        }
        void _LVALUE_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET(Expr::Table_Item*& MEMBER, Expr::LValue* LVALUE, Expr::Base* EXPR){
            assert(LVALUE && EXPR);
            LVALUE = dynamic_cast<Expr::LValue*>(Quad::emit_iftableitem(LVALUE));
            assert(LVALUE);
            MEMBER = new Expr::Table_Item(LVALUE->get_sym(), EXPR);
        }
        void _CALL_DOT_IDENT(Expr::Table_Item*& MEMBER, Expr::Base* CALL, const char* IDENT) {
            IDENT = ("\"" + static_cast<std::string>(IDENT) + "\"").c_str();
            MEMBER = Quad::member_item(dynamic_cast<Expr::LValue*>(CALL), IDENT);
        }
        void _CALL_LEFTSQUAREBRACKET_EXPR_RIGHTSQUAREBRACKET(Expr::Table_Item*&  MEMBER, Expr::Base* CALL, Expr::Base* EXPR) {
            assert(CALL && EXPR);
            MEMBER = new Expr::Table_Item(CALL->get_sym(), EXPR);
        }
    }
    namespace _CALL {
        void _CALL_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Expr::Base*& CALL, Expr::Base* CALL_OLD, EList* ELIST) {
            assert(CALL_OLD && ELIST);
            CALL = Quad::make_call(dynamic_cast<Expr::LValue*>(CALL_OLD), ELIST);
        }
        void _LVALUE_CALLSUFFIX(Expr::Base*& CALL, Expr::LValue*& LVALUE, const Call* CALLSUFFIX){
            assert(LVALUE && CALLSUFFIX);
            LVALUE = dynamic_cast<Expr::LValue*>(Quad::emit_iftableitem(LVALUE));
            assert(LVALUE);
            if(CALLSUFFIX->get_is_method()) {
                Expr::LValue* t = LVALUE;
                LVALUE = dynamic_cast<Expr::LValue*>(Quad::emit_iftableitem(Quad::member_item(t, CALLSUFFIX->get_name().c_str())));
                assert(LVALUE);
                CALLSUFFIX->get_elist()->push_front(t);
            }
            CALL = Quad::make_call(LVALUE, CALLSUFFIX->get_elist());
        }
        void _LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Expr::Base*& CALL, Symbol* FUNCDEF, EList* ELIST) {
            CALL = Quad::make_call(new Expr::Program_Func(FUNCDEF), ELIST);
            assert(CALL);
        }
    }

    namespace _CALLSUFFIX {
        void _NORMCALL(Call*& CALLSUFFIX, Call* NORMCALL) {
            assert(NORMCALL);
            CALLSUFFIX = NORMCALL;
        }
        void _METHODCALL(Call*& CALLSUFFIX, Call* METHODCALL) {
            assert(METHODCALL);
            CALLSUFFIX = METHODCALL;
        }
    }

    namespace _NORMCALL {
        void _LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Call*& NORMCALL, EList* ELIST) {
            assert(ELIST);
            NORMCALL = new Call("", false, ELIST);
        }
    }

    namespace _METHODCALL {
       void _DOUBLEDOT_IDENT_LEFTPARENTHESIS_ELIST_RIGHTPARENTHESIS(Call*& METHODCALL, char* IDENT, EList* ELIST) {
           assert(IDENT && ELIST);
           METHODCALL = new Call("\"" + static_cast<std::string>(IDENT) + "\"", true, ELIST);
       }
    }
    namespace _FUNCNAME {
        void _(char* FUNCNAME) {
            strcpy(FUNCNAME, Function::get_next_func_name().c_str());
        }
        void _IDENT(char* FUNCNAME, char* IDENT) {
            strcpy(FUNCNAME, IDENT);
        }
    }
    namespace _FUNCPREFIX {
        void _FUNCTION_FUNCNAME(Symbol*& FUNCPREFIX, const char* FUNCNAME) {
            std::string FUNCNAME_STR = static_cast<std::string>(FUNCNAME);
            if(!Symbol::Lookup(FUNCNAME_STR, Symbol::curr_scope) && !Symbol::is_library_function(FUNCNAME_STR)){
                    Symbol::Insert(FUNCNAME_STR, Function::curr_function = new Function(FUNCNAME_STR, Symbol::Type::USER_FUNCTION));
            } else {
                if(Symbol::is_library_function(FUNCNAME_STR)) compile_error(FUNCNAME_STR + " IS A LIBRARY FUNCTION");
                else compile_error(FUNCNAME_STR + " ALREADY EXISTS IN THIS SCOPE");
                Function::curr_function = Function::dummy;
            }
            ++Function::curr_function_depth;
            Stmt::func_start_jump_stack.push(Quad::Base::curr_quad_n);
            Stmt::return_jump_stack.push(std::list<unsigned>());
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
            FUNCPREFIX = Function::curr_function;
            Quad::emit(Quad::IOpCode::FUNCSTART, new Expr::Program_Func(FUNCPREFIX), nullptr, nullptr);
            Symbol::scope_offset_stack.push(Symbol::curr_scope_offset());
            Symbol::enter_scope_space();
            Symbol::reset_scope_offset(Symbol::Scope_Space::FORMAL_ARG);
        }
    }

    namespace _FUNCARGS {
        void _LEFTPARENTHESIS_IDLIST_RIGHTPARENTHESIS(){
            Symbol::enter_scope_space();
            Symbol::reset_scope_offset(Symbol::Scope_Space::FUNCTION_LOCAL);
        }
    }

     namespace _FUNCBLOCKSTART {
        void _() {
            Stmt::loop_counter_stack.push(Stmt::loop_counter);
            Stmt::loop_counter = 0;
        }
    }

    namespace _FUNCBLOCKEND {
        void _() {
            Stmt::loop_counter = Stmt::loop_counter_stack.top();
            Stmt::loop_counter_stack.pop();
        }
    }

    namespace _FUNCBODY {
        void _FUNCBLOCKSTART_BLOCK_FUNCBLOCKEND(unsigned& FUNCBODY) {
            FUNCBODY = Symbol::curr_scope_offset();
            Symbol::exit_scope_space();
        }
    }

    namespace _FUNCDEF {
        void _FUNCPREFIX_FUNCARGS_FUNCBODY(Symbol*& FUNCDEF, Symbol* FUNCPREFIX, const unsigned FUNCBODY) {
            Symbol::exit_scope_space();
            FUNCPREFIX->set_total_locals(FUNCBODY);
            Symbol::restore_curr_scope_offset(Symbol::scope_offset_stack.top());
            Symbol::scope_offset_stack.pop();
            FUNCDEF = FUNCPREFIX;
            Quad::emit(Quad::IOpCode::FUNCEND, new Expr::Program_Func(FUNCPREFIX), nullptr, nullptr);
            assert(!Stmt::func_start_jump_stack.empty());
            Quad::patchlabel(Stmt::func_start_jump_stack.top(), Quad::Base::curr_quad_n);
            Stmt::func_start_jump_stack.pop();
            assert(!Stmt::return_jump_stack.empty());
            for(auto&& label : Stmt::return_jump_stack.top()) Quad::patchlabel(label, Quad::Base::curr_quad_n-1);
            Stmt::return_jump_stack.pop();
            --Function::curr_function_depth;
        }
    }
    namespace _OBJECTDEF {
        void _LEFTSQUAREBRACKET_ELIST_RIGHTSQUAREBRACKET(Expr::New_Table*& OBJECTDEF, EList* ELIST) {
            assert(ELIST);
            OBJECTDEF = new Expr::New_Table();
            Quad::emit(Quad::IOpCode::TABLECREATE, OBJECTDEF, nullptr, nullptr);
            unsigned int i = 0;
            const auto& temp_list = *ELIST;
            for(auto& e : temp_list) {
                Quad::emit(Quad::IOpCode::TABLESETELEM, OBJECTDEF, new Expr::Const_Num(i++), e);
            }
        }
        void _LEFTSQUAREBRACKET_INDEXED_RIGHTSQUAREBRACKET(Expr::New_Table*& OBJECTDEF, IndexedList* INDEXED) {
            assert(INDEXED);
            OBJECTDEF = new Expr::New_Table();
            Quad::emit(Quad::IOpCode::TABLECREATE, OBJECTDEF, nullptr, nullptr);
            const auto& temp_list = *INDEXED;
            for(auto& i : temp_list) {
                Quad::emit(Quad::IOpCode::TABLESETELEM, OBJECTDEF, i->first, i->second);
            }
        }
    }

    namespace _INDEXED {
        void _INDEXEDELEM_KOMMA_INDEXED(IndexedList*& INDEXED, IndexedElem* INDEXEDELEM, IndexedList* INDEXED_OLD) {
            assert(INDEXEDELEM && INDEXED_OLD);
            INDEXED_OLD->push_front(INDEXEDELEM);
            INDEXED = INDEXED_OLD;
        }
        void _INDEXEDELEM(IndexedList*& INDEXED, IndexedElem* INDEXEDELEM){
            assert(INDEXEDELEM);
            INDEXED = new IndexedList();
            INDEXED->push_front(INDEXEDELEM);
        }
    }

    namespace _INDEXEDELEM {
        void _LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN(Expr::Base* EXPR_1) {
            Quad::emit_assignquads(EXPR_1);
        }
        void _LEFTCURLYBRACKET_EXPR_DOTUPANDDOWN_EXPR_RIGHTCURLYBRACKET(IndexedElem*& INDEXEDELEM, Expr::Base* EXPR_1, Expr::Base* EXPR_2){
            assert(EXPR_1 && EXPR_2);
            Quad::emit_assignquads(EXPR_2);
            INDEXEDELEM = new IndexedElem(EXPR_1, EXPR_2);
        }
    }
    namespace _BLOCK {
        void _LEFTCURLYBRACKET_STMTS_RIGHTCURLYBRACKET(Stmt& BLOCK, Stmt& STMTS) {
            BLOCK = STMTS;
        }
    }
    namespace _ELIST {
        void _(EList*& ELIST) {
            ELIST = new EList();
        }
        void _EXPR(EList*& ELIST, Expr::Base* EXPR) {
            ELIST = new EList();
            Quad::emit_assignquads(EXPR);
            ELIST->push_front(EXPR);
        }
        void _EXPR_KOMMA_ELIST(EList*& ELIST, Expr::Base* EXPR, EList* ELIST_OLD) {
            assert(EXPR && ELIST_OLD);
            Quad::emit_assignquads(EXPR);
            ELIST_OLD->push_front(EXPR);
            ELIST = ELIST_OLD;
        }
    }

    namespace _TERM {
        void _MINUS_EXPR(Expr::Base*& TERM, Expr::Base* EXPR) {
            assert(EXPR);
            EXPR->is_arith();
            TERM = EXPR->is_temp_expr() ? new Expr::Arith_Expr(EXPR->get_sym()) : new Expr::Arith_Expr();
            Quad::emit(Quad::IOpCode::MUL, TERM, EXPR, new Expr::Const_Num(-1));
        }
        void _NOT_EXPR(Expr::Base*& TERM, Expr::Base* EXPR) {
            assert(EXPR);
            if(EXPR->get_type() != Expr::Expr_t::BOOLEXPR_E) {
                _EXPR::_EXPR_DOUBLEEQUAL_EXPR(EXPR, EXPR, new Expr::Const_Bool(true));
            }
            TERM = EXPR->is_temp_expr() ? new Expr::Bool_Expr(EXPR->get_sym()) : new Expr::Bool_Expr();
            TERM->set_truelist(EXPR->get_falselist());
            TERM->set_falselist(EXPR->get_truelist());
        }
        void _LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(Expr::Base*& TERM, Expr::Base* EXPR){
            assert(EXPR);
            TERM = EXPR;
        }
        void _PLUSPLUS_LVALUE(Expr::Base*& TERM, Expr::LValue* LVALUE) {
            assert(LVALUE);
            LVALUE->is_arith();
            if(LVALUE->get_type() == Expr::Expr_t::TABLEITEM_E) {
               TERM = Quad::emit_iftableitem(LVALUE);
               Quad::emit(Quad::IOpCode::ADD, TERM, TERM, new Expr::Const_Num(1));
               Quad::emit(Quad::IOpCode::TABLESETELEM, LVALUE, LVALUE->get_index(), TERM);
            } else {
               Quad::emit(Quad::IOpCode::ADD, LVALUE, LVALUE, new Expr::Const_Num(1));
               TERM = new Expr::Arith_Expr();
               Quad::emit(Quad::IOpCode::ASSIGN, TERM, LVALUE, nullptr);
            }
        }
        void _LVALUE_PLUSPLUS(Expr::Base*& TERM, Expr::LValue* LVALUE) {
            assert(LVALUE);
            LVALUE->is_arith();
            TERM = new Expr::Var();
            assert(TERM);
            if(LVALUE->get_type() == Expr::Expr_t::TABLEITEM_E) {
                Expr::Base* val = Quad::emit_iftableitem(LVALUE);
                Quad::emit(Quad::IOpCode::ASSIGN, TERM, val, nullptr);
                Quad::emit(Quad::IOpCode::ADD, val, val, new Expr::Const_Num(1));
                Quad::emit(Quad::IOpCode::TABLESETELEM, LVALUE, LVALUE->get_index(), val);
            } else {
                Quad::emit(Quad::IOpCode::ASSIGN, TERM, LVALUE, nullptr);
                Quad::emit(Quad::IOpCode::ADD, LVALUE, LVALUE, new Expr::Const_Num(1));
            }
        }
        void _MINUSMINUS_LVALUE(Expr::Base*& TERM, Expr::LValue* LVALUE) {
            assert(LVALUE);
            LVALUE->is_arith();
            if(LVALUE->get_type() == Expr::Expr_t::TABLEITEM_E) {
               TERM = Quad::emit_iftableitem(LVALUE);
               Quad::emit(Quad::IOpCode::SUB, TERM, TERM, new Expr::Const_Num(1));
               Quad::emit(Quad::IOpCode::TABLESETELEM, LVALUE, LVALUE->get_index(), TERM);
            } else {
               Quad::emit(Quad::IOpCode::SUB, LVALUE, LVALUE, new Expr::Const_Num(1));
               TERM = new Expr::Arith_Expr();
               Quad::emit(Quad::IOpCode::ASSIGN, TERM, LVALUE, nullptr);
            }
        }
        void _LVALUE_MINUSMINUS(Expr::Base*& TERM, Expr::LValue* LVALUE) {
            assert(LVALUE);
            LVALUE->is_arith();
            TERM = new Expr::Var();
            assert(TERM);
            if(LVALUE->get_type() == Expr::Expr_t::TABLEITEM_E) {
                Expr::Base* val = Quad::emit_iftableitem(LVALUE);
                Quad::emit(Quad::IOpCode::ASSIGN, TERM, val, nullptr);
                Quad::emit(Quad::IOpCode::SUB, val, val, new Expr::Const_Num(1));
                Quad::emit(Quad::IOpCode::TABLESETELEM, LVALUE, LVALUE->get_index(), val);
            } else {
                Quad::emit(Quad::IOpCode::ASSIGN, TERM, LVALUE, nullptr);
                Quad::emit(Quad::IOpCode::SUB, LVALUE, LVALUE, new Expr::Const_Num(1));
            }
        }
        void _PRIMARY(Expr::Base*& TERM, Expr::Base* PRIMARY) {
            assert(PRIMARY);
            TERM = Quad::emit_iftableitem(PRIMARY);
        }
    }

    namespace _PRIMARY {
        void _LVALUE(Expr::Base*& PRIMARY, Expr::LValue* LVALUE) {
            assert(LVALUE);
            PRIMARY = LVALUE;
        }
        void _CALL(Expr::Base*& PRIMARY, Expr::Base* CALL) {
            assert(CALL);
            PRIMARY = CALL;
        }
        void _OBJECTDEF(Expr::Base*& PRIMARY, Expr::New_Table* OBJECTDEF){
            assert(OBJECTDEF);
            PRIMARY = OBJECTDEF;
        }
        void _LEFTPARENTHESIS_FUNCDEF_RIGHTPARENTHESIS(Expr::Base*& PRIMARY, Symbol* FUNCDEF) {
            assert(FUNCDEF);
            PRIMARY = new Expr::Program_Func(FUNCDEF);
        }
        void _CONST(Expr::Base*& PRIMARY, Expr::Const* CONST) {
            assert(CONST);
            PRIMARY = CONST;
        };
    }

    namespace _CONST {
        void _INTCONST(Expr::Const*& CONST, int INTVAL){
            CONST = new Expr::Const_Num(INTVAL);
        }
        void _REALCONST(Expr::Const*& CONST, double REALCONST){
            CONST = new Expr::Const_Num(REALCONST);
        }
        void _STRING(Expr::Const*& CONST, const char* STRING){
            CONST = new Expr::Const_String(static_cast<std::string>(STRING));
        }
        void _NIL(Expr::Const*& CONST) {
            CONST = new Expr::Nil();
        }
        void _TRUE(Expr::Const*& CONST){
            CONST = new Expr::Const_Bool(true);
        }
        void _FALSE(Expr::Const*& CONST){
            CONST = new Expr::Const_Bool(false);
        }
    }

    namespace _ASSIGNEXPR {
        void _LVALUE_EQUAL_EXPR(Expr::Assign_Expr*& ASSIGNEXPR, Expr::LValue* LVALUE, Expr::Base* EXPR) {
            assert(LVALUE);
            Quad::emit_assignquads(EXPR);
            if(LVALUE->get_sym()->get_is_func()) compile_error(LVALUE->get_sym()->get_name() + " IS A FUNCTION");
            if(LVALUE->get_type() == Expr::Expr_t::TABLEITEM_E){
                emit(Quad::IOpCode::TABLESETELEM, LVALUE, LVALUE->get_index(), EXPR);
                ASSIGNEXPR = static_cast<Expr::Assign_Expr*>(Quad::emit_iftableitem(LVALUE));
                assert(ASSIGNEXPR);
            } else {
                emit(Quad::IOpCode::ASSIGN, LVALUE, EXPR, nullptr);
                ASSIGNEXPR = new Expr::Assign_Expr();
                emit(Quad::IOpCode::ASSIGN, ASSIGNEXPR, LVALUE, nullptr);
            }
        }
    }

    namespace _EXPR {
        static inline void _EXPR_RELOP_EXPR(Quad::IOpCode relop, Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Bool_Expr();
            Quad::emit(relop, 0U, EXPR_1, EXPR_2);
            EXPR->set_truelist(Quad::Base::curr_quad_n-1);
            assert(Quad::Base::curr_quad_n-1 == Quad::Base::quad_list.size());
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
            EXPR->set_falselist(Quad::Base::curr_quad_n-1);
        }
        void _ASSIGNEXPR(Expr::Base*& EXPR, Expr::Base* ASSIGNEXPR) {
            assert(ASSIGNEXPR);
            EXPR = ASSIGNEXPR;
        }
        void _EXPR_PLUS_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Arith_Expr();
            emit(Quad::IOpCode::ADD, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_MINUS_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2){
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Arith_Expr();
            emit(Quad::IOpCode::SUB, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_MUL_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Arith_Expr();
            emit(Quad::IOpCode::MUL, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_DEVIDE_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Arith_Expr();
            emit(Quad::IOpCode::DIV, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_MOD_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            EXPR = new Expr::Arith_Expr();
            emit(Quad::IOpCode::MOD, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_GREATER_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_GREATER, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_GREATEROREQUAL_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_GREATEREQ, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_LESSER_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_LESS, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_LESSEROREQUAL_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_LESSEQ, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_DOUBLEEQUAL_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_EQ, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_NOTEQUAL_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, Expr::Base* EXPR_2) {
            _EXPR_RELOP_EXPR(Quad::IOpCode::IF_NOTEQ, EXPR, EXPR_1, EXPR_2);
        }
        void _EXPR_BOOLOP(Expr::Base*& EXPR_1) {
            if(EXPR_1->get_type() != Expr::Expr_t::BOOLEXPR_E) {
                _EXPR_DOUBLEEQUAL_EXPR(EXPR_1, EXPR_1, new Expr::Const_Bool(true));
            }
        }
        void _EXPR_AND_M_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, unsigned M, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            if(EXPR_2->get_type() != Expr::Expr_t::BOOLEXPR_E) {
                _EXPR_DOUBLEEQUAL_EXPR(EXPR_2, EXPR_2, new Expr::Const_Bool(true));
            }
            EXPR = new Expr::Bool_Expr();
            Quad::backpatch(EXPR_1->get_truelist(), M);
            EXPR->set_truelist(EXPR_2->get_truelist());
            EXPR->set_falselist(Quad::merge(EXPR_1->get_falselist(), EXPR_2->get_falselist()));
        }
        void _EXPR_OR_M_EXPR(Expr::Base*& EXPR, Expr::Base* EXPR_1, unsigned M, Expr::Base* EXPR_2) {
            assert(EXPR_1 && EXPR_2);
            if(EXPR_2->get_type() != Expr::Expr_t::BOOLEXPR_E) {
                _EXPR_DOUBLEEQUAL_EXPR(EXPR_2, EXPR_2, new Expr::Const_Bool(true));
            }
            EXPR = new Expr::Bool_Expr();
            Quad::backpatch(EXPR_1->get_falselist(), M);
            EXPR->set_truelist(Quad::merge(EXPR_1->get_truelist(), EXPR_2->get_truelist()));
            EXPR->set_falselist(EXPR_2->get_falselist());
        }
        void _TERM(Expr::Base*& EXPR, Expr::Base* TERM) {
            assert(TERM);
            EXPR = TERM;
        }
    }
    namespace _IDLIST {
        void _(){

        }
        void _IDENT_KOMMA_IDLIST(const char* IDENT){
            if(Symbol::is_library_function(IDENT)) compile_error("FORMAL ARGUMENT " + static_cast<std::string>(IDENT) + " IS A LIBRARY FUNCTION");
            else if(!Function::curr_function->add_argument(IDENT)){}
            else {Symbol::shadow_key(IDENT); Symbol::Insert(IDENT, new Variable(IDENT, Symbol::Type::FORMAL_ARGUMENT));}
        }
        void _IDENT(const char* IDENT){
            if(Symbol::is_library_function(IDENT)) compile_error("FORMAL ARGUMENT " + static_cast<std::string>(IDENT) + " IS A LIBRARY FUNCTION");
            else if(!Function::curr_function->add_argument(IDENT)){}
            else {Symbol::shadow_key(IDENT); Symbol::Insert(IDENT, new Variable(IDENT, Symbol::Type::FORMAL_ARGUMENT));}
        }
    }
    namespace _IFPREFIX {
        void _IF_LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(unsigned& IFPREFIX, Expr::Base* EXPR) {
            Quad::emit_assignquads(EXPR);
            Quad::emit(Quad::IOpCode::IF_EQ, Quad::Base::curr_quad_n+2, new Expr::Const_Bool(true), EXPR);
            IFPREFIX = Quad::Base::curr_quad_n;
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
        }
    }
    namespace _ELSEPREFIX {
        void _ELSE(unsigned& ELSEPREFIX){
            ELSEPREFIX = Quad::Base::curr_quad_n;
            emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
        }
    }
    namespace _IFSTMT {
       void _IFPREFIX_STMT(Stmt& IFSTMT, const unsigned IFPREFIX, Stmt& STMT) {
           IFSTMT = STMT;
           Quad::patchlabel(IFPREFIX);
       }
       void _IFPREFIX_STMT_ELSEPREFIX_STMT(Stmt& IFSTMT, const unsigned IFPREFIX, Stmt& STMT1, const unsigned ELSEPREFIX, Stmt& STMT2){
           Quad::patchlabel(IFPREFIX, ELSEPREFIX+1);
           Quad::patchlabel(ELSEPREFIX);
           IFSTMT.set_contlist(Quad::mergelist(STMT1.get_contlist(), STMT2.get_contlist()));
           IFSTMT.set_breaklist(Quad::mergelist(STMT1.get_breaklist(), STMT2.get_breaklist()));
       }
    }
    namespace _LOOPSTART {
        void _() { 
            ++Stmt::loop_counter;
        }
    }
    namespace _LOOPEND {
        void _() {
            --Stmt::loop_counter;
        }
    }
    namespace _LOOPSTMT {
        void _LOOPSTART_STMT_LOOPEND(Stmt& LOOPSTMT, Stmt& STMT) {
            LOOPSTMT = STMT;
        }
    }
    namespace _WHILESTART {
        void _WHILE(unsigned& WHILESTART) {
            WHILESTART = Quad::Base::curr_quad_n;
        }
    }
    namespace _WHILECOND {
        void _LEFTPARENTHESIS_EXPR_RIGHTPARENTHESIS(unsigned& WHILECOND, Expr::Base* EXPR) {
            Quad::emit_assignquads(EXPR);
            Quad::emit(Quad::IOpCode::IF_EQ, Quad::Base::curr_quad_n+2, EXPR, new Expr::Const_Bool(true));
            WHILECOND = Quad::Base::curr_quad_n;
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
        }
    }
    namespace _WHILESTMT {
        void _WHILESTART_WHILECOND_LOOPSTMT(const unsigned WHILESTART, const unsigned WHILECOND, Stmt& LOOPSTMT) {
            Quad::emit(Quad::IOpCode::JUMP, WHILESTART, nullptr, nullptr);
            Quad::patchlabel(WHILECOND);
            Quad::patchlist(LOOPSTMT.get_breaklist(), Quad::Base::curr_quad_n);
            Quad::patchlist(LOOPSTMT.get_contlist(), WHILESTART);
        }
    }
    namespace _N {
        void _(unsigned& N) {
            N = Quad::Base::curr_quad_n;
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
        }
    }
    namespace _M {
        void _(unsigned& M) {
            M = Quad::Base::curr_quad_n;
        }
    }
    namespace _FORPREFIX {
        void _FOR_LEFTPARENTHESIS_ELIST_SEMICOLON_M_EXPR_SEMICOLON(For_Prefix& FORPREFIX, unsigned M, Expr::Base* EXPR) {
            FORPREFIX.set_test(M);
            Quad::emit_assignquads(EXPR);
            FORPREFIX.set_enter(Quad::Base::curr_quad_n);
            Quad::emit(Quad::IOpCode::IF_EQ, 0U, EXPR, new Expr::Const_Bool(true));
        }
    }
    namespace _FORSTMT {
        void _FORPREFIX_N_ELIST_RIGHTPARENTHESIS_N_LOOPSTMT_N(const For_Prefix& FORPREFIX, const unsigned N1, const unsigned N2, Stmt& LOOPSTMT, const unsigned N3) {
            Quad::patchlabel(FORPREFIX.get_enter(), N2+1);
            Quad::patchlabel(N1);
            Quad::patchlabel(N2, FORPREFIX.get_test());
            Quad::patchlabel(N3, N1+1);
            Quad::patchlist(LOOPSTMT.get_breaklist(), Quad::Base::curr_quad_n);
            Quad::patchlist(LOOPSTMT.get_contlist(), N1 + 1);
        }
    }
    namespace _RETURNSTMT {
        void _RETURN_EXPR_SEMICOLON(Expr::Base* EXPR) {
            if(!Symbol::curr_function_depth) compile_error("RETURN OUTSIDE OF FUNCTION", true);
            Quad::emit_assignquads(EXPR);
            Quad::emit(Quad::IOpCode::ASSIGN, Expr::Var::_result, EXPR, nullptr);
            Stmt::return_jump_stack.top().push_back(Quad::Base::curr_quad_n);
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
            Temp_Variable::reset_temp();
        }
        void _RETURN_SEMICOLON() {
            if(!Symbol::curr_function_depth) compile_error("RETURN OUTSIDE OF FUNCTION", true);
            Stmt::return_jump_stack.top().push_back(Quad::Base::curr_quad_n);
            Quad::emit(Quad::IOpCode::JUMP, 0U, nullptr, nullptr);
            Temp_Variable::reset_temp();
        }
    }
}