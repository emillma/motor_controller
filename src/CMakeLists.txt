cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)
 
project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

## SPI
add_executable(blinker)

# pico_generate_pio_header(spi_slave ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
pico_generate_pio_header(blinker ${CMAKE_CURRENT_LIST_DIR}/pio/encoder.pio)

include_directories(${PROJECT_SOURCE_DIR})

target_sources(blinker PUBLIC
        main.c
        # core1.c
        # trigger.pio
        # selecter.pio
        )

target_link_libraries(blinker PRIVATE 
        pico_stdlib 
        hardware_pwm 
        hardware_pio
        hardware_irq
        hardware_clocks
        # hardware_spi 
        pico_multicore)

pico_enable_stdio_usb(blinker 0)
pico_enable_stdio_uart(blinker 1)

pico_add_extra_outputs(blinker)  


