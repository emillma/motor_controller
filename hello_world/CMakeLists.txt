cmake_minimum_required(VERSION 3.13)


include(/include/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

project(my_project)


add_executable(my_program 
                "src/main.cpp")

pico_generate_pio_header(my_program ${CMAKE_CURRENT_LIST_DIR}/pio/invert.pio)

add_compile_definitions(PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE=1200)

target_link_libraries(my_program 
        pico_stdlib 
        hardware_pwm
        pico_unique_id 
        hardware_watchdog 
        hardware_adc 
        hardware_i2c
        hardware_pio 
        hardware_clocks 
        hardware_spi 
        pico_multicore
        hardware_dma 
        hardware_irq )
        
pico_enable_stdio_usb(my_program 1)
pico_enable_stdio_uart(my_program 0)
pico_add_extra_outputs(my_program)




 