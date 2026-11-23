# W2 嵌入式 Linux 学习与验收总结

## 1. 本周目标

依据《嵌入式 Linux 开发实践计划 v4》，W2 重点完成：

- Linux 有线网络、IP、网关、DNS 检查
- SSH 密钥认证与 scp 文件传输
- systemctl、journalctl 服务与日志检查
- C 语言函数、指针基础
- GCC 编译及 GDB 断点、单步调试
- 真实网络故障排查与工程记录

## 2. 开发环境

- 主机：VMware Ubuntu 24.04，x86_64
- 开发板：KICKPI K11C V1.2，RK3566
- 系统：Debian 11，Linux 5.10.160，ARM64
- GCC：13.3.0
- GDB：15.1
- Git 分支：main

## 3. Linux 网络实践

### 3.1 网络配置

- 开发板网卡：eth0
- IPv4：192.168.137.52/24
- 默认网关：192.168.137.1
- Windows Wi-Fi 共享至物理以太网
- wlan0 已识别，未连接 Wi-Fi

### 3.2 网络验证

- Windows 网关 ping：0% 丢包
- 外网 1.1.1.1 ping：0% 丢包
- example.com DNS 解析成功
- SSH TCP 22 端口正常监听

### 3.3 真实故障

本周排查了两次实际故障：

1. DHCP 地址获取失败：重新初始化 Windows 网络共享后恢复。
2. 普通用户 ping 权限异常：临时调整 ping_group_range 后恢复。

详细排查过程见 [W2 网络故障排查记录](w2-network-troubleshooting.md)。

其中 DHCP 故障的 Windows 内部根因尚未完全确定；ping 权限修复未配置永久生效。

## 4. SSH 与文件传输

### 4.1 SSH 密钥

- 生成独立 Ed25519 密钥对
- 使用 ssh-copy-id 部署公钥
- 成功通过指定私钥登录 K11C
- 服务端日志确认公钥认证成功

私钥保存在 Ubuntu 用户的 ~/.ssh 目录，未加入 Git 仓库。

### 4.2 scp 文件传输

- Ubuntu → K11C：成功
- K11C → Ubuntu：成功
- 两个方向均通过 SHA-256 验证文件完整性

## 5. 系统服务与日志

掌握以下命令：

- systemctl status ssh
- systemctl is-active ssh
- systemctl is-enabled ssh
- journalctl -u ssh -n 20
- journalctl -b -u ssh -n 15
- uptime -p
- timedatectl status

验证结果：

- SSH 服务 active (running)
- SSH 服务 enabled
- journalctl 能查看密码及公钥认证记录
- 发现系统启动日志时间异常，实际运行时间与显示的日历时间跨度不一致

## 6. C 语言与 GDB

### 6.1 C 语言

源码：[src/w2/main.c](../src/w2/main.c)

完成内容：

- C 函数定义及参数传递
- 指针声明、取地址和解引用
- const 指针参数的使用
- GCC 编译与运行

编译命令：

```bash
gcc -Wall -Wextra -g -O0 -std=c11 src/w2/main.c -o /tmp/w2-c-demo
```

运行结果：

```text
x = 10
y = 20
*px = 10
*py = 20
sum = 30
```

### 6.2 GDB 调试

完成以下操作：

- break：设置函数断点
- run：启动程序
- next：单步执行，不进入函数调用
- step：进入函数内部
- print：查看变量和指针内容
- info args：查看函数参数
- backtrace：查看函数调用栈
- continue：继续执行

成功观察 main() 调用 add() 的过程，验证 result = 30。

## 7. Git 成果

本周主要提交：

- a17145d：DHCP 与 ping 网络故障排查记录
- 5c55715：C 指针与 GDB 练习程序

## 8. 最终验收

W2 核心技术任务全部通过。

已具备基本的 Linux 网络检查、SSH 文件传输、系统日志查看、C 程序编译和 GDB 调试能力。

后续需要继续巩固 C/Linux 基础，并在 W3 按 v4 计划开展 GPIO 实机测试与 ARM64 交叉编译。
