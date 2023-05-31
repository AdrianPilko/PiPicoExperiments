cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)

# Specify the path to the Pico SDK
set(PICO_SDK_PATH /home/linuxlite/src/pico-sdk/)

# Initialize the Pico SDK
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

pico_sdk_init()

project(pico_hello_world)

# Add your source file(s)
add_executable(pico_hello_world
    hello_world.c
)

add_executable(dht
    dht.c
)
# Include the Pico SDK directories
target_include_directories(pico_hello_world PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_gpio/)
target_include_directories(pico_hello_world PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_uart/)
target_include_directories(dht PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_uart/)
target_include_directories(dht PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_gpio/)

# Link the Pico SDK libraries
target_link_libraries(pico_hello_world
    pico_stdlib
)

target_link_libraries(dht
    pico_stdlib
)
# Enable the Pico SDK's standard settings
pico_enable_stdio_usb(pico_hello_world 1)
pico_enable_stdio_uart(pico_hello_world 0)

pico_enable_stdio_usb(dht 1)
pico_enable_stdio_uart(dht 0)

# Create a map/bin/hex file from the built output
pico_add_extra_outputs(pico_hello_world)
pico_add_extra_outputs(dht)
