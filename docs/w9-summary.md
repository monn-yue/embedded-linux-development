# W9 实验总结：自动化构建、部署与运行

## 1. 项目目标

W9 的主要任务是将前期已经完成的 ARM64 交叉编译、文件传输和开发板运行过程整理为可重复使用的 Shell 脚本。

本周验收目标：

PC 编译 → 文件传输 → ARM64 开发板运行。

本周完成三个脚本：

- `scripts/build.sh`：交叉编译脚本
- `scripts/deploy.sh`：自动部署脚本
- `scripts/start.sh`：远程启动脚本

## 2. 项目环境

### 2.1 开发主机

- 操作系统：Ubuntu 24.04（VMware）
- CMake：用于项目配置和构建
- ARM64 交叉编译器：`aarch64-linux-gnu-g++`
- 传输方式：SSH、SCP

### 2.2 目标开发板

- 型号：KICKPI K11C V1.2
- SoC：Rockchip RK3566
- 操作系统：Debian 11.8
- Linux 内核：5.10.160
- CPU 架构：ARM64（aarch64）
- IMU 设备节点：`/dev/i2c-3`
- SSH 用户：`linaro`

开发板 IP 通过 DHCP 获取，可能发生变化。运行部署和启动脚本时，应使用当前实际 IP。

## 3. 交叉编译脚本 build.sh

### 3.1 功能

脚本自动完成：

1. 定位项目根目录。
2. 检查 CMake、ARM64 交叉编译器和 file 命令。
3. 创建 ARM64 构建目录。
4. 使用 CMake 配置 Linux ARM64 交叉编译环境。
5. 使用 ARM64 交叉编译器构建程序。
6. 检查生成文件是否为 ARM64 静态链接 ELF 可执行文件。

### 3.2 构建配置

构建目录：

`build/arm64/`

生成文件：

`build/arm64/imu-read`

主要构建参数：

- `CMAKE_SYSTEM_NAME=Linux`
- `CMAKE_SYSTEM_PROCESSOR=aarch64`
- `CMAKE_CXX_COMPILER=aarch64-linux-gnu-g++`
- `CMAKE_EXE_LINKER_FLAGS=-static`

CMakeLists.txt 使用 C++17，启用 `-Wall -Wextra -Wpedantic` 编译警告，并链接 Threads。

### 3.3 使用方法

在 Ubuntu 项目根目录执行：

```bash
./scripts/build.sh
```

### 3.4 实际验证

脚本执行成功，生成文件经 `file` 检查确认为：

- ELF 64-bit
- ARM aarch64
- statically linked

终端显示：

`BUILD PASS`

构建产物为 ARM64 静态链接可执行文件。

## 4. 自动部署脚本 deploy.sh

### 4.1 功能

脚本自动完成：

1. 检查 SSH、SCP 和 SHA256 工具。
2. 检查本地构建产物。
3. 验证 SSH 连接。
4. 计算本地文件 SHA256。
5. 使用 SCP 将文件传输到开发板临时文件。
6. 计算远端文件 SHA256。
7. 对比两端哈希值。
8. 校验一致后设置执行权限，并将临时文件移动到正式路径。

SHA256 校验不一致时，脚本会报告部署失败，不会将未通过校验的临时文件替换为正式程序。

### 4.2 部署位置

开发板目标路径：

`/home/linaro/imu-read-w9`

### 4.3 使用方法

```bash
./scripts/deploy.sh <开发板当前IP>
```

例如：

```bash
./scripts/deploy.sh 192.168.137.147
```

示例 IP 仅代表本次测试使用的地址，不保证以后保持不变。

### 4.4 实际验证

本次实际测试结果：

本地 SHA256：

`ab89136bbe26d367aaa3922ed8e2bf614494577a89d182f718fc9cd20d3ed6a5`

开发板 SHA256：

`ab89136bbe26d367aaa3922ed8e2bf614494577a89d182f718fc9cd20d3ed6a5`

两端 SHA256 完全一致。

开发板文件：

`/home/linaro/imu-read-w9`

文件权限：

`-rwxr-xr-x`

终端显示：

`DEPLOY PASS`

## 5. 远程启动脚本 start.sh

### 5.1 功能

脚本自动完成：

1. 检查输入参数。
2. 检查采样数量是否为正整数。
3. 检查 SSH 命令。
4. 验证开发板 SSH 连接。
5. 检查远端程序是否存在且可执行。
6. 通过 SSH 启动开发板上的 IMU 采集程序。
7. 将 CSV 数据输出到 Ubuntu 终端。

脚本默认采集 10 条数据，也可以通过第二个参数指定采样数量。

### 5.2 使用方法

默认采集 10 条：

```bash
./scripts/start.sh <开发板当前IP>
```

指定采集数量：

```bash
./scripts/start.sh <开发板当前IP> 100
```

