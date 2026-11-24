# W6 IMU 自动化测试报告 v1

## 测试环境

- 目标板 KICKPI K11C V1.2，ICM-45688-P，I²C 设备 `/dev/i2c-3`
- Debian 11.8，Linux 5.10.160，aarch64，Python 3.9.2
- W5 采集程序的源码参考提交 `a7b63a0`
- 板上记录时间 `2026-11-22T15:32:24+00:00`
- [板上运行原始输出](w6-imu-test-output.txt)

## 测试结果

| 场景 | 通过条件 | 实测结果 |
| --- | --- | --- |
| 正常采集 | 采集命令以 `0` 退出，返回 3 条样本，时间戳严格递增 | PASS |
| 模拟读取超时 | `strace` 注入 `ETIMEDOUT`，`imu-read` 打印读取失败，采集命令以 `1` 退出 | PASS |

自动化脚本输出 `overall = PASS`。模拟超时由 `strace` 注入。另一次文件传输曾因板上的 SSH 客户端配置报错而中止，改由 VMware Ubuntu 发起复制后成功。

## 复现步骤

按 [W5 说明](w5-summary.md) 构建并部署 `/home/linaro/imu-read`。K11C 需要有 `python3`、`strace` 和可用的 `sudo`。

在 VMware Ubuntu 的仓库根目录，将 `K11C_IP` 换成板子的当前 IP，再把脚本复制到板上。

```bash
scp src/imu/imu-test-w6.py linaro@K11C_IP:/home/linaro/imu-test-w6.py
```

在 K11C 上执行两项测试，查看最后一行 `overall`。

```bash
python3 /home/linaro/imu-test-w6.py
```

如需单独查看超时发生的位置，在 K11C 上运行下面的命令。预期看到 `(INJECTED)`、`IMU read failed: read IMU registers: Connection timed out` 和退出码 `1`。

```bash
sudo strace -e trace=ioctl -P /dev/i2c-3 \
    -e inject=ioctl:error=ETIMEDOUT:when=4 /home/linaro/imu-read
```
