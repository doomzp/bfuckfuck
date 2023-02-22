#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP
#include <iostream>
#include <vector>

void asm_init (const std::string &asmname, bool input);
void asm_incdec (unsigned times, char kase);
void asm_print (unsigned times);
void asm_input (unsigned times);
void asm_write ();

#endif
