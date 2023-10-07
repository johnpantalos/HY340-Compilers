#include"machine_code.hpp"
#include<cassert>
#include<string>
#include<map>
#include<unordered_set>

namespace Execute {
    extern unsigned get_envvalue(unsigned);
}

namespace AVM {
    namespace Memcell {
        class Base;
    }
    constexpr unsigned NUMACTUALS_OFFSET = 4;
    constexpr unsigned SAVEDPC_OFFSET = 3;
    constexpr unsigned SAVEDTOP_OFFSET = 2;
    constexpr unsigned SAVEDTOPSP_OFFSET = 1;
    void warning(const std::string& message);
    void error(const std::string& message);

    enum Memcell_t {
        NUMBER_M = 0,
        STRING_M = 1,
        BOOL_M = 2,
        TABLE_M = 3,
        USERFUNC_M = 4,
        LIBFUNC_M = 5,
        NIL_M = 6,
        UNDEF_M = 7
    };

    class Table;

    namespace Memcell {
        class Base {
            static std::unordered_set<Machine_Code::VMArg_t> variable_types;
            public:
            virtual ~Base(void) = default;
            static std::unordered_map<Memcell_t, std::string> type_strings;
            static constexpr unsigned STACK_SIZE = 4096;
            static constexpr unsigned STACK_ENV_SIZE = 4;
            static Base *ax, *bx, *cx, *retval;
            static unsigned top, topsp;
            static unsigned total_actuals;
            static void set_reg(Base*& reg, Base* val) {
                if(reg) delete reg;
                reg = val->clone();
            }
            static std::vector<Base*> stack;
            virtual Memcell_t get_type(void) const = 0;
            virtual Base* clone(void) const = 0;
            virtual std::string to_string(void) const = 0;
            virtual bool to_bool(void) const = 0;
            virtual double get_num(void) const { assert(false); return 0.0; }
            virtual void set_num(double) { assert(false); }
            virtual std::string get_str(void) const { assert(false); return ""; }
            virtual void set_str(const std::string&) { assert(false); }
            virtual bool get_bool(void) const { assert(false); return false; }
            virtual void set_bool(bool) { assert(false); }
            virtual ::AVM::Table* get_table(void) const { assert(false); return nullptr; }
            virtual void set_table(::AVM::Table*) { assert(false); }
            virtual unsigned get_userfunc(void) const { assert(false); return 0; }
            virtual void set_userfunc(unsigned) { assert(false); }
            virtual std::string get_libfunc(void) const { assert(false); return ""; }
            virtual void set_libfunc(const std::string&) { assert(false); }
            virtual bool has_functor(void) const { assert(false); return false; }
            virtual unsigned get_functor(void) const { assert(false); return 0; }
            friend Base& translate_operand(Machine_Code::VMArg&, Base&);
        };
        class Number final : public Base {
            double num_val;
            public:
            Number(void) = default;
            Number(double num_val) : num_val(num_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::NUMBER_M; }
            std::string to_string(void) const override { return std::to_string(this->num_val); }
            bool to_bool(void) const override { return this->num_val != 0; }
            double get_num(void) const override { return this->num_val; }
            void set_num(double num_val) override { this->num_val = num_val; }
            Base* clone(void) const override { return new Number(*this); }
        };
        class String final : public Base {
            std::string str_val;
            public:
            String(void) = default;
            String(const std::string& str_val) : str_val(str_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::STRING_M; }
            std::string to_string(void) const override { return this->str_val; }
            bool to_bool(void) const override { return this->str_val != ""; }
            std::string get_str(void) const override { return this->str_val; }
            void set_str(const std::string&) { this->str_val = str_val; }
            Base* clone(void) const override { return new String(*this); }
        };
        class Bool final : public Base {
            bool bool_val;
            public:
            Bool(void) = default;
            Bool(bool bool_val) : bool_val(bool_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::BOOL_M; }
            std::string to_string(void) const override { return (this->bool_val) ? "TRUE" : "FALSE"; }
            bool to_bool(void) const override { return this->bool_val; }
            bool get_bool(void) const override { return this->bool_val; }
            void set_bool(bool bool_val) override { this->bool_val = bool_val; }
            Base* clone(void) const override { return new Bool(*this); }
        };
        class Table final : public Base {
            ::AVM::Table* table_val;
            public:
            ~Table(void);
            Table(void) = default;
            Table(::AVM::Table* table_val) : table_val(table_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::TABLE_M; }
            std::string to_string(void) const override;
            bool to_bool(void) const override { return true; }
            ::AVM::Table* get_table(void) const override { return this->table_val; }
            void set_table(::AVM::Table* table_val) override { this->table_val = table_val; }
            Base* clone(void) const override { return new Table(*this); }
            bool has_functor() const override;
            unsigned get_functor(void) const override;
        };
        class UserFunc final : public Base {
            unsigned userfunc_val;
            public:
            UserFunc(void) = default;
            UserFunc(unsigned userfunc_val) : userfunc_val(userfunc_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::USERFUNC_M; }
            std::string to_string(void) const override { return "USER FUNCTION [" + std::to_string(this->userfunc_val) + "]"; }
            bool to_bool(void) const override { return true; }
            unsigned get_userfunc(void) const override { return this->userfunc_val; }
            void set_userfunc(unsigned userfunc_val) override { this->userfunc_val = userfunc_val; }
            Base* clone(void) const override { return new UserFunc(*this); }
            
        };
        class LibFunc final : public Base {
            std::string libfunc_val;
            public:
            LibFunc(void) = default;
            LibFunc(const std::string& libfunc_val) : libfunc_val(libfunc_val) {}
            Memcell_t get_type(void) const override { return Memcell_t::LIBFUNC_M; }
            std::string to_string(void) const override { return "LIBRARY FUNCTION [" + this->libfunc_val + "]"; }
            bool to_bool(void) const override { return true; }
            std::string get_libfunc(void) const override { return this->libfunc_val; }
            void set_libfunc(const std::string& libfunc_val) override { this->libfunc_val = libfunc_val; }
            Base* clone(void) const override { return new LibFunc(*this); }
        };
        class Nil final : public Base {
            public:
            Nil() = default;
            Memcell_t get_type(void) const override { return Memcell_t::NIL_M; }
            std::string to_string(void) const override { return "NIL"; }
            bool to_bool(void) const override { return false; }
            Base* clone(void) const override { return new Nil(); }
        };
        class Undef final : public Base {
            public:
            Undef() = default;
            Memcell_t get_type(void) const override { return Memcell_t::UNDEF_M; }
            std::string to_string(void) const override { return "UNDEF"; }
            bool to_bool(void) const override { assert(false); return false; }
            Base* clone(void) const override { return new Undef(); }
        };
    }

