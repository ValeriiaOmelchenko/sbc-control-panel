cmake_minimum_required(VERSION 3.10)
project(sbc_control_panel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sources
file(GLOB SOURCES "src/*.cpp")
file(GLOB ADAFRUIT_SOURCES "libs/Adafruit_ADS1X15_RPi/*.cpp")

# Output binary
add_executable(control_panel ${SOURCES} ${ADAFRUIT_SOURCES})

# Include headers
target_include_directories(control_panel PRIVATE
    ${PROJECT_SOURCE_DIR}/includes
    ${PROJECT_SOURCE_DIR}/libs/Adafruit_ADS1X15_RPi
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
target_include_directories(control_panel PRIVATE ${ZMQ_INCLUDE_DIRS})
target_link_libraries(control_panel PRIVATE ${ZMQ_LIBRARIES})

target_link_libraries(control_panel PRIVATE wiringPi pthread)
