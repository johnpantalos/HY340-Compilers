#ifndef QUAD_H
#define QUAD_H
#include <string>
#include <iostream>
#include <vector>
#include "types.hpp"
#include <iomanip>

namespace Quad {
    enum IOpCode{
        ASSIGN, ADD, SUB, MUL, DIV, MOD, UMINUS, AND, OR, NOT, IF_EQ, IF_NOTEQ,
        IF_LESSEQ, IF_GREATEREQ, IF_LESS, IF_GREATER, CALL, PARAM, RETURN, GETRETVAL,
        FUNCSTART, FUNCEND, TABLECREATE, TABLEGETELEM, TABLESETELEM, JUMP, NOP
    };

    class Base {
        const static std::unordered_map<IOpCode, std::string> quad_strings;
        const IOpCode opcode;
        mutable unsigned taddress;
        protected:
        static constexpr unsigned PRINT_WIDTH = 20;
        Base(IOpCode opcode) : opcode(opcode) { ++curr_quad_n; }
        public:
        std::string get_opcode_string() const { return quad_strings.find(opcode)->second; }
        virtual ~Base() = default;
        IOpCode get_opcode(void) const { return this->opcode; }
        unsigned get_taddress(void) const { return this->taddress; }
        void set_taddress(unsigned taddress) const { 
            this->taddress = taddress;
        }
        virtual Expr::Base* get_arg1(void) const { return nullptr; }
        virtual Expr::Base* get_arg2(void) const { return nullptr; }
        virtual Expr::Base* get_result(void) const { return nullptr; }
        virtual void set_label(unsigned label) const { assert(false); }
        virtual unsigned get_label() const { return 0;}
        static std::vector<const Base*> quad_list;
        static unsigned curr_quad_n;
        virtual void print_self(unsigned line, std::ostream& os = std::cout) const = 0;
        static void print_all(){
            std::cout   << "\n---------------------------------------------QUAD LIST---------------------------------------------\n"
                        << "QUAD#" << std::setw(PRINT_WIDTH)
                        << "OPCODE" << std::setw(PRINT_WIDTH)
                        << "RESULT" << std::setw(PRINT_WIDTH)
                        << "ARG1" << std::setw(PRINT_WIDTH)
                        << "ARG2" << std::setw(PRINT_WIDTH)
                        << "LABEL" << std::endl;
            for(unsigned line=0; line < quad_list.size(); ++line) {
                quad_list[line]->print_self(line+1);
            }
        }
        friend void emit(IOpCode, Expr::Base*, Expr::Base*, Expr::Base*);
        friend void emit(IOpCode, unsigned, Expr::Base*, Expr::Base*);
        friend unsigned newlist(const unsigned);
        friend unsigned mergelist(const unsigned, const unsigned);
        friend void patchlist(const unsigned, const unsigned);
    };
    class R : public virtual Base {
        protected:
        Expr::Base* result;
        public:
        R(IOpCode opcode, Expr::Base* result) : Base(opcode), result(result) {}
        virtual Expr::Base* get_result(void) const override { return this->result; }
        virtual void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->result) << std::endl;
        }
    };
    class A1 : public virtual Base {
        protected:
        Expr::Base* arg1;
        public:
        A1(IOpCode opcode, Expr::Base* arg1) : Base(opcode), arg1(arg1) {}
        Expr::Base* get_arg1(void) const override { return this->arg1; }
        virtual void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->arg1) << std::endl;
        }

    };
    class A1_R final : public A1, public R {
        public:
        A1_R(IOpCode opcode, Expr::Base* arg1, Expr::Base* result) :
                Base(opcode), A1(opcode, arg1), R(opcode, result) {}
        Expr::Base* get_arg2(void) const override { return nullptr; }
        void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->result) << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->arg1) << std::endl;
        }
    };
    class A1_A2 : public A1 {
        protected:
        Expr::Base* arg2;
        public:
        A1_A2(IOpCode opcode, Expr::Base* arg1, Expr::Base* arg2) : Base(opcode), A1(opcode, arg1), arg2(arg2) {}
        Expr::Base* get_arg2(void) const override { return this->arg2; }
        virtual void print_self(unsigned line, std::ostream& os = std::cout) const override = 0;
    };
    class A1_A2_R final : public A1_A2, public R {
        public:
        A1_A2_R(IOpCode opcode, Expr::Base* arg1, Expr::Base* arg2, Expr::Base* result) :
                Base(opcode), A1_A2(opcode, arg1, arg2), R(opcode, result) {}
        void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->result) << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->arg1) << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->arg2) << std::endl;
        }
    };
    class L : public virtual Base {
        protected:
        mutable unsigned label;
        public:
        L(IOpCode opcode, unsigned label) : Base(opcode), label(label) {}
        void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(4*PRINT_WIDTH)
                << this->label << std::endl;
        }
        void set_label(unsigned label) const override { this->label = label; }
        unsigned get_label() const override { return this->label;}
    };

    class A1_A2_L final : public A1_A2, public L {
        public:
        A1_A2_L(IOpCode opcode, Expr::Base* arg1, Expr::Base* arg2, unsigned label) :
                Base(opcode), A1_A2(opcode, arg1, arg2), L(opcode, label) {}
        void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": " << std::setw(PRINT_WIDTH)
                << this->get_opcode_string() << std::setw(2*PRINT_WIDTH)
                << static_cast<std::string>(*this->arg1) << std::setw(PRINT_WIDTH)
                << static_cast<std::string>(*this->arg2) << std::setw(PRINT_WIDTH)
                << this->label << std::endl;
        }

        void set_label(unsigned label) const override { this->label = label; }
        unsigned get_label() const override { return this->label;}
    };

    class Nop final : public Base {
        public:
        Nop() : Base(IOpCode::NOP) {}
        void print_self(unsigned line, std::ostream& os = std::cout) const override {
            os
                << line << ": "
                << std::setw(PRINT_WIDTH)
                << "NOP" << std::endl;
        }
    };

    void emit(IOpCode opcode, Expr::Base* result, Expr::Base* arg1, Expr::Base* arg2);
    void emit(IOpCode opcode, unsigned label, Expr::Base* arg1, Expr::Base* arg2);
    Expr::Base* emit_iftableitem(Expr::Base*);
    Expr::Table_Item* member_item(Expr::LValue*, const char*);
    void patchlabel(unsigned quad_no, unsigned label = Base::curr_quad_n);
    void make_stmt(Stmt&);
    Expr::Base* make_call(Expr::LValue*, EList*);
    unsigned newlist(int);
    unsigned mergelist(const unsigned, const unsigned);
    void patchlist(const unsigned, const unsigned);
    void backpatch(unsigned, unsigned);
    unsigned merge(unsigned, unsigned);
    void backpatch_ifneeded(Expr::Base*, unsigned = Quad::Base::curr_quad_n, unsigned = Quad::Base::curr_quad_n);
    void emit_assignquads(Expr::Base*);
}
#endif