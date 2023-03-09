#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP
#include <iostream>
#include <vector>

void asm_init (const std::string &asmname, const std::string &bytes);
void asm_incdec (unsigned times, char kase);
void asm_print (unsigned times);
void asm_input (unsigned times);
void asm_moveThrough (unsigned times, char kase);
void asm_startLoop (char kase);
void asm_endLoop ();
void asm_write ();

#endif
