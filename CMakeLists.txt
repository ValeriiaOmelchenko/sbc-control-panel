cmake_minimum_required(VERSION 3.10)
project(sbc_control_panel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(includes)

# Include ADS1015 library 
include_directories(libs/Adafruit_ADS1X15_RPi)

add_subdirectory(libs/Adafruit_ADS1X15_RPi)

# Include sources
file(GLOB SOURCES "src/*.cpp")

# Output binary
add_executable(control_panel ${SOURCES})


find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
include_directories(${ZMQ_INCLUDE_DIRS})
target_link_libraries(control_panel ${ZMQ_LIBRARIES})

target_link_libraries(control_panel
    Adafruit_ADS1015
    wiringPi
)