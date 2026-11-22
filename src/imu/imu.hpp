#pragma once

#include <array>
#include <cstdint>
#include <string>

struct ImuSample {
    // Raw signed readings in X, Y, Z order.
    std::array<std::int16_t, 3> acceleration{};
    std::array<std::int16_t, 3> angular_velocity{};
};

class Imu {
public:
    explicit Imu(const std::string& device_path = "/dev/i2c-3",
                 std::uint8_t address = 0x69);
    ~Imu();

    Imu(const Imu&) = delete;
    Imu& operator=(const Imu&) = delete;

    ImuSample read_sample() const;

private:
    int fd_ = -1;
    std::uint8_t address_;
};
