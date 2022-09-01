cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

## SPI
add_executable(spi_slave)

pico_generate_pio_header(spi_slave ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
pico_generate_pio_header(spi_slave ${CMAKE_CURRENT_LIST_DIR}/pio/selecter.pio)

include_directories(${PROJECT_SOURCE_DIR})

target_sources(spi_slave PRIVATE
        spi_slave.c
        core1.c
        trigger.pio
        selecter.pio
        )

target_link_libraries(spi_slave PRIVATE 
        pico_stdlib 
        hardware_spi 
        hardware_pio
        hardware_irq
        pico_multicore)

pico_enable_stdio_usb(spi_slave 1)
pico_enable_stdio_uart(spi_slave 0)

pico_add_extra_outputs(spi_slave)  


