#ifndef TYPES_H
#define TYPES_H
#include "symbol.hpp"
#include <set>
#include <sstream>
#define declare_E_op_E(op) double operator op(const Base&, const Base&);
#define define_E_op_E(op)\
double operator op(const Base& expr1, const Base& expr2) {\
    return expr1.get_num_const() op expr2.get_num_const();\
}

namespace Expr{
    enum Expr_t {
        VAR_E, TABLEITEM_E, PROGRAMFUNC_E, LIBRARYFUNC_E, ARITHEXPR_E, BOOLEXPR_E,
        ASSIGNEXPR_E, NEWTABLE_E, CONSTNUM_E, CONSTBOOL_E, CONSTSTRING_E, NIL_E
    };

    class Base {
        public:
        static std::unordered_set<Base*> all_exprs;
        Base() {all_exprs.insert(this);}
        virtual ~Base() = default;
        virtual Symbol* get_sym() const { assert(false && "get_sym() WAS NOT OVERRIDDEN"); return nullptr; }
        virtual void set_sym(Symbol* sym) { assert(false && "set_sym() WAS NOT OVERRIDDEN"); }
        virtual void set_index(Base* index) { assert(false && "set_index() WAS NOT OVERRIDDEN"); }
        virtual Expr_t get_type() const = 0;
        virtual double get_num_const() const { assert(false && "get_num_const() WAS NOT OVERRIDDEN"); return 0; }
        virtual std::string get_str_const() const { assert(false && "get_str_const() WAS NOT OVERRIDDEN"); return ""; }
        virtual bool get_bool_const() const { assert(false && "get_bool_const() WAS NOT OVERRIDDEN"); return false; }
        virtual Base* get_index() const { assert(false && "get_index() WAS NOT OVERRIDDEN"); return nullptr; }
        virtual bool is_arith() const {compile_error("NOT ARITHMETIC"); return false; }
        virtual unsigned get_truelist() const {return 0;}
        virtual unsigned get_falselist() const {return 0;}
        virtual void set_truelist(unsigned) { assert(false && "set_truelist() WAS NOT OVERRIDDEN");}
        virtual void set_falselist(unsigned) { assert(false && "set_falselist() WAS NOT OVERRIDDEN");}
        virtual bool is_temp_expr() const { return false; }
        virtual operator std::string() const = 0;
    };

    class Symbol_Container : public Base {
        Symbol* sym;
        public:
        Symbol_Container(Symbol* sym) : sym(sym) {}
        Symbol* get_sym() const override { return this->sym; }
        void set_sym(Symbol* sym) override { this->sym = sym; }
        bool is_temp_expr() const override { return this->sym->get_name()[0] == '_';}
        operator std::string() const override { assert(this->get_sym()); return this->sym->get_name(); }
    };


    class LValue : public Symbol_Container {
        protected:
        LValue(Symbol* sym) : Symbol_Container(sym) {}
    };

    class Var final : public LValue {
        public:
        Var() : LValue(new Temp_Variable()) {}
        Var(Symbol* sym) : LValue(sym) {}
        bool is_arith() const override { return true; }
        Expr_t get_type() const override { return Expr_t::VAR_E; }
        static Var* _result;
    };

    class Program_Func final : public LValue {
        public:
        Program_Func() : LValue(new Temp_Variable()) {}
        Program_Func(Symbol* sym) : LValue(sym) {}
        Expr_t get_type() const override { return Expr_t::PROGRAMFUNC_E; }
    };

    class Library_Func final : public LValue {
        public:
        Library_Func() : LValue(new Temp_Variable()) {}
        Library_Func(Symbol* sym) : LValue(sym) {}
        Expr_t get_type() const override { return Expr_t::LIBRARYFUNC_E; }
    };

    class Table_Item final : public LValue {
        Base* index;
        public:
        Table_Item(Symbol* sym, Base* index) : LValue(sym), index(index) {}
        void set_index(Base* index) override { this->index = index; }
        Base* get_index() const override { return this->index; }
        Expr_t get_type() const override { return Expr_t::TABLEITEM_E; }
        bool is_arith() const override { return true; }
    };

    class Const : public Base {};
    
    class Const_Num final : public Const {
        const double num_const;
        public:
        Const_Num(const double num_const) : num_const(num_const) {}
        double get_num_const() const override { return this->num_const; }
        Expr_t get_type() const override { return Expr_t::CONSTNUM_E; }
        bool is_arith() const override { return true; }
        operator std::string() const override { return std::to_string(this->num_const); }
    };

