# W3 实验记录

本周在 KICKPI K11C V1.2 上完成了外接 LED 输出、用杜邦线模拟按键输入，以及最小 ARM64 程序的编译和部署。Ubuntu 24.04 x86_64 虚拟机负责开发和编译，K11C 运行 Debian 11 ARM64，板上安装了 `gpiod` 和 `libgpiod2` `1.6.2-1`。

## GPIO 输出与外接 LED

官方 K11C V1.2 引脚图把物理 Pin 8 标为 `GPIO3_C6`，属于 3.3 V I/O。它对应 `gpiochip3` 的线路 22。板上已有名为 `gpio3c6` 的 LED 类设备，因此本次通过 `/sys/class/leds/gpio3c6/brightness` 控制这条线路。物理 Pin 6 为 GND。 [K11C 使用手册](https://doc.kickpi.cn/Products/Beginner-Guide/KICKPI-K11C/) · [引脚和电压图](https://doc.kickpi.cn/Products/Peripherals-and-Interfaces/Pinout/#kickpi-k11c) · [KICKPI GPIO 文档](https://doc.kickpi.cn/Products/Peripherals-and-Interfaces/GPIO/)

断电时按 `Pin 8 → 330 Ω 电阻 → LED 长腿 → LED 短腿 → Pin 6（GND）` 的顺序串联。以下命令在 K11C 的 SSH 终端执行。

```bash
sudo sh -c '
led=/sys/class/leds/gpio3c6/brightness
printf "1\n" > "$led" || exit 1
cat "$led"
sleep 5
printf "0\n" > "$led" || exit 1
cat "$led"
'
```

终端依次读回 `1` 和 `0`，面包板上的黄色 LED 亮约 5 秒后熄灭。

## GPIO 输入与杜邦线模拟按键

物理 Pin 28 对应 `GPIO0_C6`，属于 1.8 V I/O，在当前系统中对应 `gpiochip0` 的线路 22。`pinmux-pins` 中该引脚的 `MUX` 和 `GPIO` 均未被申领。`GPIO4_A2` 到 `GPIO4_A5` 在 `gpioinfo` 中显示 `unused`，而 `pinmux-pins` 显示它们由 UART7 和 UART9 使用。另外，`GPIO1_A0` 和 `GPIO1_A1` 由 I2C3 使用。选脚时结合占用、复用和电压信息。 [K11C 使用手册](https://doc.kickpi.cn/Products/Beginner-Guide/KICKPI-K11C/) · [引脚和电压图](https://doc.kickpi.cn/Products/Peripherals-and-Interfaces/Pinout/#kickpi-k11c)

### 上拉排查

Pin 28 空载时，`gpioget -B pull-up` 和 `gpioget -B pull-down` 都读到 `0`。在 `gpiomon -B pull-up gpiochip0 22` 持续监听期间，`gpioinfo` 显示 `input` 和 `[used pull-up]`，而 `pinconf-pins` 仍报告 `input bias pull down`。这次板上的上拉请求未改变观察到的偏置，因此输入测试改用外部分压电路。 [Debian `gpioget` 手册](https://manpages.debian.org/bullseye/gpiod/gpioget.1.en.html) · [Linux GPIO v1 输入请求](https://docs.kernel.org/userspace-api/gpio/gpio-get-linehandle-ioctl.html)

以下命令分别查看 Pin 28 的复用归属、引脚配置和 GPIO 线路信息。

```bash
pinctrl=/sys/kernel/debug/pinctrl/pinctrl-rockchip-pinctrl
sudo grep -F '(gpio0-22):' "$pinctrl/pinmux-pins"
sudo grep -F '(gpio0-22)' "$pinctrl/pinconf-pins"
sudo gpioinfo gpiochip0 | grep -E '^[[:space:]]*line[[:space:]]+22:'
```

### 接线和读数

面包板右侧 `f–j` 区域串联四颗标称 330 Ω 电阻，依次连接第 `15 → 18 → 21 → 24 → 27` 排。上下各两颗串联得到 660 Ω，第 21 排是分压中点。

| 板上针脚 | 面包板位置 | 作用 |
|---|---|---|
| Pin 3（GND） | 右侧第 27 排 | 分压底端接地 |
| Pin 28（GPIO0_C6） | 右侧第 21 排 | 读取分压中点 |
| Pin 1（3.3 V） | 右侧第 15 排 | 接到分压顶端，模拟按下 |

每次改线前正常关机并断电，重新上电后在 K11C 的 SSH 终端用下列命令读一次。

```bash
sudo gpioget -B as-is gpiochip0 22
```

| 模拟状态 | 第 15 排接法 | Pin 28 实测读数 |
|---|---|---|
| 松开 | 留空 | `0` |
| 按下 | 接 Pin 1 的 3.3 V | `1` |

接通时，空载分压中点的计算值为 `3.3 V × 660 Ω ÷ (660 Ω + 660 Ω) ≈ 1.65 V`。这次实测记录的是 Linux 读取到的逻辑电平。开关动作由断电时插拔杜邦线模拟，两种状态分别上电读取。

## ARM64 交叉编译和部署

在 Ubuntu 虚拟机中安装 `gcc-aarch64-linux-gnu`。`aarch64-linux-gnu-gcc -dumpmachine` 返回 `aarch64-linux-gnu`，确认目标为 ARM64 Linux。源码是 [hello_arm64.c](../src/w3/hello_arm64.c)。

在 **Ubuntu 虚拟机**的项目目录编译并上传。输入本次 SSH 登录使用的开发板 IP。

```bash
aarch64-linux-gnu-gcc -static src/w3/hello_arm64.c -o /tmp/hello_arm64 &&
file /tmp/hello_arm64

read -r -p 'K11C IPv4: ' board_ip
scp -i ~/.ssh/id_ed25519_k11c -o IdentitiesOnly=yes \
    /tmp/hello_arm64 "linaro@$board_ip:~/"
```

`file` 识别出 `ELF 64-bit`、`ARM aarch64` 和 `statically linked`。静态链接让这个最小程序可以在板上的 Debian 11 环境中运行。上传命令从 Ubuntu 发起，因为编译生成的文件和 SSH 私钥都在这台虚拟机上。

登录 **K11C** 后运行程序，并检查架构和退出码。

```bash
uname -m
~/hello_arm64
echo "exit=$?"
```

板上架构为 `aarch64`，程序打印 `Hello, ARM64!` 并以 `exit=0` 正常退出。对应的源码提交为 `3694300`。

## C 基础复习

| 练习 | 实测结果 |
|---|---|
| [C 内存布局](../src/w3/memory_layout.c) | `global_data` 初值为 `42`，`nm` 标记为 `D`。`global_zero` 初值为 `0`，标记为 `B`。 |
| [`malloc/free`](../src/w3/malloc_demo.c) | 局部变量打印 `42`，动态申请的空间写入并打印 `99`，使用完毕后调用 `free`。 |
| [结构体](../src/w3/struct_demo.c) | 在 Ubuntu x86_64 上，结构体大小为 8 字节，`level` 成员的偏移为 4 字节。 |
| ARM32 与 ARM64 类型大小 | ARM64 编译器给出的 `int`、指针和 `long` 大小分别为 4、8、8 字节。常见 ARM32 Linux 下这三种类型各为 4 字节。 [Arm ABI](https://github.com/ARM-software/abi-aa) |

三份 C 练习的源码提交为 `7e04a64`。

## 本周排障记录

- SSH 连旧地址超时。串口查询发现 K11C 的 DHCP 地址已经改变，改用当时实际分配的地址后恢复连接。
- K11C 的 APT 索引刷新遇到 `bullseye-backports` 的 404。注释失效和重复的 backports 条目，保留中科大镜像上的原有 Bullseye 主仓库配置。更新索引后从 `bullseye/main` 成功安装 GPIO 工具。
- 首次传 ARM64 程序时，把 `scp` 错跑在开发板上，终端找不到 Ubuntu 的私钥。改从 Ubuntu 执行 `scp` 后，文件上传成功并在 K11C 上正常运行。
