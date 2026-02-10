# Versión mínima de CMake requerida por Zephyr
cmake_minimum_required(VERSION 3.20.0)

# Export the compile commands for the project
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


string(REPLACE "/" "_" BOARD_CLEAN ${BOARD})
message(STATUS "--------------------------------------------")
message(STATUS "COMPILING FOR BOARD: ${BOARD_CLEAN}")
message(STATUS "ARCHITECTURE DETECTED:   ${ARCH}")
message(STATUS "--------------------------------------------")

# USE ZEPHYR
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# PROJECT NAME
project(esp32_logs)



# ADD YOUR SOURCE FILE
target_sources(app PRIVATE main.c)
file(GLOB APP_SOURCES "src/*.c")
target_sources(app PRIVATE ${APP_SOURCES})
file(GLOB APP_SOURCES "src/*.cpp")
target_sources(app PRIVATE ${APP_SOURCES})

# Compile all drivers for the board too
message(STATUS "Compiling drivers from folder: drivers/${BOARD_CLEAN}")
file(GLOB BOARD_SOURCES "drivers/${BOARD_CLEAN}/*.c")
target_sources(app PRIVATE ${BOARD_SOURCES})

# Display the list of sources
message(STATUS "List of sources: ${BOARD_SOURCES}")