远程执行的程序为：

`/home/linaro/imu-read-w9`

程序使用 `--csv` 参数输出数据。

### 5.3 实际验证

本次测试使用默认采样数量 10。

实际结果：

- 成功连接开发板。
- 成功检查远端程序。
- 成功访问 `/dev/i2c-3`。
- 成功读取六轴 IMU 数据。
- 成功输出 CSV 表头及 10 条数据。
- 程序正常结束，退出码为 0。

本次采集时间从 1ms 到 920ms，采样间隔约为 102ms。

终端显示：

`START PASS`

## 6. I2C 设备访问权限问题

### 6.1 问题现象

首次通过 SSH 直接运行程序时出现：

`IMU read failed: open /dev/i2c-3: Permission denied`

### 6.2 原因

开发板上的设备文件权限为：

`crw-rw---- root i2c /dev/i2c-3`

设备属于 root 用户和 i2c 用户组。

当时 `linaro` 用户不属于 i2c 组，因此无法直接访问该设备。

此前 systemd 服务配置中存在：

`SupplementaryGroups=i2c`

但通过 SSH 直接启动程序时，不会自动继承该 systemd 服务的附加用户组配置。

### 6.3 解决方法

在开发板执行：

```bash
sudo usermod -aG i2c linaro
```

重新登录 SSH，使新增用户组权限生效。

### 6.4 验证结果

重新运行 `start.sh` 后，程序成功访问 `/dev/i2c-3`，完成六轴数据采集。

本次权限修复通过实际硬件运行验证。

## 7. Windows ICS 网络辅助配置

开发环境使用 Windows WLAN 共享网络至物理以太网，为 K11C 提供网络连接。

此前发现开发板重新上线后，Windows ICS 有时需要重新初始化。

为减少手动恢复操作，在 Windows 中创建了自动恢复任务：

`K11C-ICS-Auto-Recovery`

任务使用 NetworkProfile 网络事件触发，调用正式 ICS 恢复脚本。

本次验证结果：

- 正式计划任务注册成功。
- 任务以 SYSTEM 身份运行。
- 任务触发后恢复 WLAN Public 和 Ethernet Private 共享。
- SharedAccess 服务运行正常。
- 网关恢复为 `192.168.137.1`。
- 正式恢复脚本日志显示 `SUCCESS: ICS recovery completed`。
- 任务执行结果 `LastTaskResult=0`。
- Ubuntu 重新 SSH 连接 K11C 成功。

该配置属于当前 Windows 开发主机的网络环境辅助措施，不属于仓库中的 Linux 构建、部署和启动脚本。

Windows 主机自身重启后的自动恢复场景尚未单独验证。

## 8. W9 完整流程验收

本周按照以下顺序完成实际测试：

### 第一步：交叉编译

```bash
./scripts/build.sh
```

结果：`BUILD PASS`

### 第二步：部署

```bash
./scripts/deploy.sh 192.168.137.147
```

结果：`DEPLOY PASS`

本地与远端 SHA256 一致。

### 第三步：远程启动

```bash
./scripts/start.sh 192.168.137.147
```

结果：`START PASS`

开发板实际采集 10 条六轴数据，CSV 输出正常。

### 最终验收结果

| 验收项目 | 结果 |
|---|---|
| ARM64 交叉编译 | 通过 |
| ELF 架构与静态链接检查 | 通过 |
| SSH 连接 | 通过 |
| SCP 文件传输 | 通过 |
| SHA256 完整性校验 | 通过 |
| 开发板远程启动 | 通过 |
| I2C 设备访问 | 通过 |
| 六轴 IMU 数据采集 | 通过 |
| CSV 数据输出 | 通过 |
| 程序正常退出 | 通过 |

W9 核心验收通过，实现了 PC 编译、文件传输和 ARM64 开发板运行的完整流程。

## 9. 当前限制与未完成验证

1. 开发板 IP 由 DHCP 分配，可能发生变化，需要使用当前实际 IP。
2. ICM-45688-P 专用数据手册尚未取得，目前保留 TDK 官方 HAL 驱动仓库作为资料来源。
3. 现有 systemd 服务仍指向旧程序路径 `/home/linaro/imu-read`，当前处于 disabled、inactive 状态，本周未修改该服务。
4. systemd 失败自动重启目前仅完成配置，尚未单独进行故障注入验证。
5. Windows ICS 自动恢复尚未单独验证 Windows 主机重启场景。

## 10. 本周总结

W9 完成了交叉编译、自动部署和远程启动三个脚本的编写及实际验证。

通过脚本将原本需要手动执行的构建、文件传输、完整性校验和开发板运行过程整理为可重复使用的工程流程。

同时解决了直接 SSH 启动程序时的 I2C 用户组权限问题。

本周核心验收目标全部完成。
