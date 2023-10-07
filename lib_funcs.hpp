#include"avm_types.hpp"
#include<iostream>
#include<unordered_map>
#include<functional>
#include<cmath>

namespace Execute {
    extern void restore_stack(void);
    unsigned get_envvalue(unsigned);
}

namespace Library {
    typedef std::function<void(void)> _LibFunc;
    extern const std::unordered_map<std::string, _LibFunc> _libfuncs;
    void _print(void);
    void _typeof(void);
    void _input(void);
    void _objectmemberkeys(void);
    void _objecttotalmembers(void);
    void _objectcopy(void);
    void _totalarguments(void);
    void _argument(void);
    void _strtonum(void);
    void _sin(void);
    void _cos(void);
    void _sqrt(void);

    void call_libfunc(const std::string&);
}