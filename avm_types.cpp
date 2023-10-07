#include"avm_types.hpp"

namespace Execute {
    class Info {
        public:
        static unsigned pc;
        static bool finished;
    };
}

namespace AVM {
    void warning(const std::string& message) {
        std::cerr << "\033[1;35mWARNING [PC " << Execute::Info::pc << "]: " << message << "\033[0m" << std::endl;
    }
    void error(const std::string& message) {
        std::cerr << "\033[1;31mERROR [PC " << Execute::Info::pc << "]: " << message << "\033[0m" << std::endl;
        Execute::Info::finished = true;
    }
    namespace Memcell {
        std::unordered_set<Machine_Code::VMArg_t> variable_types = 
        {Machine_Code::VMArg_t::GLOBAL_A, Machine_Code::VMArg_t::FORMAL_A, Machine_Code::VMArg_t::LOCAL_A};
        std::unordered_map<Memcell_t, std::string> Base::type_strings = {
            {Memcell_t::BOOL_M, "BOOL"}, {Memcell_t::LIBFUNC_M, "LIBRARY FUNCTION"},
            {Memcell_t::NIL_M, "NIL"}, {Memcell_t::NUMBER_M, "NUMBER"},
            {Memcell_t::STRING_M, "STRING"}, {Memcell_t::TABLE_M, "TABLE"},
            {Memcell_t::UNDEF_M, "UNDEF"}, {Memcell_t::USERFUNC_M, "USER FUNCTION"}
        };
        std::vector<Base*> Base::stack(Base::STACK_SIZE);
        Base *Base::ax, *Base::bx, *Base::cx, *Base::retval;
        unsigned Base::top, Base::topsp;
        unsigned Base::total_actuals = 0;
    }
    double consts_getnumber(unsigned index) {
        return Machine_Code::num_consts.at(index);
    }
    std::string consts_getstring(unsigned index) {
        return Machine_Code::str_consts.at(index);
    }
    unsigned userfunc_getused(unsigned index) {
        return Machine_Code::user_funcs.at(index).get_taddress();
    }
    std::string libfunc_getused(unsigned index) {
        return Machine_Code::lib_funcs.at(index);
    }
    Memcell::Base*& translate_operand(Machine_Code::VMArg& arg, Memcell::Base*& reg) {
        typedef Machine_Code::VMArg_t VMArg_t;
        auto& stack = Memcell::Base::stack;
        if(arg.get_type() == 1 && arg.get_val() == 0) {
            return Memcell::Base::retval = new Memcell::Nil();
        }
        switch(arg.get_type()) {
            case VMArg_t::GLOBAL_A: return stack.at(Memcell::Base::STACK_SIZE /*- 1*/ - arg.get_val());
            case VMArg_t::LOCAL_A: return stack.at(Memcell::Base::topsp - arg.get_val());
            case VMArg_t::FORMAL_A: return stack.at(Memcell::Base::topsp + Memcell::Base::STACK_ENV_SIZE + 1 + arg.get_val());
            case VMArg_t::RETVAL_A: return Memcell::Base::retval;
            case VMArg_t::NUMBER_A:
                if(reg) delete reg;
                return reg = new Memcell::Number(consts_getnumber(arg.get_val()));
            case VMArg_t::STRING_A:
                if(reg) delete reg;
                return reg = new Memcell::String(consts_getstring(arg.get_val()));
            case VMArg_t::BOOL_A:
                if(reg) delete reg;
                return reg = new Memcell::Bool(static_cast<bool>(arg.get_val()));
            case VMArg_t::NIL_A:
                if(reg) delete reg;
                return reg = new Memcell::Nil();
            case VMArg_t::USERFUNC_A:
                if(reg) delete reg;
                return reg = new Memcell::UserFunc(userfunc_getused(arg.get_val()));
            case VMArg_t::LIBFUNC_A:
                if(reg) delete reg;
                return reg = new Memcell::LibFunc(libfunc_getused(arg.get_val()));
            default:
                assert(false);
                return reg;
        }
    }
    Memcell::Table::~Table(void) {
        if(!this->table_val) return; 
        this->table_val->dec_ref_counter();
        if(!this->table_val->get_ref_counter()) delete this->table_val;
        this->table_val = nullptr;
    }

    std::string Memcell::Table::to_string(void) const { 
        std::string temp = "[";
        for(auto& elem : this->table_val->num_indexed) {
            temp += "{" + std::to_string(elem.first) + ":" + elem.second->to_string() + "}, "; 
        }
        for(auto& elem : this->table_val->str_indexed) {
            temp += "{" + elem.first + ":" + elem.second->to_string() + "}, "; 
        }
        for(auto& elem : this->table_val->bool_indexed) {
            std::string _key = (elem.first) ? "TRUE" : "FALSE";
            temp += "{" + _key + ":" + elem.second->to_string() + "}, "; 
        }
        for(auto& elem : this->table_val->userfunc_indexed) {
            temp += "{" + std::to_string(elem.first) + ":" + elem.second->to_string() + "}, "; 
        }
        for(auto& elem : this->table_val->libfunc_indexed) {
            temp += "{" + elem.first + ":" + elem.second->to_string() + "}, "; 
        }
        temp.erase(temp.end()-2, temp.end());
        temp += "]";
        return temp; 
    }
    bool Memcell::Table::has_functor(void) const {
        return this->table_val->str_indexed.find("()") != this->table_val->str_indexed.end();
    }
    unsigned Memcell::Table::get_functor(void) const {
        auto& mem = this->table_val->str_indexed.at("()");
        if(mem->get_type() == AVM::Memcell_t::USERFUNC_M) {
            return mem->get_userfunc();
        } else {
            AVM::error("INVALID FUNCTOR");
            return 0;
        }
    }
    void assign(Memcell::Base*& lv, Memcell::Base* rv) {
        if(lv == rv) return;
        assert(lv);
        assert(rv);
        if(lv->get_type() == Memcell_t::TABLE_M &&
            rv->get_type() == Memcell_t::TABLE_M &&
            lv->get_table() == rv->get_table()) return;
        if(rv->get_type() == Memcell_t::UNDEF_M && rv != AVM::Memcell::Base::retval)
            warning("ASSIGNING FROM 'UNDEF' CONTENT");
        if(lv) delete lv;
        lv = rv->clone();
        if(lv->get_type() == Memcell_t::TABLE_M)
            lv->get_table()->inc_ref_counter();
    }
    unsigned get_total_actuals(unsigned topsp) {
        return Execute::get_envvalue(topsp + NUMACTUALS_OFFSET);
    }
    Memcell::Base*& get_actual(unsigned i, unsigned topsp) {
        assert(i < get_total_actuals(topsp));
        auto& stack = Memcell::Base::stack;
        return stack.at(topsp + Memcell::Base::STACK_ENV_SIZE + get_total_actuals(topsp) - i);
    }
}
void _print_stack(void) {
    /*
    std::ofstream _f("stack_log.txt");
    for(auto& t : AVM::Memcell::Base::stack) {
        if(!t) break;
        _f << t->to_string() << std::endl;
    }
    _f << "INFO:" << std::endl;
    _f << "TOP: " << std::to_string(AVM::Memcell::Base::top+1) << std::endl;
    _f << "TOPSP: " << std::to_string(AVM::Memcell::Base::topsp+1) << std::endl;
    */
}
