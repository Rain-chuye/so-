#ifndef VM_ENGINE_HPP
#define VM_ENGINE_HPP

#include <vector>
#include <cstdint>
#include <iostream>
#include <stack>
#include "mba.hpp"

namespace VM {

enum OpCode : uint8_t {
    OP_PUSH = 0x01,
    OP_ADD  = 0x02,
    OP_SUB  = 0x03,
    OP_MUL  = 0x04,
    OP_LOAD = 0x05,  // Load from local "registers"
    OP_STORE = 0x06, // Store to local "registers"
    OP_PRINT = 0x07, // Specialized for demo
    OP_RET   = 0xFF
};

struct Instruction {
    OpCode op;
    int64_t operand;
};

class VirtualMachine {
public:
    int64_t execute(const std::vector<Instruction>& bytecode, const std::vector<int64_t>& args) {
        std::stack<int64_t> stack;
        std::vector<int64_t> regs = args;
        regs.resize(16, 0);

        for (size_t pc = 0; pc < bytecode.size(); ++pc) {
            const auto& inst = bytecode[pc];

            // Dispatcher obfuscated with MBA-like logic (symbolic)
            switch (inst.op) {
                case OP_PUSH:
                    stack.push(inst.operand);
                    break;
                case OP_ADD: {
                    int64_t b = stack.top(); stack.pop();
                    int64_t a = stack.top(); stack.pop();
                    // Use MBA for the addition
                    stack.push((a ^ b) + 2 * (a & b));
                    break;
                }
                case OP_SUB: {
                    int64_t b = stack.top(); stack.pop();
                    int64_t a = stack.top(); stack.pop();
                    stack.push(a - b);
                    break;
                }
                case OP_MUL: {
                    int64_t b = stack.top(); stack.pop();
                    int64_t a = stack.top(); stack.pop();
                    stack.push(a * b);
                    break;
                }
                case OP_LOAD:
                    stack.push(regs[inst.operand]);
                    break;
                case OP_STORE:
                    regs[inst.operand] = stack.top();
                    stack.pop();
                    break;
                case OP_PRINT:
                    std::cout << "[VM] Output: " << stack.top() << std::endl;
                    break;
                case OP_RET:
                    return stack.empty() ? 0 : stack.top();
                default:
                    return -1;
            }
        }
        return 0;
    }
};

} // namespace VM

#endif
