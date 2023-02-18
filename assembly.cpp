#include "assembly.hpp"

typedef struct Label {
    const std::string temp;
    std::string body;
} Label;

namespace Assembly {
    std::vector<Label> labels;
    FILE* outf;
    Label* culabel;
    unsigned cubyte = 1;

    std::string sub_bytes (unsigned bytes, bool doinput) {
        while ( (bytes % (doinput ? 16 : 4)) ) { bytes++; }
        return std::to_string(bytes);
    }

    std::string set_bytes (unsigned bytes) {
        std::string setting;
        while ( bytes ) {
            setting += "\tmovb      $0, -" + std::to_string(bytes--) + "(%rbp)\n";
        }
        return setting;
    }
}

void asm_do_file (unsigned bytes, bool doinput) {
    Assembly::outf = fopen("out.s", "w");
    fprintf(Assembly::outf, ".text\n");
    if ( doinput ) {
        fprintf(Assembly::outf, ".section .rodata\n");
        fprintf(Assembly::outf, ".GET_IN:\n");
        fprintf(Assembly::outf, "\t.string \" %%c\"\n");
        fprintf(Assembly::outf, "\t.text\n");
    }

    fprintf(Assembly::outf, ".globl main\n");
    fprintf(Assembly::outf, ".type main, @function\n");
    Assembly::labels.push_back(Label {
        .temp = "main:\n"
                "\tpushq     %rbp\n"
                "\tmovq      %rsp, %rbp\n"
                "\tsubq      $" + Assembly::sub_bytes(bytes, doinput)  + ", %rsp\n" +
                Assembly::set_bytes(bytes) +
                "%s"
                "\tleave\n"
                "\tret\n"
    });
    Assembly::culabel = &Assembly::labels.back();
}

void asm_inc_byte (unsigned times) {
    Assembly::culabel->body += "\taddb      $" + std::to_string(times) + ", -" + std::to_string(Assembly::cubyte) + "(%rbp)\n";
}

void asm_dec_byte (unsigned times) {
    Assembly::culabel->body += "\tsubb      $" + std::to_string(times) + ", -" + std::to_string(Assembly::cubyte) + "(%rbp)\n";
}

void asm_out_byte (unsigned times) {
    Assembly::culabel->body += "\tmovsbl    -" + std::to_string(Assembly::cubyte) + "(%rbp), %edi\n" +
                               "\tcall      putchar@PLT\n" +
                               "\tmovl      $0, %eax\n";
    while ( --times ) { asm_out_byte(times); }
}

void asm_inp_byte (unsigned times) {
    Assembly::culabel->body += "\tleaq      -" + std::to_string(Assembly::cubyte) + "(%rbp), %rsi\n"
                               "\tleaq      .GET_IN(%rip), %rdi\n"
                               "\tmovl      $0, %eax\n"
                               "\tcall      __isoc99_scanf@PLT\n"
                               "\tmovl      $0, %eax\n";
    while ( --times ) { asm_out_byte(times); }
}

void asm_prv_byte () { Assembly::cubyte--; }
void asm_nxt_byte () { Assembly::cubyte++; }

void asm_new_loop () {
    static unsigned numloops = 1;
    const std::string LC = "LC" + std::to_string(numloops);
    const std::string LB = "LB" + std::to_string(numloops);

    Assembly::culabel->body += "\tcall      " + LC + "\n";
    Assembly::labels.push_back(Label {
        .temp = LC + ":\n"
                "\tcmpb      $0, -" + std::to_string(Assembly::cubyte) + "(%rbp)\n"
                "\tjne       " + LB + "\n"
                "\tret\n" +
                LB + ":\n"
                "%s"
                "\tjmp       " + LC + "\n"
                "\tret\n"
    });

    Assembly::culabel = &Assembly::labels.back();
    numloops++;
}

void asm_end_loop () {
    fprintf(Assembly::outf, Assembly::culabel->temp.c_str(), Assembly::culabel->body.c_str());
    Assembly::culabel = &Assembly::labels.rbegin()[1];
    Assembly::labels.pop_back();
}

void asm_write () {
    fprintf(Assembly::outf, Assembly::culabel->temp.c_str(), Assembly::culabel->body.c_str());
    fclose(Assembly::outf);
}
