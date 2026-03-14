#include <iostream>
#include <string>

// Simulating JNI types
typedef void* JNIEnv;
typedef void* jobject;
typedef void* jstring;

extern "C" {

jstring Java_com_example_myapp_MainActivity_stringFromJNI(JNIEnv* env, jobject thiz) {
    std::cout << "[JNI] stringFromJNI called" << std::endl;
    return (jstring)"Hello from JNI (Protected)!";
}

int Java_com_example_myapp_MainActivity_addNumbers(JNIEnv* env, jobject thiz, int a, int b) {
    std::cout << "[JNI] addNumbers called with: " << a << ", " << b << std::endl;
    return a + b;
}

}
