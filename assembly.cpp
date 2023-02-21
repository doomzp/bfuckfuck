#include "assembly.hpp"

namespace Asm {
    typedef struct Label {
        const std::string temp;
        std::string body;
    } Label;

    FILE* outf;
    Label* culabel;
    std::vector<Asm::Label> labels;
    unsigned cupos = 0;
}

void asm_init (const std::string &asmname, bool input) {
    Asm::outf = fopen(asmname.c_str(), "w");

    /* Defines an global array of 30000 bytes, all chars
     * are set in 0.
     * */
    fprintf(Asm::outf, ".globl mems\n");
    fprintf(Asm::outf, ".type mems, @object\n");

    /* Defines the main funcion. */
    fprintf(Asm::outf, ".globl main\n");
    fprintf(Asm::outf, ".type main, @main\n\n");

    /* bss section will reserve memory for the array (mems)
     * and will set all those bytes to 0. */
    fprintf(Asm::outf, ".bss\n");
    fprintf(Asm::outf, "mems:\n");
    fprintf(Asm::outf, "\t.zero  30000\n");
    fprintf(Asm::outf, "\t.align 1\n");
    fprintf(Asm::outf, "\t.size  mems, 30000\n");

    if ( input ) {
        fprintf(Asm::outf, ".data\n");
        fprintf(Asm::outf, "getin:\n");
        fprintf(Asm::outf, "\t.string \" %%c\"\n");
    }

    /* The text section is basically all the code. */
    fprintf(Asm::outf, ".text\n");
    Asm::labels.push_back(Asm::Label {
        .temp = "main:\n"        
                "\tpushq     %rbp\n"
                "\tmovq      %rsp, %rbp\n"
                "\tsubq      $15, %rbp\n"
                "\tleaq      mems(%rip), %rax\n"
                "\tmovq      %rax, -8(%rbp)\n"
                "%s"
                "\tleave\n"
                "\tret\n"
    });
    Asm::culabel = &Asm::labels.front();
}