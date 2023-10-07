#include "symbol.hpp"
void compile_error(const std::string& message, bool doExit) {
    std::cerr << "\033[1;31mCOMPILE ERROR [LINE " << yylineno <<  "]: " << message << "\033[0m" << std::endl;
    if(doExit) exit(0);
}
std::unordered_map<Symbol::Scope_Space, unsigned> Symbol::Offset = 
{{Symbol::Scope_Space::PROGRAM_VAR, 0},
{Symbol::Scope_Space::FUNCTION_LOCAL, 0},
{Symbol::Scope_Space::FORMAL_ARG, 0}};
std::stack<unsigned> Symbol::scope_offset_stack;
unsigned Symbol::scope_space_counter = 1;
unsigned Symbol::curr_scope = 0;
unsigned Symbol::curr_function_depth = 0;
unsigned Symbol::program_var_offset = 0;
unsigned Symbol::function_local_offset = 0;
unsigned Symbol::formal_argument_offset = 0;
bool Symbol::curr_is_func;
unsigned Function::unnamed_function_n = 0;
Function* Function::curr_function = 0;
Function* Function::dummy = new Function();
std::multimap<std::string, Symbol*> Symbol::Symbol_Table;
std::unordered_set<std::string> Symbol::library_functions;
unsigned Temp_Variable::curr_hidden_var_number = 0;