cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(/include/pico-sdk/pico_sdk_init.cmake)


project(my_project)

pico_sdk_init()
# initialize the Raspberry Pi Pico SDK

add_executable(ins_link
               ins.c)

include_directories(ins_link /include/pico-sdk)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/selecter.pio)

target_link_libraries(ins_link
        pico_stdlib
        pico_bootrom
        pico_bootsel_via_double_reset
        hardware_watchdog
        # pico_stdio_usb
        # hardware_spi
        # hardware_pio
        # hardware_irq
        # pico_multicore
        )

pico_enable_stdio_usb(ins_link 1)
pico_enable_stdio_uart(ins_link 0)

pico_add_extra_outputs(ins_link)


