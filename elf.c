#include <elf.h>

/*
 * Converts any string into hexadecimal. To be used to convert
 * the binary in the .text field into hex.
 */
char *bin2hex(char *data) {
    // Use:
    // bin2hex("hello, world!");
    // bin2hex("hello");

    char const hex_chars[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };
    char string[strlen(data) * 2];
    int i, j;

    for(i = 0; i < strlen(data); ++i) {
        string[i] = 0;
    }

    for(i = 0, j = 0; i < strlen(data); ++i, j += 2) {
        char const byte = data[i];

        string[j] = hex_chars[(byte & 0xF0) >> 4];
        string[j + 1] = hex_chars[(byte & 0x0F) >> 0];
    }

    string[strlen(data) * 2] = '\0';

    char *result = string;
    //printf("%s\n", string);
    return result;
}

/*
 * Does all the work.
 */
void parse_binary(char *filename) {
    char *buffer = read_binary(filename);

    elf_ehdr *ehdr;
    elf_phdr *phdr;

    if(buffer != NULL) ehdr = get_ehdr(buffer);
    else printf("ERROR: Binary was empty (?)\n");

    // If not NULL, we have read the file.
    if(ehdr == NULL) return;

    // Check if we have a valid ELF file.
    if(!is_elf(ehdr)) return;

    // Print the type of the ELF file we're handling.
    print_elf_type(ehdr);

    // Print the target architecture.
    print_tgt_arch(ehdr);

    // TODO: In the future, load the headers based on the correct architecture.
    if(ehdr->e_machine != EM_AMD64) {
        printf("Can't read this file at this time.\n");
        return;
    }

    // Read the program headers of the ELF file.
    phdr = get_phdr(buffer, ehdr->e_phoff);

    printf("%i program headers found.\n", ehdr->e_phnum);

    load_program_headers(buffer, ehdr, phdr, false);
    load_section_headers(buffer, ehdr);
    load_symbol_headers(buffer, ehdr);

    return;
}

/*
 * Reads the binary file.
 */
char *read_binary(char *filename) {
    FILE *binary;
    size_t binary_size;

    // Open the file which should be an ELF binary and get it's
    // size in bytes.
    binary = fopen(filename, "r");
    ELFFile = binary;
    binary_size = get_binary_size(&binary);
    ELFSize = binary_size;

    printf("Binary size: %i bytes.\n", binary_size);

    // Allocate the needed space and append the binary contents
    // into a string.
    char *buffer = (char *)(malloc(binary_size + 1));
    int n = fread((void *)buffer, binary_size, 1, binary);

    if(n < 0) return NULL;
    else return buffer;
}

/*
 * Returns the size of the binary.
 */
size_t get_binary_size(FILE **binary) {
    fseek(binary[0], 0, SEEK_END);
    size_t binary_size = ftell(binary[0]);
    fseek(binary[0], 0, SEEK_SET);

    return binary_size;
}

/*
 * Returns the ELF's ehdr.
 */
elf_ehdr *get_ehdr(char *buffer) {
    return (elf_ehdr *) (buffer);
}

/*
 * Returns the ELF's phdr.
 */
elf_phdr *get_phdr(char *buffer, uint32_t e_poff) {
    return (elf_phdr *) ((uint32_t) buffer + e_poff);
}

/*
 * Loads the program headers of the file.
 */
void load_program_headers(char *buffer, elf_ehdr *ehdr, elf_phdr *phdr, bool execute) {
    int i, j;
    void **phs = malloc(ehdr->e_phnum);

    printf("\nProgram Headers\n\n");

    for(i = 0, j = 0; i < ehdr->e_phnum; i++, phdr++) {
        printf(" * %2u vaddr: @%08x ", (i + 1), phdr->p_vaddr);

        if(phdr->p_type == PT_LOAD) {
            if(execute) {
                phs[j] = (void *) ((uint32_t) phdr->p_vaddr);
                phs[j] = malloc((size_t) phdr->p_memsz);
                //memset((void *) ((uint32_t) phdr->p_vaddr), 0, (size_t) phdr->p_memsz);

                memcpy(&phs[j],
                    (const void *) ((uint32_t) buffer + (uint32_t) phdr->p_offset),
                    (size_t) phdr->p_filesz);
            }

            printf("PT_LOAD    ");

            j++;
        }
        else if(phdr->p_type == PT_DYNAMIC) printf("PT_DYNAMIC ");
        else if(phdr->p_type == PT_NOTE)    printf("PT_NOTE    ");
        else if(phdr->p_type == PT_SHLIB)   printf("PT_SHLIB   ");
        else if(phdr->p_type == PT_INTERP)  printf("PT_INTERP  ");
        else if(phdr->p_type == PT_PHDR)    printf("PT_PHDR    ");
        else if(phdr->p_type == PT_NULL)    printf("PT_NULL    ");
        else if(phdr->p_type > PT_PHDR)     printf("GNU/OS?    ");
        else printf("%10u ", phdr->p_type);

        if(phdr->p_flags == PF_X) printf("EXEC");
        else if(phdr->p_flags == PF_R) printf("READ");
        else if(phdr->p_flags == PF_W) printf("WRITE");
        else printf("%i", phdr->p_flags);

        printf("\n");
    }

    if(execute) {
        //void *start = (void *) ((uint32_t) ehdr->e_entry);
        //int (*_start)(void) = (void*) &start;
        int (*_start)(void) = (void *) ((uint32_t) ehdr->e_entry);

        if(_start != NULL) {
            printf("Executing...\n");

            int ret = _start();

            printf("%i ret.\n", ret);
        }
        else printf("No program body to execute.\n");
    }
}

