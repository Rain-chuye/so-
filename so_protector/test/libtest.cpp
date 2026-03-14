extern "C" {
    int add(int a, int b) { return a + b; }
    const char* get_secret() { return "Protected Secret"; }
    void hello() { /* do nothing */ }
}
