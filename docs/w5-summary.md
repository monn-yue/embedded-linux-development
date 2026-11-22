# W5：C++17 六轴采集与错误处理

## 实现

- `Imu` 对象打开 `/dev/i2c-3`，析构时关闭设备文件。初始化失败时也会关闭已打开的文件。
- 初始化时验证 `0x69` 上的芯片 ID 为 `0xe7`，启用加速度计和陀螺仪。
- 每组连续读取 12 字节，解码为三个加速度原始值和三个角速度原始值。
- `imu-read` 默认读取一组。`imu-read 10 --csv` 输出十组，每组一行。`elapsed_ms` 是从开始采样到该组读取完成的相对毫秒数。
- 读取失败时，把错误原因写到标准错误，并以状态码 `1` 退出。

## 构建与复现

在 VMware Ubuntu 的仓库目录交叉编译：

```bash
arm_build=$(mktemp -d)
cmake -S . -B "$arm_build" \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
    -DCMAKE_EXE_LINKER_FLAGS=-static
cmake --build "$arm_build"
file "$arm_build/imu-read"
```

将生成的程序通过 `scp` 复制到 K11C 的 `~/imu-read`。在 K11C 的 SSH 终端运行：

```bash
sudo "$HOME/imu-read" 10 --csv
sudo strace -e trace=ioctl -P /dev/i2c-3 \
    -e inject=ioctl:error=ETIMEDOUT:when=4 "$HOME/imu-read"
printf 'exit=%s\n' "$?"
sudo "$HOME/imu-read" 3 --csv
printf 'exit=%s\n' "$?"
```

## 实测结果

| 验收项 | 结果 |
| --- | --- |
| CMake 交叉编译 | 生成 ARM64 静态链接可执行文件 |
| 单次与连续读取 | 板上单次读取退出码为 `0`，连续读取输出 10 组六轴值 |
| CSV 与时间戳 | 1 行表头、10 行数据，每行 7 列，相邻样本间隔 102–103 ms |
| 读取超时 | 使用 `strace` 给第一次六轴读取注入 `ETIMEDOUT`，程序打印 `IMU read failed` 并返回 `1` |
| 超时后重新运行 | 输出 3 行数据，时间戳为 1、104、206 ms，返回 `0` |
