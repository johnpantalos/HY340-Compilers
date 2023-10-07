#include"executors.hpp"
#define do_comparison(m1, m2, op) \
switch (m1.get_type()) {\
    case AVM::Memcell_t::NUMBER_M: return m1.get_num() op m2.get_num();\
    case AVM::Memcell_t::STRING_M: return m1.get_str() op m2.get_str();\
    default:    assert(false); return false;\
}


namespace Execute {
    typedef std::function<void(Machine_Code::Instruction&)> Executor_Func;
    const std::unordered_map<Machine_Code::VMOpCode, Executor_Func> executors = {
        {Machine_Code::VMOpCode::ADD_V, ADD}, {Machine_Code::VMOpCode::SUB_V, SUB}, {Machine_Code::VMOpCode::MUL_V, MUL},
        {Machine_Code::VMOpCode::DIV_V, DIV}, {Machine_Code::VMOpCode::MOD_V, MOD}, {Machine_Code::VMOpCode::NEWTABLE_V, NEWTABLE},
        {Machine_Code::VMOpCode::TABLEGETELEM_V, TABLEGETELEM}, {Machine_Code::VMOpCode::TABLESETELEM_V, TABLESETELEM},
        {Machine_Code::VMOpCode::ASSIGN_V, ASSIGN}, {Machine_Code::VMOpCode::NOP_V, NOP}, {Machine_Code::VMOpCode::JUMP_V, JUMP},
        {Machine_Code::VMOpCode::JEQ_V, JEQ}, {Machine_Code::VMOpCode::JNE_V, JNE}, {Machine_Code::VMOpCode::JGT_V, JGT},
        {Machine_Code::VMOpCode::JGE_V, JGE}, {Machine_Code::VMOpCode::JLT_V, JLT},
        {Machine_Code::VMOpCode::JLE_V, JLE}, {Machine_Code::VMOpCode::PUSHARG_V, PUSHARG}, {Machine_Code::VMOpCode::CALL_V, CALL},
        {Machine_Code::VMOpCode::FUNCENTER_V, FUNCENTER}, {Machine_Code::VMOpCode::FUNCEXIT_V, FUNCEXIT}
    };
    typedef std::function<double(double, double)> ArithmeticFunc;
    const std::map<Machine_Code::VMOpCode, ArithmeticFunc> arithmetc_funcs =
    {
    {Machine_Code::VMOpCode::ADD_V, [](double x, double y) -> double {
        return x+y;
    }},
    {Machine_Code::VMOpCode::SUB_V, [](double x, double y) -> double {
        return x-y;
    }},
    {Machine_Code::VMOpCode::MUL_V, [](double x, double y) -> double {
        return x*y;
    }},
    {Machine_Code::VMOpCode::DIV_V, [](double x, double y) -> double {
        return x/y;
    }},
    {Machine_Code::VMOpCode::MOD_V, [](double x, double y) -> double {
        int int_x = static_cast<int>(x), int_y = static_cast<int>(y);
        if(int_x != x || int_y != y) AVM::error("INVALID MOD OPERATION");
        return int_x % int_y;
    }}
    };
    void _ARITHMETIC(Machine_Code::Instruction& inst) {
        auto& lv = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax); assert(lv);
        auto& rv1 = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax); assert(rv1);
        auto& rv2 = AVM::translate_operand(*inst.get_arg2(), AVM::Memcell::Base::bx); assert(rv2);
        if(rv1->get_type() != AVM::Memcell_t::NUMBER_M || rv2->get_type() != AVM::Memcell_t::NUMBER_M) {
            AVM::error("ARITHMETIC OPERATION WITH NON-NUMBER");
            Info::finished = true;
        }
        if(lv && lv != rv1 && lv != rv2) delete lv;
        lv = new AVM::Memcell::Number(arithmetc_funcs.at(inst.get_opcode())(rv1->get_num(), rv2->get_num()));

    } 
    void ADD(Machine_Code::Instruction& inst) { _ARITHMETIC(inst); }
    void SUB(Machine_Code::Instruction& inst) { _ARITHMETIC(inst); }
    void MUL(Machine_Code::Instruction& inst) { _ARITHMETIC(inst); }
    void DIV(Machine_Code::Instruction& inst) { _ARITHMETIC(inst); }
    void MOD(Machine_Code::Instruction& inst) { _ARITHMETIC(inst); }
    void NEWTABLE(Machine_Code::Instruction& inst) {
        auto& lv = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax);
        assert(lv);
        delete lv;
        lv = new AVM::Memcell::Table(new AVM::Table());
        lv->get_table()->inc_ref_counter();
    }
    void TABLEGETELEM(Machine_Code::Instruction& inst) {
        auto& lv = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax);
        auto& t = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        auto& i = AVM::translate_operand(*inst.get_arg2(), AVM::Memcell::Base::ax);
        assert(lv);
        assert(t);
        assert(i);
        delete lv;
        lv = new AVM::Memcell::Nil();
        _print_stack();
         if(t->get_type() != AVM::Memcell_t::TABLE_M) {
            AVM::error("ILLEGAL TO USE TYPE " + AVM::Memcell::Base::type_strings.at(t->get_type()) + " AS TABLE");
            Info::finished = true;
        } else {
            auto content = t->get_table()->get_elem(i);
            if(content) {
                AVM::assign(lv, content);
            } else {
                AVM::warning("KEY NOT FOUND IN TABLE");
            }
        }
    }
    void TABLESETELEM(Machine_Code::Instruction& inst) {
        auto& t = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax);
        auto& i = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        auto& c = AVM::translate_operand(*inst.get_arg2(), AVM::Memcell::Base::bx);
        assert(t);
        assert(i);
        assert(c);
        if(t->get_type() != AVM::Memcell_t::TABLE_M) {
            AVM::error("ILLEGAL TO USE TYPE " + AVM::Memcell::Base::type_strings.at(t->get_type()) + " AS TABLE");
            Info::finished = true;
        } else {
            t->get_table()->set_elem(i, c);
        }
    }
    void ASSIGN(Machine_Code::Instruction& inst) {
        auto& stack = AVM::Memcell::Base::stack;
        auto& top = AVM::Memcell::Base::top;
        auto& retval = AVM::Memcell::Base::retval;
        AVM::Memcell::Base*& lv = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax);
        AVM::Memcell::Base*& rv = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        assert(rv);
        assert(lv);
        AVM::assign(lv, rv);
    }
    void NOP(Machine_Code::Instruction& inst) {}
    void JUMP(Machine_Code::Instruction& inst) {
        assert(inst.get_result()->get_type() == Machine_Code::VMArg_t::LABEL_A);
        Info::pc = inst.get_result()->get_val();
    }
    void _BRANCH(Machine_Code::Instruction& inst, std::function<bool(AVM::Memcell::Base&, AVM::Memcell::Base&)> comp) {
        assert(inst.get_result()->get_type() == Machine_Code::VMArg_t::LABEL_A);
        auto& rv1 = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        auto& rv2 = AVM::translate_operand(*inst.get_arg2(), AVM::Memcell::Base::bx);
        bool result = false;
        if(rv1->get_type() == AVM::Memcell_t::UNDEF_M || rv2->get_type() == AVM::Memcell_t::UNDEF_M) {
            AVM::error("'UNDEF' IN LOGICAL OPERATION");
            Info::finished = true;
            return;
        }
        if(rv1->get_type() == AVM::Memcell_t::NIL_M || rv2->get_type() == AVM::Memcell_t::NIL_M) {
            result = rv1->get_type() == rv2->get_type();
        } else if(rv1->get_type() == AVM::Memcell_t::BOOL_M || rv2->get_type() == AVM::Memcell_t::BOOL_M){
            result = rv1->to_bool() == rv2->to_bool();
        } else if(rv1->get_type() != rv2->get_type()){
            auto& type_strings = AVM::Memcell::Base::type_strings;
            AVM::error("CANNOT COMPARE " + type_strings.at(rv1->get_type()) + " AND " + type_strings.at(rv2->get_type()));
            Info::finished = true;
        } else {
            result = comp(*rv1, *rv2);
        }
        if(Info::finished || result) {
            Info::pc = inst.get_result()->get_val();
        }
    }
    void JEQ(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, ==);
        });
    }
    void JNE(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, !=);
        });
    }
    void JGT(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, >);
        });
    }
    void JGE(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, >=);
        });
    }
    void JLT(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, <);
        });
    }
    void JLE(Machine_Code::Instruction& inst) {
        _BRANCH(inst, [](AVM::Memcell::Base& m1, AVM::Memcell::Base& m2) -> bool {
            do_comparison(m1, m2, <=)
        });
    }
    void PUSHARG(Machine_Code::Instruction& inst) {
        auto& stack = AVM::Memcell::Base::stack;
        auto& top = AVM::Memcell::Base::top;
        auto*& arg = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        assert(arg);
        AVM::assign(stack.at(top), arg);
        ++AVM::Memcell::Base::total_actuals;
        dec_top();
    }
    void CALL(Machine_Code::Instruction& inst) {
        auto& func = AVM::translate_operand(*inst.get_arg1(), AVM::Memcell::Base::ax);
        assert(func);
        switch(func->get_type()) {
            case AVM::Memcell_t::USERFUNC_M:
                callsaveenvironment();
                Info::pc = func->get_userfunc();
                break;
            case AVM::Memcell_t::STRING_M:
                callsaveenvironment();
                Library::call_libfunc(func->get_str());
                break;
            case AVM::Memcell_t::LIBFUNC_M:
                callsaveenvironment();
                Library::call_libfunc(func->get_libfunc());
                break;
            case AVM::Memcell_t::TABLE_M:
                if(func->has_functor()) {
                    AVM::assign(AVM::Memcell::Base::stack.at(AVM::Memcell::Base::top), func);
                    ++AVM::Memcell::Base::total_actuals;
                    dec_top();
                    callsaveenvironment();
                    Info::pc = func->get_functor();
                } else {
                    AVM::error("OBJECT DOES NOT SUPPORT FUNCTOR");
                }
                break;
            default:
                AVM::error("FUNCTION NOT FOUND");
                
        }
    }
    void GETRETVAL(Machine_Code::Instruction& inst) {}
    void FUNCENTER(Machine_Code::Instruction& inst) {
        auto& func = AVM::translate_operand(*inst.get_result(), AVM::Memcell::Base::ax);
        assert(func);
        assert(Info::pc == func->get_userfunc());
        AVM::Memcell::Base::total_actuals = 0;
        Machine_Code::User_Func& func_info = Machine_Code::user_funcs.at(inst.get_result()->get_val());
        AVM::Memcell::Base::topsp = AVM::Memcell::Base::top;
        AVM::Memcell::Base::top = AVM::Memcell::Base::top - func_info.get_local_size();
    }
    void RETURN(Machine_Code::Instruction& inst) {}
    void FUNCEXIT(Machine_Code::Instruction& inst) {
        auto& top = AVM::Memcell::Base::top;
        auto& topsp = AVM::Memcell::Base::topsp;
        auto& pc = Info::pc;
        unsigned oldTop = top;
        top = get_envvalue(topsp + AVM::SAVEDTOP_OFFSET);
        pc = get_envvalue(topsp + AVM::SAVEDPC_OFFSET);
        topsp = get_envvalue(topsp + AVM::SAVEDTOPSP_OFFSET);
        while(oldTop++ < top)
            AVM::Memcell::Base::stack.at(oldTop) = new AVM::Memcell::Undef(); 
    }

    bool Info::finished = false;
    unsigned Info::pc = 0;
    unsigned Info::AVM_CODE_SIZE = Machine_Code::instructions.size();
    
     void dec_top(void) {
         auto& top = AVM::Memcell::Base::top;
        if(!top) {
            AVM::error("STACK OVERFLOW");
            Info::finished = true;
        } else {
            --top;
        }
    }
    void push_envvalue(unsigned val) {
        auto& stack = AVM::Memcell::Base::stack;
        auto& top = AVM::Memcell::Base::top;
        auto& stack_top = stack.at(top);
        if(stack_top) delete stack_top;
        stack_top = new AVM::Memcell::Number(val);
        dec_top();            
    }
    unsigned get_envvalue(unsigned i) {
        auto& stack = AVM::Memcell::Base::stack;
        if(i >= stack.size()) return 0;
        assert(stack.at(i)->get_type() == AVM::Memcell_t::NUMBER_M);
        unsigned val = static_cast<unsigned>(stack.at(i)->get_num());
        assert(stack.at(i)->get_num() == static_cast<double>(val));
        return val;
    }
    void callsaveenvironment(void) {
        auto& total_actuals = AVM::Memcell::Base::total_actuals;
        auto& top = AVM::Memcell::Base::top;
        auto& topsp = AVM::Memcell::Base::topsp;
        push_envvalue(total_actuals);
        push_envvalue(Info::pc+1);
        push_envvalue(top + total_actuals + 2);
        push_envvalue(topsp);

    }

    void cycle(void) {
        if(Info::finished) return;
        if(Info::pc == Info::AVM_CODE_SIZE) {
            Info::finished = true;
            return;
        }
        assert(Info::pc < Info::AVM_CODE_SIZE);
        Machine_Code::Instruction& inst =  Machine_Code::instructions.at(Info::pc);
        const unsigned old_pc = Info::pc;
        executors.at(inst.get_opcode())(inst);
        if(Info::pc == old_pc) ++Info::pc;
    }
    void restore_stack(void) {
        unsigned oldTop = AVM::Memcell::Base::top;
        AVM::Memcell::Base::top = get_envvalue(AVM::Memcell::Base::topsp + AVM::SAVEDTOP_OFFSET);
        Info::pc = get_envvalue(AVM::Memcell::Base::topsp + AVM::SAVEDPC_OFFSET);
        AVM::Memcell::Base::topsp = get_envvalue(AVM::Memcell::Base::topsp + AVM::SAVEDTOPSP_OFFSET);
        while(oldTop++ < AVM::Memcell::Base::top)
            AVM::Memcell::Base::stack.at(oldTop) = new AVM::Memcell::Undef(); 
    }
    static inline void _init_stack(void) {
        for(unsigned i = 0; i < AVM::Memcell::Base::STACK_SIZE; ++i) {
            AVM::Memcell::Base::stack.emplace(AVM::Memcell::Base::stack.begin()+i, new AVM::Memcell::Undef());
        }
        AVM::Memcell::Base::top = AVM::Memcell::Base::STACK_SIZE - 1 - Machine_Code::total_globals;
        AVM::Memcell::Base::topsp = AVM::Memcell::Base::top;
    }
    static inline void _delete_stack(void) {
        for(auto& elem : AVM::Memcell::Base::stack) {
            if(elem) delete elem;
        }
    }
    void execute_all(void) {
        _init_stack();
        Info::AVM_CODE_SIZE = Machine_Code::instructions.size();
        while(!Info::finished) cycle();
        _delete_stack();
    }
}