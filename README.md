# 嵌入式 Linux 开发实践项目

## 一、项目目标

通过一个为期 12 周的工程实践项目，系统提升嵌入式 Linux 开发、调试与工程交付能力。

主要学习内容：

- Linux 系统管理与网络配置
- C/C++17 编程
- Git、GDB 和 CMake
- GPIO 与 I2C 通信
- ARM64 交叉编译
- IMU 六轴数据采集与自动化测试

## 二、硬件平台

- 开发板：KICKPI K11C V1.2（RK3566）
- 内存：4GB
- eMMC：32GB
- 传感器：ICM-45688-P + QMC6309

## 三、开发环境

- 开发主机：VMware Ubuntu 24.04（x86_64）
- 目标系统：Debian 11（ARM64）
- 目标板内核：Linux 5.10.160

## 四、每周项目进度

### W1：系统部署与基础环境（已验收）

- 核实 Debian 11 系统及开发板硬件信息
- 建立串口控制台与 SSH 连接
- 准备官方固件和系统恢复工具
- 初始化 Git 仓库

实验记录：
[系统烧录与登录](docs/flashing-and-login.md)

### W2：网络配置与程序调试（已验收）

- 检查 IP 地址、网关、DNS 和网络接口
- 排查 DHCP 和 ping 权限问题
- 配置 SSH 公钥免密登录
- 验证 scp 双向文件传输
- 学习 systemd 服务及 journal 日志
- 练习 C 语言指针、函数和 GCC 编译
- 使用 GDB 调试 C 程序

实验记录：

- [W2 实验总结](docs/w2-summary.md)
- [网络故障排查记录](docs/w2-network-troubleshooting.md)

源代码：

- [W2 C 语言练习](src/w2/main.c)

### W3：GPIO 与 ARM64 交叉编译（已验收）

- 验证 K11C 的 GPIO 输出，外接 LED 点亮约 5 秒后熄灭
- 使用杜邦线模拟按键输入，验证 GPIO 电平变化
- 引脚 28 在断开杜邦线时读取为 0，接通分压电路后读取为 1
- 在 Ubuntu 上交叉编译 C 程序，并在 K11C 上运行
- 程序成功输出 Hello, ARM64!，退出码为 0
- 完成 C 语言内存布局、malloc/free、结构体练习
- 比较 ARM32 与 ARM64 的数据类型大小

实验记录：

- [W3 实验总结与复现步骤](docs/w3-summary.md)

### W4：IMU 六轴数据采集（已验收）

- 将 ICM-45688-P 通过 I2C3 接口连接到 K11C
- I2C 设备地址为 0x69
- 读取 WHO_AM_I 寄存器，确认芯片身份值为 0xe7
- 成功读取三轴加速度及三轴角速度原始数据
- 通过倾斜和旋转传感器验证数据变化

实验记录：

- [W4 实验总结](docs/w4-summary.md)

硬件资料：

- [IMU 模块与接线记录](docs/hardware/README.md)

### W5：C++17 IMU 采集程序（已验收）

- 使用 CMake 构建 imu-read 采集程序
- 在 Ubuntu 上交叉编译 ARM64 可执行文件
- 将程序部署到 K11C 并成功运行
- 实现六轴连续采样、CSV 输出和经过时间记录
- 通过 strace 注入 I2C 读取超时
- 验证异常信息和非零退出码
- 确认异常测试后仍可正常采集

实验记录：

- [W5 实验总结与复现步骤](docs/w5-summary.md)

源代码：

- [IMU 采集程序](src/imu/)

### W6：Python 自动化测试（已验收）

- 使用 Python subprocess.run() 调用 IMU 采集程序
- 自动获取标准输出、错误输出及退出码
- 检查采样数量与时间戳是否符合预期
- 自动测试正常采集和模拟读取超时场景
- 根据测试结果输出 PASS 或 FAIL
- 记录板上测试环境与实际运行结果
- 完成自动化测试报告 v1

实验记录：

- [W6 自动化测试报告](docs/w6-summary.md)
- [W6 板上测试输出](docs/w6-imu-test-output.txt)

源代码：

- [Python 自动化测试脚本](src/imu/imu-test-w6.py)

### W7：可靠性与 systemd 服务管理（已验收）

