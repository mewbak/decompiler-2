#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>
#include <stdio.h>
#include <fcntl.h>

int print_shdr(const char *const fname, size_t size) {
    int fd = open(fname, O_RDONLY);
    char *p = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf32_Ehdr *ehdr = (Elf32_Ehdr*)p;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(p + ehdr->e_shoff);
    int shnum = ehdr->e_shnum;

    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    const char *const sh_strtab_p = p + sh_strtab->sh_offset;

    for (int i = 0; i < shnum; ++i) {
        printf("%2d: %4d '%s'\n", i, shdr[i].sh_name,
        sh_strtab_p + shdr[i].sh_name);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct stat st;
    const char *fname = "/proc/self/exe";

    if (argc > 1)
        fname = argv[1];

    if (stat(fname, &st) != 0) {
        perror("stat");
        return 1;
    }
    return print_shdr(fname, st.st_size);
}

/*
    $ gcc -g dump_shdr.c -m32 -std=c99
    $ ./a.out
    0:    0 ''
    1:   27 '.interp'
    2:   35 '.note.ABI-tag'
    3:   49 '.note.gnu.build-id'
    4:   72 '.hash'
    5:   68 '.gnu.hash'
    6:   78 '.dynsym'
    7:   86 '.dynstr'
    8:   94 '.gnu.version'
    9:  107 '.gnu.version_r'
    10:  122 '.rel.dyn'
    11:  131 '.rel.plt'
    12:  140 '.init'
    13:  135 '.plt'
    14:  146 '.text'
    15:  152 '.fini'
    16:  158 '.rodata'
    17:  166 '.eh_frame'
    18:  176 '.ctors'
    19:  183 '.dtors'
    20:  190 '.jcr'
    21:  195 '.dynamic'
    22:  204 '.got'
    23:  209 '.got.plt'
    24:  218 '.data'
    25:  224 '.bss'
    26:  229 '.comment'
    27:  238 '.debug_aranges'
    28:  253 '.debug_pubnames'
    29:  269 '.debug_info'
    30:  281 '.debug_abbrev'
    31:  295 '.debug_line'
    32:  307 '.debug_frame'
    33:  320 '.debug_str'
    34:  331 '.debug_loc'
    35:   17 '.shstrtab'
    36:    1 '.symtab'
    37:    9 '.strtab'
*/
