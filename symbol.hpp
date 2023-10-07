#ifndef SYMBOL_H
#define SYMBOL_H
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include<list>
#include <stack>
#include<memory>
#include<cassert>
extern int yylineno;

void compile_error(const std::string&, bool=false);

namespace std {
    enum Symbol_Type { 
        GLOBAL_VARIABLE, LOCAL_VARIABLE, FORMAL_ARGUMENT, LIBRARY_FUNCTION, USER_FUNCTION, HIDDEN_VARIABLE
    };
    namespace {
        string Library_Functions[] = {"global variable", "local variable", "formal argument", "library function", "user function"};
    }
    string& to_string(Symbol_Type type);
}

inline std::string& std::to_string(std::Symbol_Type type) { return Library_Functions[type]; }

class Symbol {
    public:
    static std::stack<unsigned> scope_offset_stack;
    typedef std::Symbol_Type Type;
    enum class Scope_Space {
        PROGRAM_VAR, FUNCTION_LOCAL, FORMAL_ARG
    };
    enum class Symbol_t { 
        VAR_S, PROGRAMFUNC_S, LIBRARYFUNC_S
    };
    static unsigned curr_scope_offset() {
        return Offset[curr_scope_space()];
    }
    static void reset_scope_offset(const Symbol::Scope_Space space) {
        assert(space != Scope_Space::PROGRAM_VAR);
        Offset[space] = 0;
    }
    static void enter_scope_space() { ++scope_space_counter; }
    static void exit_scope_space() { assert(scope_space_counter > 1); --scope_space_counter; }
    static void restore_curr_scope_offset(const unsigned n) {
        if(curr_scope_space() == Scope_Space::PROGRAM_VAR) return;
        Offset[curr_scope_space()] = n;
    }
    private:
    static std::unordered_map<Scope_Space, unsigned> Offset;
    static unsigned scope_space_counter;
    static Scope_Space curr_scope_space(void) {
        if(scope_space_counter == 1)
            return Scope_Space::PROGRAM_VAR;
        if(!(scope_space_counter % 2))
            return Scope_Space::FORMAL_ARG;
        return Scope_Space::FUNCTION_LOCAL;
    }
    static bool compare_symbols(const Symbol* sym1, const Symbol* sym2){
        if(sym1->scope_n != sym2->scope_n) return (sym1->scope_n < sym2->scope_n);
        return (sym1->line_n < sym2->line_n);
    }
    protected:
    std::string name;
    Type type;
    unsigned line_n;
    unsigned scope_n;
    unsigned function_depth_n;
    Scope_Space space;
    unsigned offset;
    bool isActive;
    unsigned Shadowed_count;
    bool isFunc;
    static std::unordered_set<std::string> library_functions;
    public:
    static std::multimap<std::string, Symbol*> Symbol_Table;
    static unsigned curr_scope;
    static unsigned curr_function_depth;
    static bool curr_is_func;
    static unsigned program_var_offset;
    static unsigned function_local_offset;
    static unsigned formal_argument_offset;
    Symbol(const std::string& name, const Type type, const bool isFunc, unsigned line_n = yylineno) :
        name(name), type(type), function_depth_n(curr_function_depth), isActive(true), Shadowed_count(0), isFunc(isFunc),
        line_n(line_n), scope_n(curr_scope) {
        if(this->type == Type::GLOBAL_VARIABLE || this->type == Type::LOCAL_VARIABLE || this->type == Type::HIDDEN_VARIABLE) {
            this->space = Scope_Space::PROGRAM_VAR;
            // this->offset = program_var_offset++;
            // std::cout << "PV OFFSET: " << Offset[this->space] << std::endl;
            this->offset = Offset[this->space]++;
            
        } else if(this->type == Type::FORMAL_ARGUMENT) {
            this->space = Scope_Space::FORMAL_ARG;
            // this->offset = formal_argument_offset++;
            //std::cout << "FA OFFSET: " << Offset[this->space] << std::endl;
            this->offset = Offset[this->space]++;
        } else {
            this->space = Scope_Space::FUNCTION_LOCAL;
            // this->offset = function_local_offset++;
            //std::cout << "FL OFFSET: " << Offset[this->space] << std::endl;
            this->offset = Offset[this->space]++;
        }
    }
    virtual ~Symbol(){}
    virtual void print_info(std::ostream& os = std::cout) const = 0;
    virtual void set_total_locals(unsigned total_locals) { assert(false); }
    virtual unsigned get_total_locals() const { assert(false); return -1; }
    virtual unsigned get_address(void) const { assert(false); return -1; }
    virtual void set_address(const unsigned address) { assert(false); }
    void deactivate(){ this->isActive = false; }
    void activate(){ this->isActive = true;}
    void deshadow(){assert(this->Shadowed_count > 0); --this->Shadowed_count; }
    void shadow(){ ++this->Shadowed_count; }
    auto get_is_func() const { return this->isFunc; }
    auto get_name() const { return this->name; }
    auto get_scope() const { return this->scope_n; }
    auto get_active_state() const { return this->isActive; }
    auto get_shadowed_state() const { return this->Shadowed_count; }
    auto get_function_depth() const { return this->function_depth_n; }
    auto get_space() const { return this->space; }
    auto get_offset() const { return this->offset; }
    static void deshadow_key(const std::string& key){
        for(auto&& s = Symbol_Table.find(key); s != Symbol_Table.end() && s->first == key; ++s)
            if(s->second->get_shadowed_state()){
                s->second->deshadow();
                if(!s->second->get_shadowed_state()) s->second->activate();
            }
    }
    static void shadow_key(const std::string& key){
        for(auto&& s = Symbol_Table.find(key); s != Symbol_Table.end() && s->first == key; ++s){
            s->second->deactivate();
            s->second->shadow();
        }
    }
    static bool is_library_function(const std::string& key){ return library_functions.find(key) != library_functions.end(); }
    static Symbol* Lookup_global(const std::string& key){
        for(auto&& s = Symbol_Table.find(key); s!= Symbol_Table.end() && s->first == key; ++s){
            auto& curr_symbol = s->second;
            if(!curr_symbol->get_scope()) {
                curr_is_func = curr_symbol->isFunc;
                return curr_symbol;
            }
        }
        return nullptr;
    }
    static Symbol* Lookup(const std::string& key, const unsigned scope) {
        for(auto&& s = Symbol_Table.find(key); s!= Symbol_Table.end() && s->first == key; ++s){
            auto& curr_symbol = s->second;
            if(curr_symbol->get_scope() == scope && curr_symbol->get_active_state()) { 
                curr_is_func = curr_symbol->isFunc;
                return curr_symbol;
            }
        }
        return nullptr;
    }
    static Symbol* Lookup_range(const std::string& key, const int scope_end, int scope_begin){
        for(int scope = scope_begin; scope >= scope_end; --scope)
            for(auto&& s = Symbol_Table.find(key); s!= Symbol_Table.end() && s->first == key; ++s){
                auto& curr_symbol = s->second;
                if(curr_symbol->scope_n == scope && curr_symbol->get_active_state() ||
                    (!curr_symbol->scope_n && !curr_symbol->get_shadowed_state())) {
                    curr_is_func = curr_symbol->isFunc;
                    return curr_symbol;
                }
            }
        return nullptr;
    }
    static void Insert(const std::string& key, Symbol* symbol_to_add){
        //inc_curr_scope_offset();
        Symbol_Table.insert(std::pair<std::string, Symbol*>(key, symbol_to_add));
    }
    static void Hide(const unsigned scope){
        for(auto& s : Symbol_Table)
            if(s.second->scope_n == scope && s.second->get_active_state()) {
                deshadow_key(s.first);
                s.second->deactivate();
            }
    }
    static bool is_accessable(const std::string& key){
        for(auto&& it = Symbol_Table.find(key); it!= Symbol_Table.end() && it->first == key; ++it)
            if(it->second->get_active_state() && (it->second->get_function_depth() == curr_function_depth || !it->second->get_scope()))
                return true;
        return false;
    }
    static void print_scope_n(const unsigned scope_n, std::ostream& os = std::cout){
        os << std::endl << "----------------------     Scope #" << scope_n << "     ----------------------" << std::endl;
    }
    static void print_all_symbols(){
        std::vector<Symbol*> temp_vector;
        for(auto& s : Symbol_Table) temp_vector.push_back(s.second);
        std::sort(temp_vector.begin(), temp_vector.end(), compare_symbols);
        std::cout << "\nSYMBOL TABLE\n######################################################################################################" << std::endl;
        unsigned printing_scope;
        print_scope_n(printing_scope = 0);
        for(auto& v : temp_vector){
            if(printing_scope < v->scope_n){
                printing_scope = v->scope_n;
                print_scope_n(printing_scope);
            }
            delete v;
        }
    }
    operator std::string() const { return this->name; }
};