    class Const_Bool final : public Const {
        const bool bool_const;
        public:
        Const_Bool(const bool bool_const) : bool_const(bool_const) {}
        bool get_bool_const() const override { return this->bool_const; }
        Expr_t get_type() const override { return Expr_t::CONSTBOOL_E; }
        operator std::string() const override { return (this->get_bool_const()) ? "true" : "false"; }
    };

    class Const_String final : public Const {
        const std::string str_const;
        public:
        Const_String(const std::string& str_const) : str_const(str_const) {}
        std::string get_str_const() const override { return this->str_const; }
        Expr_t get_type() const override { return Expr_t::CONSTSTRING_E; }
        operator std::string() const override { return this->get_str_const(); }
    };

    class Nil : public Const {
        Expr_t get_type() const override { return Expr_t::NIL_E; }
        operator std::string() const override { return "nil"; }
    };

    class New_Table final : public Symbol_Container {
        public:
        New_Table() : Symbol_Container(new Temp_Variable()) {}
        Expr_t get_type() const override { return Expr_t::NEWTABLE_E; }
    };

    class Arith_Expr final : public Symbol_Container {
        public:
        Arith_Expr() : Symbol_Container(new Temp_Variable()) {}
        Arith_Expr(Symbol* sym) : Symbol_Container(sym) {}
        bool is_arith() const override { return true; }
        Expr_t get_type() const override { return Expr_t::ARITHEXPR_E; }
    };

    class Bool_Expr final : public Symbol_Container {
        unsigned truelist;
        unsigned falselist;
        public:
        Bool_Expr() : Symbol_Container(new Temp_Variable()), truelist(0), falselist(0) {}
        Bool_Expr(Symbol* sym) : Symbol_Container(sym), truelist(0), falselist(0) {}
        Expr_t get_type() const override { return Expr_t::BOOLEXPR_E; }
        unsigned get_truelist() const override { return this->truelist; }
        unsigned get_falselist() const override { return this->falselist; }
        void set_truelist(unsigned truelist) override { this->truelist = truelist; }
        void set_falselist(unsigned falselist) override { this->falselist = falselist; }
    };

    class Assign_Expr final : public Symbol_Container {
        public:
        Assign_Expr() : Symbol_Container(new Temp_Variable()) {}
        bool is_arith() const override { return true; }
        Expr_t get_type() const override { return Expr_t::ASSIGNEXPR_E; }
    };


    LValue* to_lvalue(Symbol*);

    declare_E_op_E(+);
    declare_E_op_E(-);
    declare_E_op_E(*);
    declare_E_op_E(/);
    declare_E_op_E(%);
    declare_E_op_E(>);
    declare_E_op_E(<);
    declare_E_op_E(==);
    declare_E_op_E(!=);
    declare_E_op_E(>=);
    declare_E_op_E(<=);
    
}

typedef std::list<Expr::Base*> EList;
typedef std::pair<Expr::Base*, Expr::Base*> IndexedElem;
typedef std::list<IndexedElem*> IndexedList;

class Call {
    std::string name;
    bool is_method;
    EList* elist;
    public:
    static std::unordered_set<Call*> all_calls;
    Call() { all_calls.insert(this); }
    Call(const std::string& name, const bool is_method, EList* elist) :
        name(name), is_method(is_method), elist(elist) {}
    std::string get_name() const { return this->name; }
    bool get_is_method() const { return this->is_method; }
    EList* get_elist() const { return this->elist; }
    void set_name(const std::string& name) { this->name = name; }
};

class Stmt {
    unsigned contlist;
    unsigned breaklist;
    public:
    static unsigned loop_counter;
    static std::stack<decltype(loop_counter)> loop_counter_stack;
    static std::stack<unsigned> func_start_jump_stack;
    static std::list<unsigned> return_jump_list;
    static std::stack<std::list<unsigned>> return_jump_stack;
    Stmt(unsigned contlist, unsigned breaklist) : contlist(contlist), breaklist(breaklist) {}
    Stmt() = default;
    unsigned get_contlist() const { return this->contlist; }
    unsigned get_breaklist() const { return this->breaklist; }
    void set_contlist(unsigned contlist) { this->contlist = contlist; }
    void set_breaklist(unsigned breaklist) { this->breaklist = breaklist; }
};

class For_Prefix {
    unsigned test;
    unsigned enter;
    public:
    void set_test(unsigned test) { this->test = test; }
    void set_enter(unsigned enter) { this->enter = enter; } 
    unsigned get_test() const { return this->test; }
    unsigned get_enter() const { return this->enter; }
};
#endif