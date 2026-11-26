# Author: Beniot Phan (Phan Bao Tin)
# Toolchain file for STM32F407VET6 (ARM Cortex-M4F)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size)

# Bỏ qua bước link thử nghiệm khi CMake kiểm tra compiler (tránh lỗi thiếu _exit)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Cờ kiến trúc phần cứng Cortex-M4 + FPU đơn chính xác (FPv4-SP-D16)
set(MCU_ARCH_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS_INIT   "${MCU_ARCH_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${MCU_ARCH_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_ARCH_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${MCU_ARCH_FLAGS}")