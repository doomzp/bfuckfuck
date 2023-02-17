#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP
#include <vector>
#include <iostream>

void asm_do_file (unsigned bytes, bool doinput);
void asm_inc_byte (unsigned times);
void asm_dec_byte (unsigned times);
void asm_out_byte (unsigned times);
void asm_inp_byte (unsigned times);

void asm_write ();

#endif
