#include "tcode_types.hpp"

namespace TCODE {
    const std::unordered_map<Symbol::Scope_Space, VMArg_t> VMArg::space_to_vmarg_t =
    {{Symbol::Scope_Space::FORMAL_ARG, VMArg_t::FORMAL_A},
    {Symbol::Scope_Space::PROGRAM_VAR, VMArg_t::GLOBAL_A},
    {Symbol::Scope_Space::FUNCTION_LOCAL, VMArg_t::LOCAL_A}};
    
    const std::unordered_map<Expr::Expr_t, VMArg_t> VMArg::const_expr_to_vmarg_t = 
    {{Expr::Expr_t::CONSTBOOL_E, VMArg_t::BOOL_A},
    {Expr::Expr_t::CONSTSTRING_E, VMArg_t::STRING_A},
    {Expr::Expr_t::CONSTNUM_E, VMArg_t::NUMBER_A},
    {Expr::Expr_t::NIL_E, VMArg_t::NIL_A}};

    unsigned Instruction::curr_processed_quad = 0;

    const std::unordered_set<Expr::Expr_t> VMArg::variable_users =
    {Expr::Expr_t::VAR_E, Expr::Expr_t::TABLEITEM_E, Expr::Expr_t::ARITHEXPR_E,
    Expr::Expr_t::BOOLEXPR_E, Expr::Expr_t::ASSIGNEXPR_E, Expr::Expr_t::NEWTABLE_E};
    
    std::vector<double> VMArg::num_consts;
    std::vector<std::string> VMArg::str_consts;
    std::vector<User_Func> VMArg::user_funcs;
    std::vector<std::string> VMArg::lib_funcs;

    static std::string _to_bitset(const std::string& str) {
        std::string bit_str = "";
        bit_str += std::bitset<32>(str.size()).to_string();
        for(auto& c : str) {
            bit_str += std::bitset<8>(c).to_string();
        }
        assert(bit_str.size() == str.size()*8 + 32);
        return bit_str;
    }

    static std::string _to_bitset(double dbl) {
        return std::bitset<64>(*reinterpret_cast<unsigned long long*>(&dbl)).to_string();
    }

    static std::string _to_bitset(const User_Func& func) {
        return (std::bitset<32>(func.get_taddress()).to_string()
                    + std::bitset<32>(func.get_local_size()).to_string()
                    + _to_bitset(func.get_name())); 
    }

    const std::unordered_map<Expr::Expr_t, std::function<unsigned(Expr::Const*)>> VMArg::const_expr_to_vm_arg_funcs = {
        {Expr::Expr_t::CONSTBOOL_E, [](Expr::Const* const_expr) -> unsigned {
            return static_cast<unsigned>(const_expr->get_bool_const());
        }},
        {Expr::Expr_t::CONSTNUM_E, [](Expr::Const* const_expr) -> unsigned {
            auto&& num = std::find(num_consts.begin(), num_consts.end(), const_expr->get_num_const());
            if(num != num_consts.end()) return num - num_consts.begin(); 
            num_consts.push_back(const_expr->get_num_const());
            return num_consts.size() - 1;
        }},
        {Expr::Expr_t::CONSTSTRING_E, [](Expr::Const* const_expr) -> unsigned {
            std::string&& str_const = const_expr->get_str_const();
            str_const.erase(0, 1);
            str_const.erase(str_const.size()-1);
            auto&& str = std::find(str_consts.begin(), str_consts.end(), str_const);
            if(str != str_consts.end()) return str - str_consts.begin(); 
            str_consts.push_back(str_const);
            return str_consts.size() - 1;
        }},
        {Expr::Expr_t::NIL_E, [](Expr::Const* const_expr) -> unsigned {
            return 0;
        }}
    };

    std::vector<Instruction*> Instruction::instruction_vector;

    std::list<Incomplete_Jump*> Incomplete_Jump::incomplete_jump_list;

    template <typename Vector>
    static inline void _print_table(std::ostream& os, const Vector table) {
        os << std::bitset<32>(table.size());
        unsigned i = 0;
        for(auto& item : table) {
            os << std::bitset<32>(i++) << _to_bitset(item);
        }
        os << std::endl;
    }

    void print_target_code(std::ostream& os) {
        os << std::bitset<32>(340200501).to_string();
        os << std::endl;
        unsigned i = 0;
        _print_table(os, VMArg::str_consts);
        _print_table(os, VMArg::num_consts);
        _print_table(os, VMArg::user_funcs);
        _print_table(os, VMArg::lib_funcs);
        Instruction::print_all(os);
    }
}