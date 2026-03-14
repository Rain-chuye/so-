#include <iostream>
#include <vector>
#include <cstdint>
#include <stack>
#include <fstream>
#include <string>
#include <unistd.h>

namespace VMP {
enum OpCode : uint8_t { OP_PUSH = 0x1a, OP_ADD = 0x2b, OP_LOAD = 0x6f, OP_RET = 0xff };
struct Instruction { OpCode op; int64_t operand; };

static bool is_debugged() {
    std::ifstream is("/proc/self/status");
    std::string line;
    while (std::getline(is, line)) {
        if (line.find("TracerPid:") != std::string::npos) {
            try { return std::stoi(line.substr(10)) != 0; } catch (...) { return false; }
        }
    }
    return false;
}

class VirtualMachine {
public:
    int64_t execute(const std::vector<Instruction>& bytecode, const std::vector<int64_t>& args) {
        if (is_debugged()) _exit(1);
        std::stack<int64_t> stack; std::vector<int64_t> regs = args; regs.resize(16, 0);
        size_t pc = 0; int state = 10;
        while (state != 40) {
            switch (state) {
                case 10:
                    if ((pc * pc + pc) % 2 != 0) { std::cout << "Junk" << std::endl; } // Opaque Predicate (Always false)
                    if (pc >= bytecode.size()) { state = 40; break; } state = 20; break;
                case 20: {
                    const auto& inst = bytecode[pc];
                    switch (inst.op) {
                        case OP_PUSH: stack.push(inst.operand); break;
                        case OP_ADD: { int64_t b = stack.top(); stack.pop(); int64_t a = stack.top(); stack.pop(); stack.push((a ^ b) + 2*(a & b)); break; }
                        case OP_LOAD: stack.push(regs[inst.operand]); break;
                        case OP_RET: return stack.top();
                    }
                    state = 30; break;
                }
                case 30: pc++; state = 10; break;
            }
        }
        return 0;
    }
};
}

static std::vector<VMP::Instruction> vmp_code = {{(VMP::OpCode)111, 0},{(VMP::OpCode)111, 1},{(VMP::OpCode)43, 0},{(VMP::OpCode)255, 0},};

extern "C" {
    int add(int a, int b) {
        int state = 0; int res = 0;
        while (state != -1) {
            switch (state) {
                case 0: if (1 + 1 == 3) state = 999; else state = 1; break; // Opaque Predicate
                case 1: { VMP::VirtualMachine vm; res = (int)vm.execute(vmp_code, {(int64_t)a, (int64_t)b}); } state = -1; break;
                case 999: std::cout << "Dead code" << std::endl; state = -1; break;
            }
        }
        return res;
    }
    void hello() { std::cout << "Advanced Protected SO Running..." << std::endl; }
    const char* get_secret() { return "Billion Dollar Secret"; }
}
