cmake_minimum_required(VERSION 3.13)


include(external/pico-sdk/pico_sdk_init.cmake)

project(my_project)
pico_sdk_init()
include_directories(include external)
file(GLOB SOURCES "src/*.c")
add_executable(my_program src/main.c)

# include_directories(ins_link pico-sdk)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
# pico_generate_pio_header(ins_link ${CMAKE_CURRENT_LIST_DIR}/pio/selecter.pio)

add_compile_definitions(PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE=1200)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=false)


# add_compile_definitions(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK=1)
# add_compile_definitions(PICO_STDIO_UART_SUPPORT_CHARS_AVAILABLE_CALLBACK=1)

target_link_libraries(my_program
        pico_stdlib
        pico_stdio_usb
        pico_stdio_uart
        pico_stdio
        pico_bootrom
        hardware_irq
        # pico_bootsel_via_double_reset
        # hardware_watchdog
        # hardware_spi
        # hardware_pio
        # pico_multicore
        )

pico_enable_stdio_usb(my_program 1)
pico_enable_stdio_uart(my_program 0)

pico_add_extra_outputs(my_program)


