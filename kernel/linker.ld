OUTPUT_ARCH("x86_64")
ENTRY(_start)

SECTIONS {
  /* Kernel load address */
  . = 0x100000;

  /* --- Code section --- */
  .text : ALIGN(0x1000) {
    *(.text.entry)
    *(.text .text.*)
    . = ALIGN(0x1000);

    /* trampoline page */
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline <= 0x1000, "trampoline to large");

    PROVIDE(etext = .);
  }

  /* --- Read-only data --- */
  .rodata : ALIGN(0x1000) {
    *(.rodata .rodata.*)
  }

  /* --- Initialized data --- */
  .data : ALIGN(0x1000) {
    *(.data .data.*)
  }

  /* --- Uninitialized data --- */
  .bss : ALIGN(0x1000) {
    *(.bss .bss.*)
    *(COMMON)
  }

  . = ALIGN(0x1000);
  PROVIDE(end = .);
}

