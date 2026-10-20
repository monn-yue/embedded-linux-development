# W2 网络故障排查记录

## 1. 开发环境

- 开发主机：VMware Ubuntu 24.04，x86_64
- 开发板：KICKPI K11C V1.2（RK3566）
- 开发板系统：Debian 11，Linux 5.10.160，ARM64
- 开发板用户：linaro（UID/GID 1000）
- 网络连接：Windows Wi-Fi 共享至物理以太网，K11C 通过网线连接 Windows
- 开发板 IP：192.168.137.52/24
- 默认网关：192.168.137.1

## 2. 故障一：DHCP 地址获取失败

### 2.1 故障现象

K11C 无法正常通过 SSH 登录。

排查过程中观察到：

- 曾经 Ping 192.168.137.52 有响应，但 SSH 连接被远程关闭。
- 后续通过串口检查，发现 eth0 没有 IPv4 地址。
- `ip route` 没有默认路由。
- DHCP 客户端无法成功获取地址。

以上现象来自不同检查阶段，不认定它们全部同时发生。

### 2.2 排查过程

1. 通过 CH340 USB-TTL 串口登录 K11C，确认系统正常运行。
2. 执行 `systemctl status ssh`，确认 SSH 服务正常。
3. 执行 `ss -lntp`，确认 TCP 22 端口正在监听。
4. 执行 `ip -br link`，确认 eth0 为 UP、LOWER_UP。
5. 执行 `ip -4 -br addr`，发现 eth0 没有 IPv4 地址。
6. 执行 `dhclient -1 -v eth0`，观察到反复发送 DHCPDISCOVER，但没有收到 DHCPOFFER。
7. 在 Windows 检查 SharedAccess 服务，最初状态为 Stopped。
8. 启动 SharedAccess 后，DHCP 仍未恢复。
9. 关闭并重新开启 Windows WLAN 网络共享。
10. 再次执行 dhclient，成功收到 DHCPOFFER、DHCPREQUEST 和 DHCPACK。

### 2.3 故障恢复

重新获取网络配置：

- IPv4：192.168.137.52/24
- 默认网关：192.168.137.1

VMware Ubuntu 重新通过 SSH 登录 K11C 成功。

### 2.4 原因分析

直接故障发生在 DHCP 地址获取阶段，开发板没有成功收到 DHCP 服务器的地址分配响应。

Windows ICS 网络共享功能异常是最可能的原因。重新初始化 WLAN 网络共享后，DHCP 恢复正常。

但尚未定位 Windows 内部的具体故障组件，不能确认是 SharedAccess 服务本身导致。

### 2.5 经验总结

- 物理网口 UP、LOWER_UP 不代表已经获得 IPv4 地址。
- SSH 连接异常时，应分别检查网络地址、路由、SSH 服务和端口监听。
- DHCPDISCOVER 持续发送但没有 DHCPOFFER，应检查 DHCP 服务端及其所在网络。
- Windows ICS 共享异常可能影响开发板获取 IPv4 地址。

## 3. 故障二：普通用户 ping 权限异常

### 3.1 故障现象

在 K11C Debian 11 中，普通用户执行：

`ping -c 3 192.168.137.1`

出现错误：

`ping: socket: Operation not permitted`

但使用 sudo 执行 ping，能够正常访问网关和互联网。

### 3.2 排查过程

检查用户：

`id`

结果显示：

- UID：1000
- GID：1000

检查 ping 程序权限：

`ls -l "$(command -v ping)"`

结果：

`-rwxr-xr-x 1 root root 69080 /usr/bin/ping`

检查文件 Capability：

`getcap "$(command -v ping)"`

结果：无输出。

检查内核 ICMP 权限范围：

`cat /proc/sys/net/ipv4/ping_group_range`

结果：

`1 0`

这是空的用户组权限范围，普通用户无法通过该机制创建 ICMP Echo 套接字。

### 3.3 故障原因

ping 程序没有 setuid 权限，也没有配置额外的文件 Capability。

同时，内核 `ping_group_range` 被设置为 `1 0`，不允许普通用户组创建 ICMP Echo 套接字。

因此普通用户执行 ping 时出现权限错误。

### 3.4 临时修复

执行：

`sudo sysctl -w 'net.ipv4.ping_group_range=1000 1000'`

将允许创建 ICMP Echo 套接字的用户组范围设置为 GID 1000。

此修改在当前运行环境中生效，未设置永久生效配置。

### 3.5 修复验证

**网关测试**

普通用户执行：

`ping -c 3 192.168.137.1`

结果：

- 发送：3
- 接收：3
- 丢包率：0%
- 平均延迟：1.166 ms

**外网测试**

普通用户执行：

`ping -c 3 1.1.1.1`

结果：

- 发送：3
- 接收：3
- 丢包率：0%
- 平均延迟：66.703 ms

**DNS 测试**

执行：

`getent ahostsv4 example.com`

成功解析域名，返回 IPv4 地址。

### 3.6 经验总结

- ping 报错不一定表示网络不可达。
- `Operation not permitted` 应优先检查权限及安全限制。
- Linux Capability 和 `ping_group_range` 都可能影响 ICMP 权限。
- 使用 sudo 与普通用户进行对照测试，可以帮助判断权限问题。
- 修复后应重新测试，使用实际结果证明问题已经解决。

## 4. W2 网络排障阶段总结

本次共记录两次真实故障：

1. DHCP 地址获取失败：重新初始化 Windows 网络共享后恢复，具体内部原因尚未确定。
2. 普通用户 ping 权限异常：通过调整 ICMP 用户组权限范围完成临时修复，并验证成功。

已完成 IP 地址、默认网关、DNS 解析、局域网和外网连通性验证。

当前剩余事项：

- DHCP 是否再次出现异常，待后续观察。
- ping 权限目前仅做临时修改，未配置永久生效。
- 继续完成 W2 中 Wi-Fi 状态、SSH 密钥、scp 等任务。

本次记录基于真实终端输出与操作过程，不包含人为制造的故障。
