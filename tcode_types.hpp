#include"quad.hpp"
#include<string>
#include<map>
#include<cassert>
#include<algorithm>
#include<bitset>
#include<ranges>
#include<functional>

namespace TCODE {
    enum VMOpCode {
        ASSIGN_V,   ADD_V,      SUB_V,
        MUL_V,      DIV_V,      MOD_V,
        UMINUS_V,   JEQ_V,      JNE_V,
        JLE_V,      JGE_V,      JLT_V,
        JGT_V,      CALL_V,     PUSHARG_V,
        NEWTABLE_V, JUMP_V,     TABLEGETELEM_V,
        FUNCENTER_V,            FUNCEXIT_V,
        TABLESETELEM_V,         NOP_V   
    };

    enum VMArg_t {
        LABEL_A = 0,
        GLOBAL_A = 1,
        FORMAL_A = 2,
        LOCAL_A = 3,
        NUMBER_A = 4,
        STRING_A = 5,
        BOOL_A = 6,
        NIL_A = 7,
        USERFUNC_A = 8,
        LIBFUNC_A = 9,
        RETVAL_A = 10
    };

    class User_Func final {
        unsigned taddress;
        unsigned local_size;
        std::string name;
        public:
        unsigned get_taddress(void) const { return this->taddress; }
        void set_taddress(unsigned taddress) { this->taddress = taddress; }
        unsigned get_local_size(void) const { return this->local_size; }
        void set_local_size(unsigned local_size) { this->local_size = local_size; }
        std::string get_name(void) const { return this->name; }
        void set_name(const std::string& name) { this->name = name; }
        User_Func() = default;
        User_Func(unsigned taddress, unsigned local_size, const std::string& name) :
                    taddress(taddress), local_size(local_size), name(name) {}
        bool operator==(const std::string& func2_name) {
            return this->name == func2_name;
        }
        bool operator!=(const User_Func& func2) {
            return this->name != func2.get_name();
        }
    };


    class VMArg final {
        VMArg_t type;
        unsigned val;
        static const std::unordered_map<Symbol::Scope_Space, VMArg_t> space_to_vmarg_t;
        static const std::unordered_map<Expr::Expr_t, VMArg_t> const_expr_to_vmarg_t;
        static const std::unordered_set<Expr::Expr_t> variable_users;
        static const std::unordered_map<Expr::Expr_t, std::function<unsigned(Expr::Const*)>> const_expr_to_vm_arg_funcs;
        static inline bool uses_variable_for_storage(Expr::Base* expr) {
            return variable_users.find(expr->get_type()) != variable_users.end();
        }
        static inline Expr::Const* is_const(Expr::Base* expr) {
            return dynamic_cast<Expr::Const*>(expr);
        }
        public:
        static std::vector<double> num_consts;
        static std::vector<std::string> str_consts;
        static std::vector<User_Func> user_funcs;
        static std::vector<std::string> lib_funcs;
        static inline unsigned userfuncs_newfunc(Symbol* sym) {
            auto&& func = std::find(user_funcs.begin(), user_funcs.end(), sym->get_name());
            if(func != user_funcs.end())
                return func - user_funcs.begin();
            return 777;
        }
        static inline unsigned libfuncs_newused(const std::string& name) {
            auto&& func = std::find(lib_funcs.begin(), lib_funcs.end(), name);
            if(func != lib_funcs.end())
                return func - lib_funcs.begin();
            lib_funcs.push_back(name);
            return lib_funcs.size() - 1;
        }
        VMArg(VMArg_t type, unsigned val) : type(type), val(val) {}
        VMArg(Expr::Base* expr) {
            if(!expr) { 
                this->val = 0;
                this->type = VMArg_t::LABEL_A;
                return;
            }
            if(expr->get_type() == Expr::Expr_t::PROGRAMFUNC_E){
                this->type = VMArg_t::USERFUNC_A;
                this->val = userfuncs_newfunc(expr->get_sym());
            } else if(expr->get_type() == Expr::Expr_t::LIBRARYFUNC_E) {
                this->type = VMArg_t::LIBFUNC_A;
                this->val = libfuncs_newused(expr->get_sym()->get_name());
            } else if(uses_variable_for_storage(expr)) {
                this->type = space_to_vmarg_t.at(expr->get_sym()->get_space());
                this->val = expr->get_sym()->get_offset();
            } else if(Expr::Const* const_expr = is_const(expr)) {
                this->type = const_expr_to_vmarg_t.at(const_expr->get_type());
                this->val = const_expr_to_vm_arg_funcs.at(const_expr->get_type())(const_expr);
            } else {
                assert(false);
            }
        }
        VMArg_t get_type(void) const { return this->type; }
        void set_type(VMArg_t type) { this->type = type; }
        unsigned get_val(void) const { return this->val; }
        void set_val(unsigned val) { this->val = val; }
        std::string get_binary_operand(void) {
            return std::bitset<4>(this->type).to_string() + std::bitset<28>(this->val).to_string();
        }
    };

