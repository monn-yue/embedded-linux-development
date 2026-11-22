#include "imu.hpp"

#include <charconv>
#include <chrono>
#include <exception>
#include <iostream>
#include <string_view>
#include <system_error>
#include <thread>

int main(int argc, char* argv[]) {
    int sample_count = 1;

    const bool csv = argc > 1 && std::string_view(argv[argc - 1]) == "--csv";
    const int positional_count = argc - (csv ? 1 : 0);

    if (positional_count > 2) {
        std::cerr << "Usage: imu-read [sample-count] [--csv]\n";
        return 2;
    }

    if (positional_count == 2) {
        const std::string_view count_text = argv[1];
        const char* end = count_text.data() + count_text.size();
        const auto result =
            std::from_chars(count_text.data(), end, sample_count);

        if (result.ec != std::errc{} || result.ptr != end || sample_count < 1) {
            std::cerr << "Sample count must be a positive integer.\n";
            return 2;
        }
    }

    try {
        const Imu imu;
        if (csv) {
            std::cout << "elapsed_ms,accel_x_raw,accel_y_raw,accel_z_raw,"
                         "gyro_x_raw,gyro_y_raw,gyro_z_raw\n";
        }

        const auto start = std::chrono::steady_clock::now();
        for (int index = 0; index < sample_count; ++index) {
            const ImuSample sample = imu.read_sample();

            if (csv) {
                const auto elapsed = std::chrono::steady_clock::now() - start;
                const auto elapsed_ms =
                    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
                std::cout << elapsed_ms.count();
                for (const auto value : sample.acceleration) {
                    std::cout << ',' << value;
                }
                for (const auto value : sample.angular_velocity) {
                    std::cout << ',' << value;
                }
                std::cout << '\n';
            } else {
                std::cout << "accel_raw x=" << sample.acceleration[0]
                          << " y=" << sample.acceleration[1]
                          << " z=" << sample.acceleration[2] << '\n';

                std::cout << "gyro_raw  x=" << sample.angular_velocity[0]
                          << " y=" << sample.angular_velocity[1]
                          << " z=" << sample.angular_velocity[2] << '\n';
            }

            if (index + 1 < sample_count) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "IMU read failed: " << error.what() << '\n';
        return 1;
    }
}
