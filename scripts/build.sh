#!/usr/bin/env bash

set -euo pipefail

# 获取项目根目录
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

# 设置构建目录和输出文件
BUILD_DIR="${PROJECT_DIR}/build/arm64"
OUTPUT_FILE="${BUILD_DIR}/imu-read"

# ARM64 交叉编译器
CXX_COMPILER="aarch64-linux-gnu-g++"

echo "========== 检查编译环境 =========="

for tool in cmake "$CXX_COMPILER" file; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "错误：未找到命令 $tool" >&2
        exit 1
    fi
done

echo "编译环境检查通过"

echo ""
echo "========== 配置 ARM64 构建 =========="

mkdir -p "$BUILD_DIR"

cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
    -DCMAKE_EXE_LINKER_FLAGS=-static

echo ""
echo "========== 开始编译 =========="

cmake --build "$BUILD_DIR" --parallel 2

echo ""
echo "========== 检查编译结果 =========="

if [[ ! -x "$OUTPUT_FILE" ]]; then
    echo "错误：没有生成可执行文件" >&2
    exit 1
fi

FILE_INFO="$(file "$OUTPUT_FILE")"

echo "$FILE_INFO"

if [[ "$FILE_INFO" != *"ARM aarch64"* ]]; then
    echo "错误：生成文件不是 ARM64 程序" >&2
    exit 1
fi

if [[ "$FILE_INFO" != *"statically linked"* ]]; then
    echo "错误：生成文件不是静态链接程序" >&2
    exit 1
fi

echo ""
echo "BUILD PASS"
echo "生成文件：$OUTPUT_FILE"
