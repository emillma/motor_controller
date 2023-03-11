cmake_minimum_required(VERSION 3.13)


include(external/pico-sdk/pico_sdk_init.cmake)

project(my_project)
pico_sdk_init()
include_directories(include external)
file(GLOB SOURCES "src/*.c")
add_executable(my_program ${SOURCES})

# include_directories(ins_link pico-sdk)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/selecter.pio)

target_link_libraries(my_program
        pico_stdlib
        # pico_bootrom
        # pico_bootsel_via_double_reset
        # hardware_watchdog
        # pico_stdio_usb
        # hardware_spi
        # hardware_pio
        # hardware_irq
        # pico_multicore
        )

pico_enable_stdio_usb(my_program 1)
pico_enable_stdio_uart(my_program 0)

pico_add_extra_outputs(my_program)


