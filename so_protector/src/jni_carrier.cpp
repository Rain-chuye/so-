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
class VM {
public:
    int64_t run(const std::vector<Instruction>& code, const std::vector<int64_t>& args) {
        std::stack<int64_t> s; std::vector<int64_t> r = args; r.resize(16);
        size_t pc = 0; int state = 1;
        while (state != 0) {
            switch (state) {
                case 1: if (pc >= code.size()) state = 0; else state = 2; break;
                case 2: {
                    auto& i = code[pc];
                    switch (i.op) {
                        case OP_PUSH: s.push(i.operand); break;
                        case OP_ADD: { int64_t b=s.top(); s.pop(); int64_t a=s.top(); s.pop(); s.push((a ^ b) + 2*(a & b)); break; }
                        case OP_LOAD: s.push(r[i.operand]); break;
                        case OP_RET: return s.top();
                    } pc++; state = 1; break;
                }
            }
        }
        return 0;
    }
};
}

static std::vector<VMP::Instruction> add_code = {{VMP::OP_LOAD, 2}, {VMP::OP_LOAD, 3}, {VMP::OP_ADD, 0}, {VMP::OP_RET, 0}};

extern "C" {
    int Java_com_example_myapp_MainActivity_addNumbers(void* env, void* thiz, int a, int b) {
        int state = 1; int res = 0;
        while (state != 0) {
            switch(state) {
                case 1: { VMP::VM vm; res = (int)vm.run(add_code, {0, 0, (int64_t)a, (int64_t)b}); } state = 0; break;
            }
        }
        return res;
    }
    void* Java_com_example_myapp_MainActivity_stringFromJNI(void* env, void* thiz) {
        return (void*)"JNI Content Protected by VMP";
    }
}
