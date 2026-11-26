/* Author: Beniot Phan (Phan Bao Tin) */
/* Linker Script for STM32F407VET6 (512KB Flash, 128KB SRAM, 64KB CCMRAM) */

ENTRY(Reset_Handler)

MEMORY
{
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  CCMRAM (rw)  : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Đỉnh Stack nằm ở cuối 128KB SRAM (Full-Descending Stack) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Kiểm tra tĩnh dung lượng tối thiểu dành cho Main Stack (4KB) và Heap (0B) */
_Min_Heap_Size  = 0x0;
_Min_Stack_Size = 0x1000;

SECTIONS
{
  /* 1. Bảng Vector ngắt đặt ngay đầu Flash 0x08000000 */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Bắt buộc KEEP để --gc-sections không xóa mất bảng vector */
    . = ALIGN(4);
  } >FLASH

  /* 2. Mã lệnh chương trình (.text) và hằng số chỉ đọc (.rodata) */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  /* Địa chỉ nạp (LMA) của vùng .data trong Flash */
  _sidata = LOADADDR(.data);

  /* 3. Biến toàn cục/static có giá trị khởi tạo khác 0 (VMA ở RAM, LMA ở Flash) */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  /* 4. Biến toàn cục/static khởi tạo bằng 0 (chỉ chiếm chỗ RAM, không tốn Flash) */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  /* 5. Vùng RAM không xóa khi Soft/Watchdog Reset (lưu Crash Log HardFault) */
  .noinit (NOLOAD) :
  {
    . = ALIGN(4);
    _snoinit = .;
    *(.noinit)
    *(.noinit*)
    . = ALIGN(4);
    _enoinit = .;
  } >RAM

  /* 6. Core Coupled Memory (CPU-only, cấm cấp phát buffer DMA ở đây) */
  .ccmram (NOLOAD) :
  {
    . = ALIGN(4);
    _sccmram = .;
    *(.ccmram)
    *(.ccmram*)
    . = ALIGN(4);
    _eccmram = .;
  } >CCMRAM

  /* Kiểm tra tràn RAM ngay lúc link */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM
}