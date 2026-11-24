#!/usr/bin/env bash

set -euo pipefail

# 获取项目目录
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

# 本地编译产物
LOCAL_FILE="${PROJECT_DIR}/build/arm64/imu-read"

# 开发板连接信息
REMOTE_USER="${REMOTE_USER:-linaro}"
REMOTE_DIR="/home/linaro"
REMOTE_FILE="${REMOTE_DIR}/imu-read-w9"

# 检查参数
if [[ $# -ne 1 ]]; then
    echo "用法：$0 <开发板IP>" >&2
    exit 2
fi

REMOTE_HOST="$1"
REMOTE="${REMOTE_USER}@${REMOTE_HOST}"

echo "========== 检查部署环境 =========="

for tool in ssh scp sha256sum; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "错误：未找到命令 $tool" >&2
        exit 1
    fi
done

if [[ ! -x "$LOCAL_FILE" ]]; then
    echo "错误：编译产物不存在，请先运行 build.sh" >&2
    exit 1
fi

LOCAL_HASH_LINE="$(sha256sum "$LOCAL_FILE")"
LOCAL_SHA="${LOCAL_HASH_LINE%% *}"

echo "本地 SHA256：$LOCAL_SHA"

echo ""
echo "========== 检查 SSH 连接 =========="

if ! ssh -o BatchMode=yes "$REMOTE" \
    "printf '%s\n' 'SSH connection OK'"; then
    echo "错误：无法通过 SSH 连接开发板" >&2
    exit 1
fi

if ! ssh -o BatchMode=yes "$REMOTE" \
    "command -v sha256sum >/dev/null 2>&1"; then
    echo "错误：开发板缺少 sha256sum" >&2
    exit 1
fi

echo ""
echo "========== 传输文件 =========="

REMOTE_TMP="${REMOTE_FILE}.tmp.$$"

scp -o BatchMode=yes "$LOCAL_FILE" "${REMOTE}:${REMOTE_TMP}"

echo ""
echo "========== 校验文件完整性 =========="

REMOTE_HASH_LINE="$(
    ssh -o BatchMode=yes "$REMOTE" \
    "sha256sum '$REMOTE_TMP'"
)"

REMOTE_SHA="${REMOTE_HASH_LINE%% *}"

echo "本地 SHA256：$LOCAL_SHA"
echo "远端 SHA256：$REMOTE_SHA"

if [[ "$LOCAL_SHA" != "$REMOTE_SHA" ]]; then
    ssh "$REMOTE" "rm -f '$REMOTE_TMP'" || true
    echo "错误：SHA256 不一致，部署失败" >&2
    exit 1
fi

echo ""
echo "========== 完成部署 =========="

ssh -o BatchMode=yes "$REMOTE" \
    "chmod 755 '$REMOTE_TMP' && mv -f '$REMOTE_TMP' '$REMOTE_FILE'"

ssh -o BatchMode=yes "$REMOTE" \
    "ls -lh '$REMOTE_FILE' && sha256sum '$REMOTE_FILE'"

echo ""
echo "DEPLOY PASS"
echo "部署位置：$REMOTE_FILE"