    class Table final {
        public:
        unsigned ref_counter;
        std::map<double, Memcell::Base*> num_indexed;
        std::map<std::string, Memcell::Base*> str_indexed;
        std::map<bool, Memcell::Base*> bool_indexed;
        std::map<unsigned, Memcell::Base*> userfunc_indexed;
        std::map<std::string, Memcell::Base*> libfunc_indexed;
        unsigned total;
        Table(void) : ref_counter(0), total(0) {}
        void inc_ref_counter(void) { ++this->ref_counter; }
        void dec_ref_counter(void) {
            assert(this->ref_counter);
            --this->ref_counter;
        }
        unsigned get_ref_counter(void) { return this->ref_counter; }
        void set_elem(Memcell::Base* key, Memcell::Base* elem) {
            switch(key->get_type()){
                case Memcell_t::NUMBER_M:
                    num_indexed.insert_or_assign(key->get_num(), elem->clone());
                    break;
                case Memcell_t::STRING_M:
                    str_indexed.insert_or_assign(key->get_str(), elem->clone());
                    break;
                case Memcell_t::BOOL_M:
                    bool_indexed.insert_or_assign(key->get_bool(), elem->clone());
                    break;
                case Memcell_t::USERFUNC_M:
                    userfunc_indexed.insert_or_assign(key->get_userfunc(), elem->clone());
                    break;
                case Memcell_t::LIBFUNC_M:
                    libfunc_indexed.insert_or_assign(key->get_libfunc(), elem->clone());
                    break;
                default:
                    AVM::error("UNSUPPORTED KEY TYPE");
            }
        }
        Memcell::Base* get_elem(Memcell::Base* key) {
            switch(key->get_type()){
                case Memcell_t::NUMBER_M:
                    return (num_indexed.find(key->get_num()) != num_indexed.end()) ? 
                    num_indexed.at(key->get_num()) : nullptr;
                case Memcell_t::STRING_M:
                    return (str_indexed.find(key->get_str()) != str_indexed.end()) ? 
                    str_indexed.at(key->get_str()) : nullptr;
                case Memcell_t::BOOL_M:
                    return (bool_indexed.find(key->get_bool()) != bool_indexed.end()) ? 
                    bool_indexed.at(key->get_bool()) : nullptr;
                case Memcell_t::USERFUNC_M:
                    return (userfunc_indexed.find(key->get_userfunc()) != userfunc_indexed.end()) ? 
                    userfunc_indexed.at(key->get_userfunc()) : nullptr;
                case Memcell_t::LIBFUNC_M:
                    return (libfunc_indexed.find(key->get_libfunc()) != libfunc_indexed.end()) ? 
                    libfunc_indexed.at(key->get_libfunc()) : nullptr;
                default:
                    AVM::error("UNSUPPORTED KEY TYPE");
                    return nullptr;
            }
        }
    };
    double consts_getnumber(unsigned);
    std::string consts_getstring(unsigned);
    unsigned userfunc_getused(unsigned);
    std::string libfunc_getused(unsigned);
    Memcell::Base*& translate_operand(Machine_Code::VMArg& arg, Memcell::Base*& reg);
    void assign(Memcell::Base*& lv, Memcell::Base* rv);

    unsigned get_total_actuals(unsigned=Memcell::Base::topsp);
    Memcell::Base*& get_actual(unsigned, unsigned=Memcell::Base::topsp);

}

void _print_stack(void);