#include"machine_code.hpp"

std::vector<std::string> Machine_Code::str_consts;
std::vector<double> Machine_Code::num_consts;
std::vector<Machine_Code::User_Func> Machine_Code::user_funcs;
std::vector<std::string> Machine_Code::lib_funcs;
std::vector<Machine_Code::Instruction> Machine_Code::instructions;
unsigned Machine_Code::total_globals = 0;

std::map<Machine_Code::VMOpCode, std::string> Machine_Code::opcode_to_str = {
        {Machine_Code::VMOpCode::ASSIGN_V, "ASSIGN"}, {Machine_Code::VMOpCode::ADD_V, "ADD"},
        {Machine_Code::VMOpCode::SUB_V, "SUB"}, {Machine_Code::VMOpCode::MUL_V, "MUL"},
        {Machine_Code::VMOpCode::DIV_V, "DIV"}, {Machine_Code::VMOpCode::MOD_V, "MOD"},
        {Machine_Code::VMOpCode::UMINUS_V, "UMINUS"}, {Machine_Code::VMOpCode::JEQ_V, "JEQ"},
        {Machine_Code::VMOpCode::JNE_V, "JNE"}, {Machine_Code::VMOpCode::JLE_V, "JLE"},
        {Machine_Code::VMOpCode::JGE_V, "JGE"}, {Machine_Code::VMOpCode::JLT_V, "JLT"},
        {Machine_Code::VMOpCode::JGT_V, "JGT"}, {Machine_Code::VMOpCode::CALL_V, "CALL"},
        {Machine_Code::VMOpCode::PUSHARG_V, "PUSHARG"}, {Machine_Code::VMOpCode::NEWTABLE_V, "NEWTABLE"},
        {Machine_Code::VMOpCode::JUMP_V, "JUMP"}, {Machine_Code::VMOpCode::TABLEGETELEM_V, "TABLEGETELEM"},
        {Machine_Code::VMOpCode::FUNCENTER_V, "FUNCENTER"}, {Machine_Code::VMOpCode::FUNCEXIT_V, "FUNCEXIT"},
        {Machine_Code::VMOpCode::TABLESETELEM_V, "TABLESETELEM"}, {Machine_Code::VMOpCode::NOP_V, "NOP"}};