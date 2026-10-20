
# Embedded Linux Internship Preparation

## Project Goal

Prepare for an Embedded Linux internship through
a 12-week hands-on engineering project.

Main topics:
- Linux system administration and networking
- C/C++17 programming
- Git, GDB and CMake
- GPIO and I2C communication
- ARM64 cross-compilation
- IMU data acquisition and automated testing

## Hardware

- Board: KICKPI K11C V1.2 (RK3566)
- RAM: 4GB
- eMMC: 32GB
- IMU: ICM-45688-P + QMC6309

## Development Environment

- Host: VMware Ubuntu 24.04 (x86_64)
- Target: Debian 11 (ARM64)
- Kernel: Linux 5.10.160

## Weekly Progress

### W1 - System Setup [Completed]

- Verified Debian 11 and board hardware
- Established serial console and SSH access
- Prepared official firmware and recovery tools
- Initialized Git repository

Documentation:
[Flashing and Login](docs/flashing-and-login.md)

### W2 - Networking and Debugging [Completed]

- Verified IP, gateway, DNS and network interfaces
- Diagnosed DHCP and ping permission issues
- Configured SSH public-key authentication
- Verified bidirectional scp file transfers
- Inspected systemd services and journal logs
- Practiced C pointers, functions and GCC
- Debugged C programs using GDB

Documentation:
- [W2 Summary](docs/w2-summary.md)
- [Network Troubleshooting](docs/w2-network-troubleshooting.md)

Source Code:
- [W2 C Practice](src/w2/main.c)

## Current Status

W1 and W2 completed and verified.

Next milestone: W3 - GPIO, libgpiod,
device tree basics and ARM64 cross-compilation.
