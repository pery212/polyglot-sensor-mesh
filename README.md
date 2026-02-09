# polyglot-sensor-mesh
Test of a project with several boards and several languajes

## Project List

- [ ] stm32_uart_c	STM32F103	C (Bare Metal)	Sensor (Proactive) → UART
- [ ] stm32_uart_rust	STM32F103	Rust (Embedded)	Sensor (Proactive) → UART
- [ ] esp32_i2c_cpp	ESP32	C++ (ESP-IDF)	Sensor (Slave) → I2C
- [ ] esp32_ble_rust	ESP32	Rust (ESP-IDF)	Sensor (Server) → BLE
- [ ] esp32_gateway_zephyr_cpp	ESP32	C++ (Zephyr)	Gateway: The brain that unites everything
- [ ] rpi_rust	Raspberry Pi 1	Rust (Linux)	Bridge: Sockets → MQTT
- [ ] debian_rust	PC Debian	Rust (Desktop)	Backend: MQTT → Logic of data.


## Zephyr configuration
    west init
    west update

    Create a folder for your code with
    prj.conf
    CMakeLists.txt
    main.c

    create a virtual environment and install the requirements
    python3 -m venv .venv
    source .venv/bin/activate

    Refresh terminal and make source .venv/bin/activate again
    west packages pip --install

    To compile
    rm -rf build/ &&  west build -b esp32_devkitc/esp32/procpu
    clear && rm -rf build && west build -b esp32_devkitc/esp32/procpu


    To flash
    west flash

    Logs
    picocom -b 115200 /dev/ttyUSB0 

    Compile for linux
    west build -b native_sim .
    clear && rm -rf build && west build -b native_sim .

    Run for linux
    ./build/zephyr/zephyr.exe
    