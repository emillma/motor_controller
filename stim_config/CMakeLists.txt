cmake_minimum_required(VERSION 3.13)


include(/include/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

project(my_project)

add_executable(stim_configurator 
                "src/main.cpp")

pico_generate_pio_header(stim_configurator ${CMAKE_CURRENT_LIST_DIR}/pio/invert.pio)

add_compile_definitions(PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE=1200)

target_link_libraries(stim_configurator 
        pico_stdlib 
        hardware_pio 
         )
        
pico_enable_stdio_usb(stim_configurator 1)
pico_enable_stdio_uart(stim_configurator 0)
pico_add_extra_outputs(stim_configurator)




 