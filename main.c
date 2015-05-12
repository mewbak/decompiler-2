#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <elf.h>

#define EXEC "execer"
#define APPNAME "Execer"
#define VERSION "0.1"

static int verbose_flag;

static struct option long_options[] = {
    { "verbose", no_argument,       &verbose_flag,  1  },
    { "elf",     required_argument, 0,             'e' },
    { "help",    no_argument,       0,             'h' },
    {  0, 	     0, 		0, 		0  }
};

void print_help() {
    printf("%s v%s\n\n", APPNAME, VERSION);
    printf("Usage:   %s [options]\n\n", EXEC);
    printf("Options: --elf,  -e [filename]: Reads an ELF file\n");
    printf("	 --help, -h: 	        Prints this message\n\n");
    printf("What is this?\n\n");
    printf(" This program is an attempt to understand the ELF format\n");
    printf(" and how it works.\n");
}

void process_args(int c, int option_index) {
    switch(c) {
        case 0:
            break;
        case 'e':
            printf("Reading ELF file \"%s\"\n", optarg);

            // Launch the tests ELFs.
            parse_binary(optarg);
            break;
        case 'h':
            default:
            print_help();
        break;
    }
}

int main(int argc, char **argv) {
    int option_index = 0;

    int c = getopt_long(argc, argv, "abc:e:p:s:h:", long_options, &option_index);
    process_args(c, option_index);
}
