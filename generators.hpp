#include "tcode_types.hpp"
#include <functional>

void clean_up();

namespace Generate {
    typedef std::function<void(const Quad::Base*)> Generator_Func;
    extern const std::unordered_map<Quad::IOpCode, Generator_Func> generators;
    void ADD(const Quad::Base*);
    void SUB(const Quad::Base*);
    void MUL(const Quad::Base*);
    void DIV(const Quad::Base*);
    void MOD(const Quad::Base*);
    void NEWTABLE(const Quad::Base*);
    void TABLEGETELEM(const Quad::Base*);
    void TABLESETELEM(const Quad::Base*);
    void ASSIGN(const Quad::Base*);
    void NOP(const Quad::Base*);
    void JUMP(const Quad::Base*);
    void IF_EQ(const Quad::Base*);
    void IF_NOTEQ(const Quad::Base*);
    void IF_GREATER(const Quad::Base*);
    void IF_GREATER_EQ(const Quad::Base*);
    void IF_LESS(const Quad::Base*);
    void IF_LESSEQ(const Quad::Base*);
    void PARAM(const Quad::Base*);
    void CALL(const Quad::Base*);
    void GETRETVAL(const Quad::Base*);
    void FUNCSTART(const Quad::Base*);
    void RETURN(const Quad::Base*);
    void FUNCEND(const Quad::Base*);

    void emit(TCODE::Instruction*);
    void generate_all(void);
}