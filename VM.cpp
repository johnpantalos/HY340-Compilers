#include"executors.hpp"

int main(int argc, char** argv) {
    --argc;
    ++argv;
    if(!argc) {
        std::cerr << "VM ERROR: NO INPUT FILE GIVEN" << std::endl;
        exit(0);
    }
    Machine_Code::Load(*argv);
    // Machine_Code::print_info();
    Execute::execute_all();
    return 0;
}