/*
 * Loads the section headers of the file.
 */
void load_section_headers(char *buffer, elf_ehdr *ehdr) {
    elf_shdr *eshdr = (elf_shdr *) (buffer + ehdr->e_shoff);
    elf_shdr *sh_strtab = &eshdr[ehdr->e_shstrndx];

    printf("\nSection Headers (@%i)\n\n", (ehdr->e_shstrndx + 1));

    const char *const sh_strtab_p = (void *) buffer + sh_strtab->sh_offset;

    //char* stringTable = (void *)buffer + (eshdr + ehdr->e_shstrndx)->sh_offset;
    int i;

    for(i = 0; i < ehdr->e_shnum; ++i) {
        elf_shdr *shdr = (elf_shdr *) (buffer + (ehdr->e_shoff + (ehdr->e_shentsize * i)));
        printf(" Section %2u: type: %2u ", (i + 1), eshdr[i].sh_type);

        // http://fxr.watson.org/fxr/source/sys/elf_common.h?im=10#L293
        if(eshdr[i].sh_name < ELFSize) {
            char *_name = (char *) (sh_strtab_p + shdr->sh_name);
            printf("%4d = \"%s\"",  eshdr[i].sh_name, (char *) _name);

            // When _name == ".text" print out the addres at which it resides.
            if(!strcmp(_name, ".text")) {
                printf(" [Entry point: 0x%x", eshdr[i].sh_addr);
                //printf(" | Entry Size: %i", eshdr[i].sh_entsize);
                printf(" | Size: %i]", eshdr[i].sh_size);

                // === Experiments =============================================
                elf_shdr *sc_strtab = &eshdr[i];
                const char *const sc_strtab_p = (void *) buffer + sc_strtab->sh_offset;
                char text[sc_strtab->sh_size];
                char hextext[sc_strtab->sh_size * 2];

                int i, j;
                printf("\n");
                for(i = 0, j = 0; i < sc_strtab->sh_size && j < sc_strtab->sh_size * 2; i++, j++) {
                    text[i] = sc_strtab_p[i];

                    char tmp[1];
                    tmp[0] = sc_strtab_p[i];

                    char *temp = bin2hex((char *)tmp);

                    hextext[j] = temp[0] ? temp[0] : '0';
                    hextext[j + 1] = (temp[1] >= 0 && temp[1] <= 126) ? temp[1] : '0';

                    printf("%c%c ", hextext[j], hextext[j + 1]);
                }

                char *hex = bin2hex((char *)text);

                printf("\nData in .text:\n  %s", hex);
                printf("\nData in .text:\n  %s", hextext);
                // === Experiments =============================================
            }

            // char* name = stringTable + eshdr->sh_name;
            // printf("\"%s\"\n",name);
            // eshdr++;
        }

        printf("\n");
    }
}

/*
 * Loads the symbols in the executable.
 */
void load_symbol_headers(char *buffer, elf_ehdr *ehdr) {
    elf_shdr *shdrs = (elf_shdr *) (buffer + ehdr->e_shoff);
    elf_shdr *sym_shdr;
    elf_sym  *syms;
    int       sym_count = 0;

    printf("\nSymbols\n\n");

    int i;
    for(i = 0; i < ehdr->e_shnum; ++i) {
        elf_shdr *shdr = (elf_shdr *) (buffer + (ehdr->e_shoff + (ehdr->e_shentsize * i)));

        if(shdrs[i].sh_type == SHT_SYMTAB && i >= ehdr->e_shnum - 2) {
            // TODO: Get sym_shdr here instead and use it in the other if
            //       to loop through all the strings.
            syms = (elf_sym *) (buffer + shdr->sh_offset);
            sym_shdr = &shdr[0];

            if(shdrs[i].sh_entsize > 0) sym_count = shdrs[i].sh_size / shdrs[i].sh_entsize;

            printf("sh_size:    %4u\n", shdrs[i].sh_size);
            printf("sh_entsize: %4u\n\n", shdrs[i].sh_entsize);
            printf("%3u symbols found.\n", sym_count);
        }

        if(shdrs[i].sh_type == SHT_STRTAB && i >= ehdr->e_shnum - 2) {
            // Save the symbol string table here.
            elf_shdr *sc_strtab = &shdrs[i];

            // Save the string that contains all the symbols here.
            const char *const sc_strtab_p = (void *) buffer + sc_strtab->sh_offset;
            printf("%s\n", sc_strtab_p); // This serves no purpose.

            // char c = sc_strtab_p[2];
            // char cn = 0;
            //
            // while(c != '\0') {
            //     printf("%c", c);
            //     c = sc_strtab_p[cn];
            //     cn++;
            // }

            // Make sure that we've parsed the symbols location.
            if(syms != NULL) {
                int j;
                for(j = 0; j < sym_count; ++j, ++syms) {
                    // Rotate the symbol table so we can parse all of them.
                    syms = (elf_sym *) (buffer + sym_shdr->sh_offset + (sym_shdr->sh_entsize * j));

                    // At position 0, sc_strtab_p contains a \0 which means that we don't want them.
                    if(syms->st_name > 0) {
                        char *name = (char *) sc_strtab_p + syms->st_name;

                        // If the name exists, print it.
                        if(name != NULL && strcmp(name, "")) {
                            printf("%4u: \"%s\"\n", syms->st_name, name);
                        }
                    }
                }
            }
        }
    }
}

