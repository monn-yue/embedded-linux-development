#include "imu.hpp"

#include <cerrno>
#include <chrono>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <system_error>
#include <thread>
#include <unistd.h>

namespace {

void transfer_i2c(int fd, i2c_msg* messages, int count,
                  const char* operation) {
    i2c_rdwr_ioctl_data transfer{messages, static_cast<std::uint32_t>(count)};
    const int completed = ::ioctl(fd, I2C_RDWR, &transfer);
    if (completed < 0) {
        const int transfer_error = errno;
        throw std::system_error(
            transfer_error, std::generic_category(), operation);
    }
    if (completed != count) {
        throw std::runtime_error(std::string(operation) + ": incomplete transfer");
    }
}

void read_registers(int fd, std::uint8_t address, std::uint8_t reg,
                    std::uint8_t* data, std::uint16_t length) {
    i2c_msg messages[] = {
        {address, 0, 1, &reg},
        {address, I2C_M_RD, length, data},
    };
    transfer_i2c(fd, messages, 2, "read IMU registers");
}

void write_register(int fd, std::uint8_t address, std::uint8_t reg,
                    std::uint8_t value) {
    std::uint8_t bytes[] = {reg, value};
    i2c_msg message{address, 0, sizeof(bytes), bytes};
    transfer_i2c(fd, &message, 1, "write IMU register");
}

std::int16_t decode_axis(const std::uint8_t* bytes) {
    std::int32_t value = bytes[0] | (std::int32_t{bytes[1]} << 8);
    if (value >= 0x8000) {
        value -= 0x10000;
    }
    return static_cast<std::int16_t>(value);
}

}  // namespace

Imu::Imu(const std::string& device_path, std::uint8_t address)
    : address_(address) {
    fd_ = ::open(device_path.c_str(), O_RDWR | O_CLOEXEC);
    if (fd_ < 0) {
        const int open_error = errno;
        throw std::system_error(
            open_error, std::generic_category(), "open " + device_path);
    }

    try {
        std::uint8_t chip_id = 0;
        read_registers(fd_, address_, 0x72, &chip_id, 1);
        if (chip_id != 0xe7) {
            throw std::runtime_error(
                "unexpected IMU WHO_AM_I: " + std::to_string(chip_id) +
                " (expected 231 / 0xe7)");
        }

        write_register(fd_, address_, 0x10, 0x0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::uint8_t power_mode = 0;
        read_registers(fd_, address_, 0x10, &power_mode, 1);
        if ((power_mode & 0x0f) != 0x0f) {
            throw std::runtime_error("IMU accel and gyro modes did not verify");
        }
    } catch (...) {
        ::close(fd_);
        throw;
    }
}

Imu::~Imu() {
    ::close(fd_);
}

ImuSample Imu::read_sample() const {
    std::uint8_t raw[12]{};
    read_registers(fd_, address_, 0x00, raw, sizeof(raw));

    return {
        {decode_axis(raw), decode_axis(raw + 2), decode_axis(raw + 4)},
        {decode_axis(raw + 6), decode_axis(raw + 8), decode_axis(raw + 10)},
    };
}
