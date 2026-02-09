# Versión mínima de CMake requerida por Zephyr
cmake_minimum_required(VERSION 3.20.0)

message(STATUS "--------------------------------------------")
message(STATUS "COMPILING FOR BOARD: ${BOARD}")
message(STATUS "ARCHITECTURE DETECTED:   ${ARCH}")
message(STATUS "--------------------------------------------")

# USE ZEPHYR
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# PROJECT NAME
project(esp32_logs)



# ADD YOUR SOURCE FILE
target_sources(app PRIVATE main.c)

# Compile all drivers for the board too
file(GLOB BOARD_SOURCES "drivers/${BOARD}/*.c")
target_sources(app PRIVATE ${BOARD_SOURCES})