    class Instruction final {
        VMOpCode opcode;
        VMArg* arg1;
        VMArg* arg2;
        VMArg* result;
        unsigned src_line;
        public:
        static std::vector<Instruction*> instruction_vector;
        static unsigned curr_processed_quad;
        static inline unsigned next_label(void) { return instruction_vector.size(); }
        Instruction() = default;
        /*~Instruction() {
            if(this->arg1) delete this->arg1;
            if(this->arg2) delete this->arg2;
            if(this->result) delete this->result;
        }*/
        Instruction(VMOpCode opcode, VMArg* arg1, VMArg* arg2, VMArg* result, unsigned src_line) :
        opcode(opcode), arg1(arg1), arg2(arg2), result(result), src_line(src_line) {}
        VMOpCode get_opcode(void) const { return this->opcode; }
        void set_opcode(VMOpCode opcode) { this->opcode = opcode; }
        VMArg* get_arg1(void) const { return this->arg1; }
        void set_arg1(VMArg* arg1) { this->arg1 = arg1; }
        VMArg* get_arg2(void) const { return this->arg2; }
        void set_arg2(VMArg* arg2) { this->arg2 = arg2; }
        VMArg* get_result(void) const { return this->result; }
        void set_result(VMArg* result) { this->result = result; }
        unsigned get_src_line(void) const { return this->src_line; }
        static void print_all(std::ostream& os = std::cout) {
            os << std::bitset<32>(instruction_vector.size()).to_string();
            for(auto& inst : instruction_vector) {
                os  << std::bitset<5>(inst->opcode).to_string()
                    << ((inst->result) ? (inst->result->get_binary_operand())  : std::bitset<32>(0).to_string())
                    << ((inst->arg1) ? (inst->arg1->get_binary_operand()) : std::bitset<32>(0).to_string())
                    << ((inst->arg2 && inst->opcode) ? (inst->arg2->get_binary_operand()) : std::bitset<32>(0).to_string());
            }
        }
    };


    class Incomplete_Jump final {
        unsigned instr_no;
        unsigned iaddress;
        public:
        Incomplete_Jump(unsigned instr_no, unsigned iaddress) : instr_no(instr_no), iaddress(iaddress) {}
        unsigned get_instr_no(void) const { return this->instr_no; } 
        unsigned get_iaddress(void) const { return this->iaddress; }
        static std::list<Incomplete_Jump*> incomplete_jump_list;
        static void add(unsigned instr_no, unsigned iaddress) {
            incomplete_jump_list.push_back(new Incomplete_Jump(instr_no, iaddress));
        }
        static void patch_incomplete(void) {
            const unsigned i_code_size = Quad::Base::quad_list.size();
            for(auto& x : incomplete_jump_list) {
                Instruction::instruction_vector.at(x->get_instr_no())->get_result()->set_val(
                    (x->get_iaddress() == i_code_size) ? 
                    i_code_size : Quad::Base::quad_list.at(x->get_iaddress())->get_taddress() // ?
                );
            }
        }
    };

    void print_target_code(std::ostream& = std::cout);
}