/*
 * Gets a symbol. Thanks to the OSDev wiki article:
 *  - http://wiki.osdev.org/ELF_Tutorial
 */
int elf_get_symval(elf_ehdr *ehdr, int table, int idx) {
    if(table == SHN_UNDEF || idx == SHN_UNDEF) return 0;
    else {
        elf_shdr *symtab = elf_section(ehdr, table);

        if(idx >= symtab->sh_size) {
            printf("ERROR: Symbol index is out of range (%d:%u).\n", table, idx);
            return -1;
        }
        else {
            int symaddr = (int) ehdr + symtab->sh_offset;
            elf_sym *symbol = &((elf_sym *) symaddr)[idx];

            // Get the name of the symbol.
            elf_shdr *strtab = elf_section(ehdr, symtab->sh_link);
            const char *name = (const char *) ehdr + strtab->sh_offset + symbol->st_name;

            // extern void *elf_lookup_symbol(const char *name);
            printf("\"%s\"\n", name);

            if(symbol->st_shndx == SHN_UNDEF) {
                return -2;
            }
            else if(symbol->st_shndx == 0xfff1) {
                return symbol->st_value;
            }
            else {
                elf_shdr *target = elf_section(ehdr, symbol->st_shndx);
                return (int) ehdr + symbol->st_value + target->sh_offset;
            }
        }
    }
}

/*
 * Validates that we have an ELF file.
 */
bool is_elf(elf_ehdr *ehdr) {
    if(ehdr->e_ident[EI_MAG0] != ELF_MAG0 ||
       ehdr->e_ident[EI_MAG1] != ELF_MAG1 ||
       ehdr->e_ident[EI_MAG2] != ELF_MAG2 ||
       ehdr->e_ident[EI_MAG3] != ELF_MAG3) {
        printf("ERROR: This is not an ELF file!\n");
        return false;
    }

    return true;
}

/*
 * Print the type of ELF file that we have.
 */
void print_elf_type(elf_ehdr *ehdr) {
    uint16_t type = ehdr->e_type;

    printf("ELF type: ");

    switch(type) {
        case ET_NONE: printf("ET_NONE"); break;
        case ET_REL:  printf("Relocatable file."); break;
        case ET_EXEC: printf("Executable file."); break;
        case ET_DYN:  printf("Dynamically linked library."); break;
        case ET_CORE: printf("Kernel module(?)"); break;
        default:      printf("Unknown ELF type"); break;
    }

    printf("\n");
}

/*
 * Print the target architecture that ELF was compiled in.
 */
void print_tgt_arch(elf_ehdr *ehdr) {
    uint16_t arch = ehdr->e_machine;

    printf("ELF arch: ");

    switch(arch) {
        case ET_NONE: printf("ET_NONE"); break;
        case EM_M32: printf("AT&T WE 32100"); break;
        case EM_SPARC: printf("Sparc"); break;
        case EM_386: printf("x86 (32-bit)"); break;
        case EM_860: printf("Intel 80860"); break;
        case EM_MIPS: printf("MIPS"); break;
        case EM_ARM: printf("ARM"); break;
        case EM_AMD64: printf("x86_64 (64-bit)"); break;
        default: printf("Unknown target architecture."); break;
    }

    printf("\n");
}

/*
 * Returns a specified area in the binary.
 */
void *getarea(long offset, unsigned long size) {
    void *buf;

    if(fseek(ELFFile, offset, SEEK_SET))
        return NULL;

    if(!(buf = malloc(size))) {
        if(fread(buf, size, 1, ELFFile) != 1) {
            free(buf);
            return NULL;
        }
    }

    return buf;
}

// This is going to help a lot:
// 	http://wiki.osdev.org/ELF_Tutorial
//  http://wiki.osdev.org/ELF
elf_shdr *elf_sheader(elf_ehdr *hdr) {
    return (elf_shdr *)((void *)hdr + hdr->e_shoff);
}

elf_shdr *elf_section(elf_ehdr *hdr, int idx) {
    return &elf_sheader(hdr)[idx];
}

char *elf_str_table(elf_ehdr *hdr) {
    if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
    return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}

char *elf_lookup_string(elf_ehdr *hdr, int offset) {
    char *strtab = elf_str_table(hdr);

    if(strtab == NULL) return NULL;

    return strtab + offset;
}
