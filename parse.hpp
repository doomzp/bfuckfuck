#ifndef _PARSE_HPP
#define _PARSE_HPP
#include "assembly.hpp"

void parse_read (FILE *bfile);
void parse_new (char token, bool acumulate);
void parse_run ();

#endif
