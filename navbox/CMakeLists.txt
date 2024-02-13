cmake_minimum_required(VERSION 3.13)


include(/include/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

project(my_project)


add_executable(navbox 
                "src/main.cpp")
pico_generate_pio_header(navbox ${CMAKE_CURRENT_LIST_DIR}/pio/invert.pio)
pico_generate_pio_header(navbox ${CMAKE_CURRENT_LIST_DIR}/pio/trigger.pio)
pico_generate_pio_header(navbox ${CMAKE_CURRENT_LIST_DIR}/pio/uart.pio)

add_compile_definitions(PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE=1200)

target_link_libraries(navbox 
        pico_stdlib 
        hardware_pwm
        pico_unique_id 
        hardware_watchdog 
        hardware_adc 
        hardware_pio 
        hardware_clocks 
        hardware_spi 
        hardware_i2c 
        pico_multicore
        hardware_dma 
        hardware_irq )
        
pico_enable_stdio_usb(navbox 1)
pico_enable_stdio_uart(navbox 0)
pico_add_extra_outputs(navbox)




 