#ifndef VM_COMPILER_HPP
#define VM_COMPILER_HPP

#include "vm_engine.hpp"

namespace VMP {

class Compiler {
public:
    static std::vector<Instruction> compile_add_function() {
        return {
            {OP_LOAD, 0},
            {OP_LOAD, 1},
            {OP_ADD, 0},
            {OP_RET, 0}
        };
    }
};

} // namespace VMP

#endif
