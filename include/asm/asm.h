#ifndef _EXECER_ASM_H
#define _EXECER_ASM_H

char *instructions;
char *x64 = {
    // 0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
    { "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "",    "",    "OR",  "OR",  "OR",  "OR",  "OR",  "OR",  "",    "ESC" },
    { "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "",    "",    "SBB", "SBB", "SBB", "SBB", "SBB", "SBB", "",    ""    },
    { "AND", "AND", "AND", "AND", "AND", "AND", "ES:", "",    "SUB", "SUB", "SUB", "SUB", "SUB", "SUB", "CS:", ""    },
    { "XOR", "XOR", "XOR", "XOR", "XOR", "XOR", "SS:", "",    "CMP", "CMP", "CMP", "CMP", "CMP", "CMP", "DS:", ""    },
    { "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX", "REX" },
    { "PUSH", "PUSH", "PUSH", "PUSH", "PUSH", "PUSH", "PUSH", "PUSH", "POP", "POP", "POP", "POP", "POP", "POP", "POP", "POP" },
    { "", "", "MVEX", "MOVSXD", "FS:", "GS:", "op size", "addr size", "PUSH", "IMUL", "PUSH", "IMUL", "INS", "INS", "OUTS", "OUTS" },
    // JCC
    { "-O", "-NO", "-C", "-NC", "-E", "-NE", "-BE", "-A", "-S", "-NS", "-PE", "-PO", "-L", "-GE", "-LE", "-G" },
    { "ADDOR", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" }
};

#endif
