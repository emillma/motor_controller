cmake_minimum_required(VERSION 3.12)

include(/include/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

project(pps_sim)
# add_compile_definitions(PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE=1200)


add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})
