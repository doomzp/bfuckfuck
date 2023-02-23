#include "assembly.hpp"

namespace Asm {
    typedef struct Label {
        const std::string temp;
        std::string body;
    } Label;

    FILE* outf;
    Label* culabel;
    std::vector<Asm::Label> labels;
}

void asm_init (const std::string &asmname, const std::string &bytes) {
    Asm::outf = fopen(asmname.c_str(), "w");

    fprintf(Asm::outf, ".globl mems\n");
    fprintf(Asm::outf, ".bss\n");
    fprintf(Asm::outf, "\t.align 1\n");
    fprintf(Asm::outf, "\t.type mems, @object\n");
    fprintf(Asm::outf, "\t.size mems, %s\n", bytes.c_str());

    fprintf(Asm::outf, "mems:\n");
    fprintf(Asm::outf, "\t.zero %s\n", bytes.c_str());

    fprintf(Asm::outf, ".globl main\n");
    fprintf(Asm::outf, ".type main, @function\n"); 
    fprintf(Asm::outf, ".text\n");

    Asm::labels.push_back(Asm::Label {
        .temp = "main:\n"        
                "\tpushq \t %rbp\n"
                "\tmovq \t %rsp, %rbp\n"
                "\tsubq \t $16, %rsp\n"
                "\tleaq \t mems(%rip), %rax\n"    /* At beginning of the program the pointer will point to the first memory. */
                "\tmovq \t %rax, -8(%rbp)\n"      /* The pointer is saved at -8 stack position. */
                "%s"
                "\tmovl \t $0, %%eax\n"
                "\tleave\n"
                "\tret\n"
    });
    Asm::culabel = &Asm::labels.front();
}

void asm_incdec (unsigned times, char kase) {
    /* To be able to perform this operation the assembly code
     * needs to pass the mem address of the current mem into a register
     * of 64 bits, in this case %rax, so:
     *                         movq     -8(%rbp), %rax.
     * 
     * Then %rax is holding a mem address, but the program needs the varibale,
     * not its mem address, so the program needs to store the byte into some
     * register of 8 bits, in this case %al.
     *                         movb     (%rax), %al.
     *                                     \
     *                                    When parenthese are used means the value
     *                                    that contains that mem address.
     *
     * Now it's time to modify the value to the current memory (bf term.).
     *                         addb     $x, %al.
     *                         subb
     *
     * Once the value has been modifed, the new value is gonna be saved into
     * another register, since al and its "family" (rax, eax, ax & al [in this case])
     * 'cause when you change one of those registers all of them also change their value.
     *                         movb     %al, %bl          : Set the value of %al to %bl.
     *                         movq     -8(%rbp), %rax    : Will save the current mem address into rax again.
     * 
     * And finally set the new value into the mem address.
     *                         movb     %bl, (%rax).
     * */
    const std::string duz = (kase == '+') ? "\taddb" : "\tsubb";
    Asm::culabel->body += "\tmovq \t -8(%rbp), %rax\n"
                          "\tmovb \t (%rax), %al\n" +
                          duz + " \t $" + std::to_string(times) + ", %al\n"
                          "\tmovb \t %al, %bl\n"
                          "\tmovq \t -8(%rbp), %rax\n"
                          "\tmovb \t %bl, (%rax)\n";
}

void asm_print (unsigned times) {
    /* %edi register is used as first argument, but how this
     * function (putchar) prints a char, the program doesn't
     * need a 32 bit register, so %dil is used since it's the
     * 8 bit version of %edi.
     * */
    Asm::culabel->body += "\tmovq \t -8(%rbp), %rax\n"
                          "\tmovb \t (%rax), %dil\n"
                          "\tcall \t putchar@PLT\n";
    if ( --times ) { asm_print(times); }
}

void asm_input (unsigned times) {
    Asm::culabel->body += "\tcall \t getchar@PLT\n"
                          "\tmovl \t %eax, %edx\n"
                          "\tmovq \t -8(%rbp), %rax\n"
                          "\tmovb \t %dl, (%rax)\n";
    if ( --times ) { asm_print(times); }
}

void asm_moveThrough (unsigned times, char kase) {
    const std::string duz = (kase == '>') ? "\taddq" : "\tsubq";
    Asm::culabel->body += duz + " \t $" + std::to_string(times) + ", -8(%rbp)\n";
}

void asm_startLoop (char kase) {
    static unsigned nlabel = 1;
    const std::string LC = "LC" + std::to_string(nlabel);
    const std::string LB = "LB" + std::to_string(nlabel);

    /* This is explain in the readme file. */
    std::string CMP = "\tcmpb \t $0, %%al\n";
    std::string JMP = "\tjne  \t ";
    if ( kase == '(' ) {
        CMP = "\tcmpb \t $31, %%al\n";
        JMP = "\tjg   \t ";
    }

    Asm::culabel->body += "\tcall \t " + LC + "\n";
    Asm::labels.push_back(Asm::Label {
        .temp = LC + ":\n"
                "\tmovq \t -8(%rbp), %rax\n"
                "\tmovb \t (%rax), %%al\n" +
                CMP +
                JMP + LB + "\n"
                "\tret\n" +
                LB + ":\n"
                "%s"
                "\tjmp  \t " + LC + "\n"
                "\tret\n"
    });
    Asm::culabel = &Asm::labels.back();
    nlabel++;
}

void asm_endLoop () {
    fprintf(Asm::outf, Asm::culabel->temp.c_str(), Asm::culabel->body.c_str());
    Asm::culabel = &Asm::labels.rbegin()[1];
    Asm::labels.pop_back();
}

void asm_write () {
    fprintf(Asm::outf, Asm::culabel->temp.c_str(), Asm::culabel->body.c_str());
    fclose(Asm::outf);
}
