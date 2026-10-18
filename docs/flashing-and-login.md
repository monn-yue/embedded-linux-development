# K11C 烧录与登录记录 v1

## 硬件
- KICKPI K11C
- RK3566
- 4GB RAM / 32GB eMMC

## 系统
- 目标系统：Debian 11
- 电脑开发环境：Ubuntu 24.04
- 预装系统：待实机验证

## 官方资料
- https://doc.kickpi.cn/Products/Introduction/KICKPI-K11C/
- https://doc.kickpi.cn/Products/Beginner-Guide/KICKPI-K11C/

## 部署计划
1. 到货后检查 PCB 版本及硬件。
2. 启动预装系统并核对版本。
3. 验证串口登录。
4. 配置网络并验证 SSH。
5. 保存官方镜像和恢复流程，W10 验证重刷。

## 实机验证记录

### 1. 开发板信息

- 型号：KICKPI K11C V1.2
- SoC：Rockchip RK3566
- 内存：4GB，系统识别约 3.8GiB
- eMMC：32GB，系统识别约 29.1GiB
- 操作系统：Debian GNU/Linux 11 (bullseye)
- CPU 架构：aarch64
- Linux 内核：5.10.160

### 2. SSH 登录

主机环境：
- Windows + VMware Ubuntu 24.04
- Windows Wi-Fi 共享至物理以太网

网络信息：
- Windows 以太网 IP：192.168.137.1
- K11C IP：192.168.137.52

SSH 命令：

ssh linaro@192.168.137.52

结果：登录成功。

注意：IP 地址可能随着网络配置变化。

### 3. USB-TTL 串口调试

调试工具：
- USB-TTL：CH340
- USB ID：1a86:7523
- Ubuntu 设备：/dev/ttyUSB0
- 波特率：1500000

启动命令：

picocom -b 1500000 /dev/ttyUSB0

实际结果：
- 成功读取 Debian 11 启动日志
- 串口自动进入 root Shell
- whoami 输出 root
- tty 输出 /dev/ttyFIQ0
- 串口收发正常

退出 picocom：
Ctrl+A，然后 Ctrl+X

### 4. 已观察到的问题

启动日志出现：

Failed to start Set console font and keymap.

同时观察到 SKWIFI6621S Wi-Fi 驱动持续输出调试日志。

目前系统能够正常启动，SSH 与串口均可正常使用。

以上问题暂未深入排查。

### 5. 后续待完成

- 保存官方 Debian 11 镜像来源及校验信息
- 整理 RKDevTool 烧录工具与恢复教程
- W10 完成自主重刷和环境恢复验证
