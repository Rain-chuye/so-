#ifndef VM_COMPILER_HPP
#define VM_COMPILER_HPP

#include "vm_engine.hpp"
#include <map>
#include <string>

namespace VM {

class Compiler {
public:
    static std::vector<Instruction> compile_add_function() {
        // (arg0 + arg1)
        return {
            {OP_LOAD, 0},
            {OP_LOAD, 1},
            {OP_ADD, 0},
            {OP_RET, 0}
        };
    }

    static std::vector<Instruction> compile_hello_logic() {
        // Simple return 42 to signal success in VM
        return {
            {OP_PUSH, 42},
            {OP_RET, 0}
        };
    }
};

} // namespace VM

#endif
