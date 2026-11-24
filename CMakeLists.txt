cmake_minimum_required(VERSION 3.16)
project(embedded_linux_development LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(imu-read
    src/imu/main.cpp
    src/imu/imu.cpp
)

set_target_properties(imu-read PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_options(imu-read PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(imu-read PRIVATE Threads::Threads)
