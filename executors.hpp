#include"lib_funcs.hpp"
#include<functional>

namespace Execute {
    typedef std::function<void(Machine_Code::Instruction&)> Executor_Func;
    extern const std::unordered_map<Machine_Code::VMOpCode, Executor_Func> executors;
    void ADD(Machine_Code::Instruction&);
    void SUB(Machine_Code::Instruction&);
    void MUL(Machine_Code::Instruction&);
    void DIV(Machine_Code::Instruction&);
    void MOD(Machine_Code::Instruction&);
    void NEWTABLE(Machine_Code::Instruction&);
    void TABLEGETELEM(Machine_Code::Instruction&);
    void TABLESETELEM(Machine_Code::Instruction&);
    void ASSIGN(Machine_Code::Instruction&);
    void NOP(Machine_Code::Instruction&);
    void JUMP(Machine_Code::Instruction&);
    void JEQ(Machine_Code::Instruction&);
    void JNE(Machine_Code::Instruction&);
    void JGT(Machine_Code::Instruction&);
    void JGE(Machine_Code::Instruction&);
    void JLT(Machine_Code::Instruction&);
    void JLE(Machine_Code::Instruction&);
    void PUSHARG(Machine_Code::Instruction&);
    void CALL(Machine_Code::Instruction&);
    void FUNCENTER(Machine_Code::Instruction&);
    void FUNCEXIT(Machine_Code::Instruction&);

    class Info final {
        public:
        static bool finished;
        static unsigned pc;
        static unsigned currline;
        static unsigned AVM_CODE_SIZE;
    };

    void dec_top(void);
    void push_envvalue(unsigned);
    unsigned get_envvalue(unsigned);
    void callsaveenvironment(void);
    void restore_stack(void);

    void cycle(void);
    void execute_all(void);
}