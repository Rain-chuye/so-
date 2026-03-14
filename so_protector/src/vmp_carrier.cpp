
#include <iostream>
#include <vector>
#include <cstdint>
#include <stack>
#include <fstream>
#include <string>
#include <unistd.h>

namespace VM {
enum OpCode : uint8_t { OP_PUSH = 0x01, OP_ADD = 0x02, OP_SUB = 0x03, OP_MUL = 0x04, OP_LOAD = 0x05, OP_STORE = 0x06, OP_RET = 0xFF };
struct Instruction { OpCode op; int64_t operand; };

static bool is_debugged() {
    std::ifstream is("/proc/self/status");
    std::string line;
    while (std::getline(is, line)) {
        if (line.find("TracerPid:") != std::string::npos) {
            try {
                int pid = std::stoi(line.substr(10));
                return pid != 0;
            } catch (...) { return false; }
        }
    }
    return false;
}

class VirtualMachine {
public:
    int64_t execute(const std::vector<Instruction>& bytecode, const std::vector<int64_t>& args) {
        if (is_debugged()) {
            std::cerr << "[Security] Anti-Reverse: Debugger detected." << std::endl;
            _exit(1);
        }
        std::stack<int64_t> stack;
        std::vector<int64_t> regs = args;
        regs.resize(16, 0);
        for (const auto& inst : bytecode) {
            switch (inst.op) {
                case OP_PUSH: stack.push(inst.operand); break;
                case OP_ADD: { int64_t b = stack.top(); stack.pop(); int64_t a = stack.top(); stack.pop(); stack.push((a ^ b) + 2*(a & b)); break; }
                case OP_LOAD: stack.push(regs[inst.operand]); break;
                case OP_RET: return stack.top();
                default: break;
            }
        }
        return 0;
    }
};
}

static std::vector<VM::Instruction> vmp_code = {
{(VM::OpCode)5, 0},{(VM::OpCode)5, 1},{(VM::OpCode)2, 0},{(VM::OpCode)255, 0},
};

extern "C" {
    int add(int a, int b) {
        VM::VirtualMachine vm;
        return (int)vm.execute(vmp_code, {(int64_t)a, (int64_t)b});
    }
    void hello() { std::cout << "Hello from VMP Protected SO!" << std::endl; }
    const char* get_secret() { return "VMP Protected Secret"; }
}
