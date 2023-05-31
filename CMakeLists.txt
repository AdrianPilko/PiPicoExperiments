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

add_executable(pio_blink)
pico_generate_pio_header(pio_blink ${CMAKE_CURRENT_LIST_DIR}/blink.pio)
target_sources(pio_blink PRIVATE blink_pio.c)



# Include the Pico SDK directories
target_include_directories(pico_hello_world PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_gpio/)
target_include_directories(pico_hello_world PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_uart/)
target_include_directories(dht PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_uart/)
target_include_directories(dht PRIVATE ${PICO_SDK_PATH}/src/rp2_common/pico_sdk_headers/hardware_gpio/)

target_include_directories(pio_blink PRIVATE ${PICO_SDK_PATH}/src/rp2_common/hardware_pio/include)
target_include_directories(pio_blink PRIVATE ${PICO_SDK_PATH}/src/rp2_common/hardware_dma/include)

# Link the Pico SDK libraries
target_link_libraries(pico_hello_world
    pico_stdlib
)

target_link_libraries(dht
    pico_stdlib
)


target_link_libraries(pio_blink PRIVATE pico_stdlib hardware_pio)

# Enable the Pico SDK's standard settings
pico_enable_stdio_usb(pico_hello_world 1)
pico_enable_stdio_uart(pico_hello_world 0)

pico_enable_stdio_usb(dht 1)
pico_enable_stdio_uart(dht 0)


pico_enable_stdio_usb(pio_blink 0)
pico_enable_stdio_uart(pio_blink 0)

# Create a map/bin/hex file from the built output
pico_add_extra_outputs(pico_hello_world)
pico_add_extra_outputs(dht)
pico_add_extra_outputs(pio_blink)
