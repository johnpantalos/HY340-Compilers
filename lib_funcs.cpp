#include"lib_funcs.hpp"
#include<regex>

namespace Library {
    const std::unordered_map<std::string, _LibFunc> _libfuncs = {
        {"print", _print}, {"typeof", _typeof}, {"input", _input},
        {"totalarguments", _totalarguments}, {"argument", _argument},
        {"strtonum", _strtonum}, {"sin", _sin}, {"cos", _cos},
        {"sqrt", _sqrt}, {"objectmemberkeys", _objectmemberkeys},
        {"objecttotalmembers", _objecttotalmembers}, {"objectcopy", _objectcopy}
    };
    void _print(void) {
        unsigned n = AVM::get_total_actuals();
        for(unsigned i = 0; i < n; ++i) {
            std::cout << AVM::get_actual(i)->to_string();
        }
        AVM::Memcell::Base::retval = new AVM::Memcell::Undef();
        Execute::restore_stack();
    }

    void _typeof(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("typeof EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            AVM::Memcell::Base::retval = new AVM::Memcell::String(
                AVM::Memcell::Base::type_strings.at(AVM::get_actual(0)->get_type())
            );
        }
        Execute::restore_stack();
        
    }

    void _input(void) {
        unsigned n = AVM::get_total_actuals();
        for(unsigned i = 0; i < n; ++i) {
            std::cout << AVM::get_actual(i)->to_string();
        }
        std::string temp;
        std::getline(std::cin, temp);
        auto& retval = AVM::Memcell::Base::retval;
        if(temp[0] == '"' && temp[temp.size()-1] == '"') {
            temp.erase(0, 1);
            temp.erase(temp.size()-1);
            retval = new AVM::Memcell::String(temp);
        } else if(std::regex_match(temp, std::regex("-?[0-9]+\\.?[0-9]*"))) {
            retval = new AVM::Memcell::Number(std::stod(temp));
        } else if(temp == "true") {
            retval = new AVM::Memcell::Bool(true);
        } else if(temp == "false") {
            retval = new AVM::Memcell::Bool(false);
        } else if(temp == "nil") {
            retval = new AVM::Memcell::Nil();
        } else {
            retval = new AVM::Memcell::String(temp);
        }
        Execute::restore_stack();
    }

    void _objectmemberkeys(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("objectmemberkeys EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::TABLE_M) {
            AVM::error("objectmemberkeys EXPECTS A TABLE");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            unsigned i = 0;
            auto table = AVM::get_actual(0)->get_table();
            auto table_keys = new AVM::Table();
            for(auto& elem : table->num_indexed) {
                table_keys->num_indexed.insert({i++, new AVM::Memcell::Number(elem.first)});
            }
            for(auto& elem : table->str_indexed) {
                table_keys->num_indexed.insert({i++, new AVM::Memcell::String(elem.first)});
            }
            AVM::Memcell::Base::retval = new AVM::Memcell::Table(table_keys);
        }
        Execute::restore_stack();
    }
    void _objecttotalmembers(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("objecttotalmembers EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::TABLE_M) {
            AVM::error("objecttotalmembers EXPECTS A TABLE");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            unsigned totalmembers = 0;
            auto table = AVM::get_actual(0)->get_table();
            totalmembers += table->num_indexed.size();
            totalmembers += table->str_indexed.size();
            AVM::Memcell::Base::retval = new AVM::Memcell::Number(totalmembers);
        }
        Execute::restore_stack();
    }

    void _objectcopy(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("objecttotalmembers EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::TABLE_M) {
            AVM::error("objecttotalmembers EXPECTS A TABLE");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            AVM::Memcell::Base::retval = new AVM::Memcell::Table(
                new AVM::Table(*AVM::get_actual(0)->get_table())
            );
        }
        Execute::restore_stack();
    }

    void _totalarguments(void) {
        auto& topsp = AVM::Memcell::Base::topsp;
        unsigned p_topsp = Execute::get_envvalue(topsp + AVM::SAVEDTOPSP_OFFSET);
        if(!p_topsp) {
            AVM::error("totalarguments CALLED OUTSIDE FUNCTION");
        } else {
            auto& retval = AVM::Memcell::Base::retval;
            retval = new AVM::Memcell::Number(Execute::get_envvalue(p_topsp + AVM::NUMACTUALS_OFFSET));
        }
        Execute::restore_stack();
    }
    void _argument(void) {
        auto& topsp = AVM::Memcell::Base::topsp;
        unsigned p_topsp = Execute::get_envvalue(topsp + AVM::SAVEDTOPSP_OFFSET);
        unsigned n = AVM::get_total_actuals();
        if(!p_topsp) {
            AVM::error("argument CALLED OUTSIDE FUNCTION");
        } else if(n != 1){
            AVM::error("argument EXPECTS EXACTLY 1 ARGUMENT");
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::NUMBER_M) {
            AVM::error("argument EXPECTS A NUMBER");
            exit(0);
        } else {
            AVM::Memcell::Base::retval = AVM::get_actual(AVM::get_actual(0)->get_num(), p_topsp);
        }
        Execute::restore_stack();
    }

    void _strtonum(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("strtonum EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::STRING_M) {
            AVM::error("strtonum EXPECTS A STRING");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            if(std::regex_match(AVM::get_actual(0)->get_str(), std::regex("-?[0-9]+\\.?[0-9]*"))){
                AVM::Memcell::Base::retval = new AVM::Memcell::Number(
                    std::stod(AVM::get_actual(0)->get_str().c_str())
                );
            } else {
                AVM::Memcell::Base::retval = new AVM::Memcell::Nil();
            }
        }
        Execute::restore_stack();
    }

    static inline double _deg_to_rad(double rad) {
        return rad*3.141/180;
    }
    void _sin(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("sin EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::NUMBER_M) {
            AVM::error("sin EXPECTS A STRING");
            exit(0);
        } else if(AVM::get_actual(0)->get_num() < 0 || AVM::get_actual(0)->get_num() > 360) {
            AVM::error("INVALID DEGREES");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            AVM::Memcell::Base::retval = new AVM::Memcell::Number(
                sin(_deg_to_rad(AVM::get_actual(0)->get_num()))
            );
        }
        Execute::restore_stack();
    }
    void _cos(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("cos EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::NUMBER_M) {
            AVM::error("cos EXPECTS A STRING");
            exit(0);
        } else if(AVM::get_actual(0)->get_num() < 0 || AVM::get_actual(0)->get_num() > 360) {
            AVM::error("INVALID DEGREES");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            AVM::Memcell::Base::retval = new AVM::Memcell::Number(
                cos(_deg_to_rad(AVM::get_actual(0)->get_num()))
            );
        }
        Execute::restore_stack();
    }
    void _sqrt(void) {
        unsigned n = AVM::get_total_actuals();
        if(n != 1) {
            AVM::error("sqrt EXPECTS EXACTLY 1 ARGUMENT");
            exit(0);
        } else if(AVM::get_actual(0)->get_type() != AVM::Memcell_t::NUMBER_M) {
            AVM::error("sqrt EXPECTS A NUMBER");
            exit(0);
        } else {
            delete AVM::Memcell::Base::retval;
            if(AVM::get_actual(0)->get_num() < 0) {
                AVM::Memcell::Base::retval = new AVM::Memcell::Nil();
            } else {
                AVM::Memcell::Base::retval = new AVM::Memcell::Number(
                    sqrt(AVM::get_actual(0)->get_num())
                );
            }
        }
        Execute::restore_stack();
    }

    void call_libfunc(const std::string& name) {
        if(_libfuncs.find(name) == _libfuncs.end()) {
            AVM::error("UNSUPPORTED LIBRARY FUNCTION: " + name);
            exit(0);
        } else {
            auto& topsp = AVM::Memcell::Base::topsp;
            auto& top = AVM::Memcell::Base::top;
            auto& total_actuals = AVM::Memcell::Base::total_actuals;
            topsp = top;
            total_actuals = 0;
            _libfuncs.at(name)();
        }
    }
}