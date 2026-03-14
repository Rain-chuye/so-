#ifndef VM_ENGINE_HPP
#define VM_ENGINE_HPP

#include <vector>
#include <cstdint>
#include <stack>

namespace VMP {

enum OpCode : uint8_t {
    OP_PUSH = 0x1A,
    OP_ADD  = 0x2B,
    OP_SUB  = 0x3C,
    OP_MUL  = 0x4D,
    OP_XOR  = 0x5E,
    OP_LOAD = 0x6F,
    OP_STORE = 0x70,
    OP_JMP   = 0x81,
    OP_RET   = 0xFF
};

struct Instruction {
    OpCode op;
    int64_t operand;
};

} // namespace VMP

#endif
