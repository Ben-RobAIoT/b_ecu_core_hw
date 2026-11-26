# Author: Beniot Phan (Phan Bao Tin)
# Build configuration for ECU-A (STM32F407VET6 Bare-Metal)

cmake_minimum_required(VERSION 3.20)

# Nạp toolchain file trước lệnh project()
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi-gcc.cmake")

project(ecu_a_f407 C ASM)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker/stm32f407vet6.ld")

add_executable(${PROJECT_NAME}.elf
    startup/startup_stm32f407vetx.s
    src/main.c
    src/bsp_gpio.c
    src/bsp_uart_dwt.c
    src/fault_handler.c
    src/fault_experiments.c  
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/proto
)

# Bộ cờ biên dịch nghiêm ngặt chuẩn Automotive / MISRA-style
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -std=c11
    -Og
    -g3
    -Wall
    -Wextra
    -Wconversion
    -Wshadow
    -fstack-usage
    -ffunction-sections
    -fdata-sections
)

# Bộ cờ Linker: loại bỏ code chết (--gc-sections), xuất app.map, dùng newlib-nano
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/app.map
    -Wl,--print-memory-usage
    --specs=nano.specs
    --specs=nosys.specs
)

# Tự động xuất file .bin, .hex và thống kê kích thước phân vùng sau mỗi lần build
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .bin, .hex and checking Flash/RAM usage..."
)