#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EI_NIDENT 16

// ELF magic number and identification
#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFMAG "\177ELF"

// ELF identification indices
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_OSABI   7
#define EI_PAD     8

// ELF classes
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

// ELF data encodings
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// ELF file types
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

// ELF machine types
#define EM_NONE    0
#define EM_M32     1
#define EM_SPARC   2
#define EM_386     3
#define EM_68K     4
#define EM_88K     5
#define EM_860     7
#define EM_MIPS    8
#define EM_S370    9
#define EM_MIPS_RS3_LE 10
#define EM_PARISC  15
#define EM_VPP500  17
#define EM_SPARC32PLUS 18
#define EM_960     19
#define EM_PPC     20
#define EM_PPC64   21
#define EM_S390    22
#define EM_V800    36
#define EM_FR20    37
#define EM_RH32    38
#define EM_RCE     39
#define EM_ARM     40
#define EM_ALPHA   41
#define EM_SH      42
#define EM_SPARCV9 43
#define EM_TRICORE 44
#define EM_ARC     45
#define EM_H8_300  46
#define EM_H8_300H 47
#define EM_H8S     48
#define EM_H8_500  49
#define EM_IA_64   50
#define EM_MIPS_X  51
#define EM_COLDFIRE 52
#define EM_68HC12  53
#define EM_MMA     54
#define EM_PCP     55
#define EM_NCPU    56
#define EM_NDR1    57
#define EM_STARCORE 58
#define EM_ME16    59
#define EM_ST100   60
#define EM_TINYJ   61
#define EM_X86_64  62
#define EM_AARCH64 183

// ELF versions
#define EV_NONE    0
#define EV_CURRENT 1

// Program header types
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7
#define PT_LOOS    0x60000000
#define PT_HIOS    0x6FFFFFFF
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7FFFFFFF

// Program header flags
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

// Section header types
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_HASH     5
#define SHT_DYNAMIC  6
#define SHT_NOTE     7
#define SHT_NOBITS   8
#define SHT_REL      9
#define SHT_SHLIB    10
#define SHT_DYNSYM   11
#define SHT_LOPROC   0x70000000
#define SHT_HIPROC   0x7FFFFFFF
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xFFFFFFFF

// Section header flags
#define SHF_WRITE     0x1
#define SHF_ALLOC     0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC  0xF0000000

typedef struct elf64_ehdr {
  unsigned char e_ident[EI_NIDENT];
  uint16_t      e_type;
  uint16_t      e_machine;
  uint32_t      e_version;
  uint64_t      e_entry;
  uint64_t      e_phoff;
  uint64_t      e_shoff;
  uint32_t      e_flags;
  uint16_t      e_ehsize;
  uint16_t      e_phentsize;
  uint16_t      e_phnum;
  uint16_t      e_shentsize;
  uint16_t      e_shnum;
  uint16_t      e_shstrndx;
} elf64_ehdr;

typedef struct elf64_phdr {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} elf64_phdr;

typedef struct elf64_shdr {
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
} elf64_shdr;

#ifdef __cplusplus
}
#endif

