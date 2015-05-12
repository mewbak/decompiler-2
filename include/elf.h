#ifndef _EXECER_ELF_H
#define _EXECER_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Define ELF macros ---------------------------------

#define PF_PRES     0x1 // Is present.
#define PF_W        0x2 // Is writeable.
#define PF_USER     0x4 // Is user-mode.
#define SHN_UNDEF	(0x00)

// ELF generic macros --------------------------------

// ELF magic numbers; needed to identify the validity
// of an ELF executable.
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3

#define ELF_MAG0 127 // 0x7f
#define ELF_MAG1 69  // E
#define ELF_MAG2 76  // L
#define ELF_MAG3 70  // F

// Other macros: version, data, class, etc.
#define ELF_CLASS      4
#define ELF_DATA       5
#define ELF_VERSION    6
#define ELF_OSABI      7
#define ELF_ABIVERSION 8
#define ELF_PAD        9
#define ELF_NIDENT     16

#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ELFOSABI_SYSV       0
#define ELFOSABI_HPUX 	    1
#define ELFOSABI_STANDALONE 255

// Machine type; to be populated.
//  More info:
//    http://www.sco.com/developers/gabi/2000-07-17/ch4.eheader.htmls
#define EM_NONE  0  // None
#define EM_M32   1  // AT&T WE 32100
#define EM_SPARC 2  // Sparc architecture
#define EM_386   3  // x86 architecture
#define EM_68K	 4  // Motorola 68000
#define EM_88K   5  // Motorola 88000
#define EM_860   7  // Intel 80860
#define EM_MIPS  8  // MIPS architecture
#define EM_ARM   40 // ARM architecture
#define EM_AMD64 62 // x86_64/x64

// Type of ELF file: Executable, library, etc.
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4

// ELF64 format ---------------------------------------

// 64-bit ELF header
struct _elf_ehdr {
    uint8_t  e_ident[ELF_NIDENT];	// ELF identification
    uint16_t e_type;	// object file type
    uint16_t e_machine;	// machine type
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;   // Program headers offset in bytes.
    uint64_t e_shoff;   // Section headers offset in bytes.
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx; // The section that contains all the section names.
} __attribute__((packed));

// 64-bit ELF section header
struct _elf_shdr {
    uint32_t sh_name; // Contains the offset, in bytes, to the section name, relative to the
                      // start of the section name string table.
    uint32_t sh_type; // This member categorizes the section’s contents and semantics. Section types and their
                      // descriptions appear below.
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} __attribute__((packed));

// ELF section header macros  --------------------------------

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

#define SHF_WRITE     0x01
#define SHF_ALLOC     0x02
#define SHF_EXECINSTR 0x04

// 64-bit ELF program header
struct _elf_phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

struct _elf_sym {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t st_shndx;
};

struct _dyna {
    int number;
    char name[10];
    int age;
};

// ELF program header macros  --------------------------------

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

// Declaration of the used types -----------------------------

typedef struct _elf_ehdr elf_ehdr;
typedef struct _elf_phdr elf_phdr;
typedef struct _elf_shdr elf_shdr;
typedef struct _elf_sym elf_sym;

// Declaration of the public functions -----------------------

static FILE   *ELFFile;
static size_t  ELFSize;

void      parse_binary(char *);
char     *read_binary(char *);
elf_ehdr *get_ehdr(char *);
elf_phdr *get_phdr(char *, uint32_t);
size_t    get_binary_size(FILE **);
bool      is_elf();
void      load_program_headers(char *, elf_ehdr *, elf_phdr *, bool);
void      load_section_headers(char *, elf_ehdr *);
void      load_symbol_headers(char *, elf_ehdr *);
int       elf_get_symval(elf_ehdr *, int, int);
void      print_elf_type(elf_ehdr *);
void      print_tgt_arch(elf_ehdr *);
void     *getarea(long, unsigned long);

elf_shdr *elf_sheader(elf_ehdr *);
elf_shdr *elf_section(elf_ehdr *, int);
char *elf_str_table(elf_ehdr *);
char *elf_lookup_string(elf_ehdr *, int);

#endif