class Variable final : public Symbol {
    protected:
    void print_info(std::ostream& os = std::cout) const {
        os
            << "\"" << this->name << "\" "
            << "[" << std::to_string(this->type) << "] "
            << "(line " << this->line_n <<") "
            << "(scope " << this->scope_n << ") "
            << "{function depth: " << this->function_depth_n << "}"
            << " |offset: " << this->offset << "|" << std::endl;
    }
    public:
    Variable(const std::string& name, const Type type) : Symbol(name, type, false) {}
};

class Temp_Variable final : public Symbol {
    static unsigned curr_hidden_var_number;
    std::string new_temp_name() { return "_t" + std::to_string(curr_hidden_var_number++); }
    public:
    Temp_Variable() : Symbol(new_temp_name(), Type::HIDDEN_VARIABLE, false) {}
    static void reset_temp() { /*curr_hidden_var_number = 0;*/}
    void print_info(std::ostream& os = std::cout) const override {} 
};

class Function final : public Symbol {
    private:
    unsigned total_locals;
    unsigned address;
    std::list<std::string> arguments;
    public:
    Function() : Symbol("dummy", Type::USER_FUNCTION, true) {}
    Function(const std::string& name, const Type type, unsigned line_n = yylineno) : Symbol(name, type, true, line_n), total_locals(0) {}
    unsigned get_address(void) const override { return this->address; }
    void set_address(const unsigned address) override { this->address = address; }
    bool add_argument(const std::string& arg) { 
        if(this == dummy) return false;
        if(std::find(this->arguments.begin(), this->arguments.end(), arg) == this->arguments.end()){
            this->arguments.push_front(arg);
            return true;
        } else { 
            compile_error(arg + " IS DECLARED AS A FORMAL ARGUMENT MORE THAN ONCE");
            return false;
        }
    }
    void print_info(std::ostream& os = std::cout) const {
        os
            << "\"" << this->name << "\" "
            << "[" << std::to_string(this->type) << "] "
            << "(line " << this->line_n <<") "
            << "(scope " << this->scope_n << ") "
            << "{function depth: " << this->function_depth_n << "} | Arguments: " ;
        for(auto& a : arguments) os << a << " ";
        os << std::endl;            
    }
    void set_total_locals(unsigned total_locals) override { this->total_locals = total_locals; }
    virtual unsigned get_total_locals() const override { return this->total_locals; }
    static Function* dummy;
    static Function* curr_function;
    static unsigned unnamed_function_n;
    static std::string get_next_func_name(){ return "$f" + std::to_string(unnamed_function_n++); }
    static void initialize_library_functions(){
        library_functions = {"print", "input", "objectmemberkeys", "objecttotalmembers", "objectcopy",
                            "totalarguments", "argument", "typeof", "strtonum", "sqrt", "cos", "sin"};
        for(auto& lib_func : library_functions) Insert(lib_func, new Function(lib_func, Type::LIBRARY_FUNCTION, 0));
    }
};
#endif
