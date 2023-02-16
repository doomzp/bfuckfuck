#include "parse.hpp"

void usage () {
    puts("U: Pass a .bf file to compile!");
    exit(0);
}

int main (int argc, char** argv) {
    FILE* bfile = fopen(argv[1], "r");
    if ( !bfile ) { usage(); }

    parse_read(bfile);
    return 0;
}
