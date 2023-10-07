#include "quad.hpp"

namespace Quad {
    unsigned Base::curr_quad_n = 1;
    std::vector<const Base*> Base::quad_list;
    const std::unordered_map<IOpCode, std::string> Base::quad_strings = {
         {ASSIGN, "ASSIGN"}, {ADD, "ADD"}, {SUB, "SUB"}, {MUL, "MUL"}, {DIV, "DIV"}, {MOD, "MOD"},
         {UMINUS, "UMINUS"}, {AND, "AND"}, {OR, "OR"}, {NOT, "NOT"}, {IF_EQ, "IF_EQ"}, {IF_NOTEQ, "IF_NOTEQ"},
         {IF_LESSEQ, "IF_LESSEQ"}, {IF_GREATEREQ, "IF_GREATEREQ"}, {IF_LESS, "IF_LESS"}, {IF_GREATER, "IFGREATER"}, 
         {CALL, "CALL"}, {PARAM, "PARAM"}, {RETURN, "RETURN"}, {GETRETVAL, "GETRETVAL"}, {FUNCSTART, "FUNCSTART"}, {FUNCEND, "FUNCEND"},
         {TABLECREATE, "TABLECREATE"}, {TABLEGETELEM, "TABLEGETELEM"}, {TABLESETELEM, "TABLESETELEM"}, {JUMP, "JUMP"}
     };
    void emit(IOpCode opcode, Expr::Base* result, Expr::Base* arg1, Expr::Base* arg2) {
        Quad::Base* q = nullptr;
        if(result) {
            if(arg1) {
                if(arg2) q = new A1_A2_R(opcode, arg1, arg2, result);
                else q = new A1_R(opcode, arg1, result);
            } else {
                q = new R(opcode, result);
            }
        } else {
            q = new A1(opcode, arg1);
        }
        assert(q);
        Base::quad_list.push_back(q);
    }
    void emit(IOpCode opcode, unsigned label, Expr::Base* arg1, Expr::Base* arg2) {
        Quad::Base* q;
        if(arg1) q = new A1_A2_L(opcode, arg1, arg2, label);
        else q = new L(opcode, label);
        Base::quad_list.push_back(q);
    }

    Expr::Base* emit_iftableitem(Expr::Base* expr) {
        if(expr->get_type() != Expr::Expr_t::TABLEITEM_E) {
            return expr;
        } else {
            Expr::Var* result = new Expr::Var();
            emit(Quad::IOpCode::TABLEGETELEM, result, expr, expr->get_index());
            return result;
        }
    }

    Expr::Table_Item* member_item(Expr::LValue* lval, const char* name){
        assert(name);
        lval = dynamic_cast<Expr::LValue*>(emit_iftableitem(lval));
        assert(lval);
        return new Expr::Table_Item(lval->get_sym(), new Expr::Const_String(name));
    }

    void patchlabel(unsigned quad_no, unsigned label) {
        Quad::Base::quad_list[quad_no-1]->set_label(label);
    }

    void make_stmt(Stmt& stmt) {
        new (&stmt) Stmt(0, 0);
    }

    Expr::Base* make_call(Expr::LValue* lval, EList* elist){
        Expr::Base* func = emit_iftableitem(lval);
        for(auto&& e = elist->begin(); e != elist->end(); ++e){
            emit(IOpCode::PARAM, nullptr, *e, nullptr);
        }
        emit(IOpCode::CALL, nullptr, func, nullptr);
        Expr::Var* result = new Expr::Var();
        emit(IOpCode::GETRETVAL, result, nullptr, nullptr);
        return result;
    }
    unsigned newlist(int i) {
        return i-2;
    }
    unsigned mergelist(const unsigned l1, const unsigned l2) {
        if(!l1) return l2;
        else if(!l2) return l1;
        else {
            const auto& quad_list = Base::quad_list;
            unsigned i = l1;
            assert(i < quad_list.size());
            while(quad_list[i]->get_label()) 
                i = quad_list[i]->get_label();
            quad_list[i]->set_label(l2);
            return l1;
        }

    }
    void patchlist(unsigned list, const unsigned label) {
        const auto& quad_list = Base::quad_list;
        while(list) {
            unsigned next = quad_list[list]->get_label();
            quad_list[list]->set_label(label);
            list = next;
        }
    }
    unsigned merge(unsigned l1, unsigned l2){
        if(!l1) return l2;
        else if(!l2) return l1;
        else {
            const auto& quad_list = Base::quad_list;
            unsigned i = l1;
            assert(i-1 < quad_list.size());
            while(quad_list[i-1]->get_label())
                i = quad_list[i-1]->get_label();
            quad_list[i-1]->set_label(l2);
            return l1;
        }
    }
    void backpatch(unsigned list, unsigned label) {
        const auto& quad_list = Base::quad_list;
        while(list) {
            assert(list-1 < quad_list.size());
            unsigned next = quad_list[list-1]->get_label();
            quad_list[list-1]->set_label(label);
            list = next;
        }
    }
    void backpatch_ifneeded(Expr::Base* expr, unsigned true_quad, unsigned false_quad) {
        Expr::Bool_Expr* bool_expr = dynamic_cast<Expr::Bool_Expr*>(expr);
        if(bool_expr) {
            assert(bool_expr->get_type() == Expr::Expr_t::BOOLEXPR_E);
            if(bool_expr->get_falselist()) Quad::backpatch(bool_expr->get_falselist(), false_quad);
            if(bool_expr->get_truelist()) Quad::backpatch(bool_expr->get_truelist(), true_quad);
        }
    }
    void emit_assignquads(Expr::Base* expr) {
        if(expr->get_type() == Expr::Expr_t::BOOLEXPR_E) {
            Quad::backpatch_ifneeded(expr, Quad::Base::curr_quad_n, Quad::Base::curr_quad_n+2);
            Quad::emit(Quad::IOpCode::ASSIGN, expr, new Expr::Const_Bool(true), nullptr);
            Quad::emit(Quad::IOpCode::JUMP, Quad::Base::curr_quad_n+2, nullptr, nullptr);
            Quad::emit(Quad::IOpCode::ASSIGN, expr, new Expr::Const_Bool(false), nullptr);
        }
    }
}