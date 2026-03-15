#!/bin/bash
# Android 编译脚本

echo "开始编译..."
COMPILER=""
if command -v clang++ &> /dev/null; then COMPILER="clang++"; fi
if [ -z "$COMPILER" ] && command -v g++ &> /dev/null; then COMPILER="g++"; fi

if [ -z "$COMPILER" ]; then
    echo "错误: 请安装 clang 或 g++"
else
    $COMPILER -O2 src/protector.cpp -o protector
    chmod +x protector
    echo "编译完成! 使用方法: ./protector"
fi
