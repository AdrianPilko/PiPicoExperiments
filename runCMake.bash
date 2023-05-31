#!/bin/bash

declare -x PICO_SDK_PATH=/home/linuxlite/src/pico-sdk/
cmake -B build
cd build
cmake --build .
echo "press BOOTSEL on pico, and momentary ground RUN pin"
sleep 3
#cmake --build . --target pico_hello_world -- -j 4
echo "release BOOTSEL on pico"
sudo cp ./pico_hello_world.uf2 /media/linuxlite/RPI-RP2/
