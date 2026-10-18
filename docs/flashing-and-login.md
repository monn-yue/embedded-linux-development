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

### 5. 官方镜像与恢复资料

#### 5.1 官方系统镜像

开发板：KICKPI K11C V1.2

系统版本：Debian 11

镜像类型：Multi

镜像文件：
update-rk3566-kickpi-k11c-v12-linux-debian-multi-20260730-161405.img

Windows 保存位置：
F:\K11C\images

镜像大小：
4264809034 字节

SHA256：
已计算并保存至 F:\K11C\images\image-checksum.txt

说明：
镜像已下载完成。
已计算本地 SHA256，但尚未与官方校验值比对。

#### 5.2 烧录工具

已准备以下工具：

- RKDevTool
- DriverAssistant

Windows 保存目录：
F:\K11C\tools

工具已下载并解压。

当前尚未执行重新烧录操作。

#### 5.3 eMMC 恢复计划

计划在 W10 完成自主重刷及恢复验证。

预计操作流程：

1. 备份开发板重要代码和配置。
2. 检查镜像 SHA256。
3. 准备 RKDevTool 和 USB 驱动。
4. 按照官方教程进入烧录模式。
5. 使用匹配的镜像烧录 eMMC。
6. 重启开发板并检查 Debian 11 系统。
7. 验证 SSH、串口和项目功能。

以上为计划流程，尚未实际执行。

#### 5.4 W1 最终结论

KICKPI K11C V1.2 已成功运行 Debian 11。

已完成以下验证：

- Ubuntu 主机开发环境正常。
- K11C 系统和硬件信息核验通过。
- SSH 登录成功。
- USB-TTL 串口通信成功。
- 官方 Debian 11 镜像已下载并保存校验信息。
- RKDevTool 和 DriverAssistant 已准备完成。
- Git 项目及工程文档已建立。

W1 的核心部署与验证任务已完成。

实际系统重刷与恢复验证保留至 W10。
