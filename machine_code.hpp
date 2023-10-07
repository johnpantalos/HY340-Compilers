#include"tcode_types.hpp"
#include<iostream>
#include<fstream>
#include<vector>
#include<bitset>
#include<cstring>

class Machine_Code {
    public:
    typedef TCODE::Instruction Instruction;
    typedef TCODE::VMOpCode VMOpCode;
    typedef TCODE::VMArg_t VMArg_t;
    typedef TCODE::VMArg VMArg;
    typedef TCODE::User_Func User_Func;
    static std::map<VMOpCode, std::string> opcode_to_str;
    private:
    static std::string get_slice(std::string& str, unsigned bits) {
        std::string slice = str.substr(0, bits);
        str.erase(0, bits);
        return slice;
    }
    static inline void _parse_magic_number(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long magic_number = std::stoul(buffer, 0, 2);
        if(magic_number != 340200501) {
            std::cerr << "INVALID .abc FILE" << std::endl;
            exit(0);
        }
    }
    static inline void _parse_str_consts(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long size = std::stoul(get_slice(buffer, 32), 0, 2);
        while(buffer != "") {
            unsigned long pos = std::stoul(get_slice(buffer, 32), 0, 2);
            unsigned long str_size = std::stoul(get_slice(buffer, 32), 0, 2);
            std::string curr_string = "";
            for(unsigned char_n = 0; char_n < str_size; ++char_n) {
                unsigned long ascii_code = std::stoul(get_slice(buffer, 8), 0, 2);
                curr_string += static_cast<char>(ascii_code);
            }
            str_consts.push_back(curr_string);
        }
    }
    template<typename InputIterator>
    static double bitstring_to_double(InputIterator begin, InputIterator end)
    {
        unsigned long long x = 0;
        for (; begin != end; ++begin)
        {
            x = (x << 1) + (*begin - '0');
        }
        double d;
        memcpy(&d, &x, 8);
        return d;
    }
    static inline void _parse_num_consts(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long size = std::stoul(get_slice(buffer, 32), 0, 2);
        while(buffer != "") {
            unsigned long pos = std::stoul(get_slice(buffer, 32), 0, 2);
            std::string str = get_slice(buffer, 64);
            static_assert(sizeof(double) == sizeof(unsigned long));
            auto bit_num = std::bitset<64>(str).to_ullong();
            double num = *reinterpret_cast<double*>(&bit_num);
            num_consts.push_back(num);
        }
    }
    static inline void _parse_user_funcs(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long size = std::stoul(get_slice(buffer, 32), 0, 2);
        while(buffer != "") {
            unsigned long pos = std::stoul(get_slice(buffer, 32), 0, 2);
            User_Func func;
            func.set_taddress(
                std::stoul(get_slice(buffer, 32), 0, 2)
            );
            func.set_local_size(
                std::stoul(get_slice(buffer, 32), 0, 2)
            );
            std::string str = "";
            const unsigned size = std::stoul(get_slice(buffer, 32), 0, 2);
            for(unsigned char_n = 0; char_n < size; ++char_n) {
                unsigned long ascii_code = std::stoul(get_slice(buffer, 8), 0, 2);
                str += static_cast<char>(ascii_code);
            }
            func.set_name(str);
            user_funcs.push_back(func);
        }
    }
    static inline void _parse_lib_funcs(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long size = std::stoul(get_slice(buffer, 32), 0, 2);
        while(buffer != "") {
            unsigned long pos = std::stoul(get_slice(buffer, 32), 0, 2);
            unsigned long str_size = std::stoul(get_slice(buffer, 32), 0, 2);
            std::string curr_string = "";
            for(unsigned char_n = 0; char_n < str_size; ++char_n) {
                unsigned long ascii_code = std::stoul(get_slice(buffer, 8), 0, 2);
                curr_string += static_cast<char>(ascii_code);
            }
            lib_funcs.push_back(curr_string);
        }
    }
    static inline void _parse_instructions(std::ifstream& abc_file) {
        std::string buffer;
        std::getline(abc_file, buffer);
        unsigned long size = std::stoul(get_slice(buffer, 32), 0, 2);
        while(buffer != "") {
            Instruction inst;
            inst.set_opcode(
                static_cast<VMOpCode>(std::stoul(get_slice(buffer, 5), 0, 2))
            );
            inst.set_result(
                new VMArg(
                    static_cast<VMArg_t>(std::stoul(get_slice(buffer, 4), 0, 2)), 
                    static_cast<unsigned>(std::stoul(get_slice(buffer, 28), 0, 2)))
            );
            if(inst.get_result()->get_type() == VMArg_t::GLOBAL_A && inst.get_result()->get_val() > total_globals)
                total_globals = inst.get_result()->get_val();
            inst.set_arg1(
                new VMArg(
                    static_cast<VMArg_t>(std::stoul(get_slice(buffer, 4), 0, 2)), 
                    static_cast<unsigned>(std::stoul(get_slice(buffer, 28), 0, 2)))
            );
            if(inst.get_arg1()->get_type() == VMArg_t::GLOBAL_A && inst.get_arg1()->get_val() + 1 > total_globals)
                total_globals = inst.get_arg1()->get_val()+1;
            inst.set_arg2(
                new VMArg(
                    static_cast<VMArg_t>(std::stoul(get_slice(buffer, 4), 0, 2)), 
                    static_cast<unsigned>(std::stoul(get_slice(buffer, 28), 0, 2)))
            );
            if(inst.get_arg2()->get_type() == VMArg_t::GLOBAL_A && inst.get_arg2()->get_val() + 1 > total_globals)
                total_globals = inst.get_arg2()->get_val()+1;
            instructions.push_back(inst);
        }
    }
    public:
    static std::vector<std::string> str_consts;
    static std::vector<double> num_consts;
    static std::vector<User_Func> user_funcs;
    static std::vector<std::string> lib_funcs;
    static std::vector<Instruction> instructions;
    static unsigned total_globals;
    static void Load(const std::string abc_filename) {
        std::ifstream abc_file;
        abc_file.open(abc_filename, std::ios::in | std::ios::binary);
        _parse_magic_number(abc_file);
        _parse_str_consts(abc_file);
        _parse_num_consts(abc_file);
        _parse_user_funcs(abc_file);
        _parse_lib_funcs(abc_file);
        _parse_instructions(abc_file);
    }
    static void print_info(std::ostream& os = std::cout) {
        os << "------------------------------ MACHINE CODE ------------------------------" << std::endl;
        os << std::endl << "STRING TABLE: " << std::endl;
        for(auto& t : str_consts) {
            os << t << std::endl;
        }
        os << std::endl << "NUMBER TABLE: " << std::endl;
        for(auto& t : num_consts) {
            os << t << std::endl;
        }
        os << std::endl << "USER FUNCTION TABLE: " << std::endl;
        for(auto& t : user_funcs) {
            os  << "NAME: " << t.get_name() << "\t"
                << "TADDRESS: " << t.get_taddress() << "\t"
                << "TOTAL LOCALS: " << t.get_local_size() << std::endl;
        }
        os << std::endl << "LIBRARY FUNCTION TABLE: " << std::endl;
        for(auto& t : lib_funcs) {
            os << t << std::endl;
        }
        os << "TOTAL GLOBALS: " << total_globals << std::endl;
        os << std::endl << "INSTRUCTIONS: " << std::endl;
        unsigned i = 0;
        for(auto& t : instructions) {
            os  << i++ << " "
                << "OPCODE " << opcode_to_str.at(t.get_opcode()) << "\t"
                << "ARG1 T:" << t.get_arg1()->get_type() << " V:" << t.get_arg1()->get_val() << "\t" 
                << "ARG2 T:" << t.get_arg2()->get_type() << " V:" << t.get_arg2()->get_val() << "\t" 
                << "RESULT T:" << t.get_result()->get_type() << " V:" << t.get_result()->get_val() << std::endl; 
        }
        os << std::endl;
    }
    Machine_Code(Machine_Code const&) = delete;
    Machine_Code operator=(Machine_Code const&) = delete;
};
