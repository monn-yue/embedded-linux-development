#!/usr/bin/env bash

set -euo pipefail

# 获取项目目录
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

# 开发板连接配置
REMOTE_USER="${REMOTE_USER:-linaro}"
REMOTE_DIR="/home/linaro"
REMOTE_FILE="${REMOTE_DIR}/imu-read-w9"

# 检查参数
if [[ $# -lt 1 || $# -gt 2 ]]; then
    echo "用法：$0 <开发板IP> [采样数量]" >&2
    exit 2
fi

REMOTE_HOST="$1"
SAMPLE_COUNT="${2:-10}"
REMOTE="${REMOTE_USER}@${REMOTE_HOST}"

# 验证采样数量
if [[ ! "$SAMPLE_COUNT" =~ ^[1-9][0-9]*$ ]]; then
    echo "错误：采样数量必须是正整数" >&2
    exit 2
fi

echo "========== 检查启动环境 ==========" >&2

if ! command -v ssh >/dev/null 2>&1; then
    echo "错误：未找到 SSH 命令" >&2
    exit 1
fi

echo "目标开发板：$REMOTE" >&2
echo "采样数量：$SAMPLE_COUNT" >&2

echo "========== 检查 SSH 连接 ==========" >&2

ssh -o BatchMode=yes -o ConnectTimeout=5 "$REMOTE" \
    "if [ ! -x '$REMOTE_FILE' ]; then
        echo '错误：开发板程序不存在或不可执行' >&2
        exit 1
    fi"

echo "SSH 和程序检查通过" >&2

echo "========== 启动 IMU 采集 ==========" >&2

ssh -o BatchMode=yes "$REMOTE" \
    "exec '$REMOTE_FILE' '$SAMPLE_COUNT' --csv"

echo "START PASS" >&2