- 编写 systemd 服务配置，管理 IMU 采集程序
- 使用 linaro 用户和 i2c 组权限运行采集服务
- 完成 36001 条六轴样本的连续采集
- 实测采样持续时间约 61 分 14 秒
- 采集进程正常结束，退出码为 0
- 长时间测试期间没有发生自动重启
- 验证 K11C 正常重启后能够自动恢复采集
- 验证结束后停止服务并关闭开机自启动

实验记录：

- [W7 可靠性测试报告](docs/w7-summary.md)
- [W7 长时间采集统计](docs/w7-long-run-summary.txt)

服务配置：

- [IMU systemd 服务文件](deploy/imu-capture.service)

### W8：干净环境重建与硬件资料整理（已验收）

- 从 GitHub 全新克隆项目到独立目录，验证干净环境重建。
- 使用 CMake 和 ARM64 交叉编译器成功构建 IMU 程序。
- 确认生成文件为 ARM64 静态链接 ELF 可执行文件。
- 通过 SSH 部署程序，并通过 SHA256 验证文件一致性。
- 在 K11C 上实际采集 10 条六轴数据，CSV 输出正常，退出码为 0。
- 整理 IMU 模块引脚定义、实际接线记录及厂商参考资料。

硬件资料：

- [IMU 模块与接线记录](docs/hardware/README.md)

说明：ICM-45688-P 专用数据手册尚未取得，目前保留 TDK 官方 HAL 驱动仓库作为参考资料。

### W9：自动化构建、部署与运行（已验收）

- 编写 ARM64 交叉编译脚本 `build.sh`。
- 编写自动部署脚本 `deploy.sh`，实现 SCP 传输及 SHA256 校验。
- 编写远程启动脚本 `start.sh`，通过 SSH 启动开发板上的 IMU 程序。
- 解决 linaro 用户访问 I2C 设备的用户组权限问题。
- 完成 PC 编译、文件传输和 ARM64 开发板运行的完整流程。
- 实际采集 10 条六轴数据，CSV 输出正常，程序退出码为 0。

实验记录：

- [W9 自动化构建、部署与运行总结](docs/w9-summary.md)

## 五、构建与运行

在 VMware Ubuntu 24.04 中进入项目根目录，按照以下顺序执行。

### 1. ARM64 交叉编译

```bash
./scripts/build.sh
```

生成文件：

`build/arm64/imu-read`

### 2. 部署到 K11C

```bash
./scripts/deploy.sh <开发板当前IP>
```

脚本自动完成 SSH 连接、SCP 文件传输和 SHA256 完整性校验。

部署位置：

`/home/linaro/imu-read-w9`

### 3. 远程启动 IMU 程序

默认采集 10 条数据：

```bash
./scripts/start.sh <开发板当前IP>
```

指定采样数量：

```bash
./scripts/start.sh <开发板当前IP> 100
```

程序通过 SSH 远程运行，CSV 数据返回 Ubuntu 终端。

注意：

- 开发板 IP 由 DHCP 分配，使用前应确认当前 IP。
- 部署和启动脚本使用 `linaro` 用户。
- SSH 需要配置免密登录，以满足脚本的非交互执行要求。
- 构建产物位于 `build/`，已通过 `.gitignore` 排除。

完整实验记录见 [W9 自动化构建、部署与运行总结](docs/w9-summary.md)。

## 六、当前项目状态

W1–W9 已完成当前阶段的核心实践任务和对应验收。

W8 完成干净环境重建、ARM64 程序构建部署、实际 IMU 数据采集及硬件资料整理。

W9 完成自动化交叉编译、部署和远程启动脚本，并通过完整流程实测。

当前 IMU systemd 服务处于停止状态，且已关闭开机自启动。其服务配置仍指向旧程序路径，W9 使用独立的 SSH 启动脚本，未修改该服务。systemd 失败自动重启尚未进行单独故障注入验证。

ICM-45688-P 专用数据手册尚未取得，相关资料限制见硬件文档。

后续按照《安克嵌入式 Linux 实习准备方案 v4 最终版》继续推进 W10–W12。

项目原则：以真实代码、实际测试结果、故障记录和可复现的工程文档作为验收依据。
