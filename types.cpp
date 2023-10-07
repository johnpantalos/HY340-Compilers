#include "types.hpp"

namespace Expr {
    std::unordered_set<Base*> Base::all_exprs;
    LValue* to_lvalue(Symbol* sym){
        assert(sym);
        LValue* lval = nullptr;
        if(Symbol::is_library_function(sym->get_name())) lval = new Library_Func(sym);
        else if (sym->get_is_func()) lval = new Program_Func(sym);
        else lval = new Var(sym);
        return lval;
    }
    
    define_E_op_E(+);
    define_E_op_E(-);
    define_E_op_E(*);
    define_E_op_E(/);
    define_E_op_E(>);
    define_E_op_E(<);
    define_E_op_E(==);
    define_E_op_E(!=);
    define_E_op_E(>=);
    define_E_op_E(<=);

    Var* Var::_result = new Var(new Variable("_result", Symbol::Type::GLOBAL_VARIABLE));
}

unsigned Stmt::loop_counter = 0;
std::stack<decltype(Stmt::loop_counter)> Stmt::loop_counter_stack;

std::stack<unsigned> Stmt::func_start_jump_stack;

std::stack<std::list<unsigned>> Stmt::return_jump_stack;

std::unordered_set<Call*> Call::all_calls;
