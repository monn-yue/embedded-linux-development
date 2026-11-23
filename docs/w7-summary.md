# W7 IMU 可靠性与 systemd 服务测试报告

## 1. 测试环境

- 开发板：KICKPI K11C V1.2（RK3566）
- 操作系统：Debian 11.8，Linux 5.10.160
- 处理器架构：ARM64
- 传感器：ICM-45688-P
- 接口：/dev/i2c-3
- 测试日期：2026-11-23

## 2. systemd 服务管理

服务配置位于 deploy/imu-capture.service。

- 使用 linaro 用户及 i2c 组运行
- 每轮采集 36001 条六轴数据
- 配置 Restart=on-failure
- 配置 RestartSec=5s
- 支持开机自启动

服务启动、停止及状态查询均已测试通过。

## 3. 一小时连续采集测试

采集进程 PID：2606

- 样本总数：36001
- 第一条时间戳：1 ms
- 最后一条时间戳：3674491 ms
- 采集持续时间：3674490 ms（约 61 分 14 秒）
- Result：success
- ExecMainStatus：0
- NRestarts：0

测试结论：PASS

原始统计记录：w7-long-run-summary.txt

## 4. 重启恢复测试

启用 systemd 开机自启动后，正常重启 K11C。

重启后观察结果：

- 服务自动启动成功
- Active 状态为 active (running)
- 新采集进程 PID：522
- 六轴数据持续输出

测试结论：PASS

验证结束后，手动停止服务，并关闭开机自启动。

最终状态：

- ActiveState=inactive
- UnitFileState=disabled

## 5. 故障与异常记录

本轮一小时长测未观察到真实采集故障。

W5、W6 曾通过 strace 人为注入 ETIMEDOUT，
并验证程序能够报告错误和以非零退出码结束。

人为注入的超时不属于本轮真实故障。

## 6. 验收结论

- systemd 服务管理：PASS
- 连续采集至少一小时：PASS
- 采样数量与时间检查：PASS
- 正常退出且无异常重启：PASS
- 正常重启后自动恢复：PASS

W7 主要验收项目通过。
