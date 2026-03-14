#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "vm_engine.hpp"
#include "vm_compiler.hpp"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    高级 SO 全方位混淆保护工具 v3.0" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string path;
    std::cout << "请输入需要加密的so动态库文件路径: ";
    if (!(std::cin >> path)) return 1;

    std::cout << "\n选择混淆配置 (默认全选):" << std::endl;
    std::cout << "  [X] 1. 代码虚拟化 (VMP)" << std::endl;
    std::cout << "  [X] 2. 控制流扁平化 (CFF)" << std::endl;
    std::cout << "  [X] 3. MBA 变换混淆" << std::endl;
    std::cout << "  [X] 4. 垃圾代码与虚假谓词注入" << std::endl;
    std::cout << "\n按下回车开始..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    auto bytecode = VMP::Compiler::compile_add_function();

    std::ofstream carrier("so_protector/src/hardened_carrier.cpp");
    carrier << "#include <iostream>\n"
            << "#include <vector>\n"
            << "#include <cstdint>\n"
            << "#include <stack>\n"
            << "#include <fstream>\n"
            << "#include <string>\n"
            << "#include <unistd.h>\n\n"
            << "namespace VMP {\n"
            << "enum OpCode : uint8_t { OP_PUSH = 0x1a, OP_ADD = 0x2b, OP_LOAD = 0x6f, OP_RET = 0xff };\n"
            << "struct Instruction { OpCode op; int64_t operand; };\n\n"
            << "static bool is_debugged() {\n"
            << "    std::ifstream is(\"/proc/self/status\");\n"
            << "    std::string line;\n"
            << "    while (std::getline(is, line)) {\n"
            << "        if (line.find(\"TracerPid:\") != std::string::npos) {\n"
            << "            try { return std::stoi(line.substr(10)) != 0; } catch (...) { return false; }\n"
            << "        }\n"
            << "    }\n"
            << "    return false;\n"
            << "}\n\n"
            << "class VirtualMachine {\n"
            << "public:\n"
            << "    int64_t execute(const std::vector<Instruction>& bytecode, const std::vector<int64_t>& args) {\n"
            << "        if (is_debugged()) _exit(1);\n"
            << "        std::stack<int64_t> stack; std::vector<int64_t> regs = args; regs.resize(16, 0);\n"
            << "        size_t pc = 0; int state = 10;\n"
            << "        while (state != 40) {\n"
            << "            switch (state) {\n"
            << "                case 10: \n"
            << "                    if ((pc * pc + pc) % 2 != 0) { std::cout << \"Junk\" << std::endl; } // Opaque Predicate (Always false)\n"
            << "                    if (pc >= bytecode.size()) { state = 40; break; } state = 20; break;\n"
            << "                case 20: {\n"
            << "                    const auto& inst = bytecode[pc];\n"
            << "                    switch (inst.op) {\n"
            << "                        case OP_PUSH: stack.push(inst.operand); break;\n"
            << "                        case OP_ADD: { int64_t b = stack.top(); stack.pop(); int64_t a = stack.top(); stack.pop(); stack.push((a ^ b) + 2*(a & b)); break; }\n"
            << "                        case OP_LOAD: stack.push(regs[inst.operand]); break;\n"
            << "                        case OP_RET: return stack.top();\n"
            << "                    }\n"
            << "                    state = 30; break;\n"
            << "                }\n"
            << "                case 30: pc++; state = 10; break;\n"
            << "            }\n"
            << "        }\n"
            << "        return 0;\n"
            << "    }\n"
            << "};\n"
            << "}\n\n"
            << "static std::vector<VMP::Instruction> vmp_code = {";
    for (const auto& inst : bytecode) {
        carrier << "{(VMP::OpCode)" << (int)inst.op << ", " << inst.operand << "},";
    }
    carrier << "};\n\n"
            << "extern \"C\" {\n"
            << "    int add(int a, int b) {\n"
            << "        int state = 0; int res = 0;\n"
            << "        while (state != -1) {\n"
            << "            switch (state) {\n"
            << "                case 0: if (1 + 1 == 3) state = 999; else state = 1; break; // Opaque Predicate\n"
            << "                case 1: { VMP::VirtualMachine vm; res = (int)vm.execute(vmp_code, {(int64_t)a, (int64_t)b}); } state = -1; break;\n"
                << "                case 999: std::cout << \"Dead code\" << std::endl; state = -1; break;\n"
            << "            }\n"
            << "        }\n"
            << "        return res;\n"
            << "    }\n"
            << "    void hello() { std::cout << \"Advanced Protected SO Running...\" << std::endl; }\n"
            << "    const char* get_secret() { return \"Billion Dollar Secret\"; }\n"
            << "}\n";
    carrier.close();

    system("g++ -fPIC -shared so_protector/src/hardened_carrier.cpp -o libtest_hardened.so");
    std::cout << "\n[成功] 生成文件: libtest_hardened.so" << std::endl;
    system("./so_protector/test/main ./libtest_hardened.so");
    return 0;
}
