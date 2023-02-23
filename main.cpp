#include "assembly.hpp"
#include <string.h>

namespace Args {
    std::string bfcompile;
    std::string saveasm_as = "out";
}

namespace Prgm {
    typedef struct Token {
        unsigned times;
        const char token;
    } Token;
    std::vector<Prgm::Token> tokens;
    bool input = false;

    void new_token (char token, bool repeats) {
        /* Tokens in brainfuck can be accumulated. Instead of
         * creating one token per every token, it can be simplyfed
         * to save a token that appears N times secuentually.
         * */
        static char prevtoken = '0';
        if ( repeats && token == prevtoken ) {
            Prgm::tokens.back().times++;
            return;
        }

        if ( token == ',' ) { Prgm::input = true; }
        Prgm::tokens.push_back(Prgm::Token {
            .times = 1,
            .token = token
        });
        prevtoken = token;
    }

    void pass_tokens () {
        for (Token const &token : Prgm::tokens) {
            switch ( token.token ) {
                case '+' : case '-' : { asm_incdec(token.times, token.token); break; }
                case '.' : { asm_print(token.times); break; }
                case ',' : { asm_input(token.times); break; }
                case '<' : case '>' : { asm_moveThrough(token.times, token.token); break; }
                case '[' : { asm_startLoop(); break; }
                case ']' : { asm_endLoop(); break; }
            }
        }
        asm_write();
    }
}

void usage () {
    puts("U: This compiler has these arguments:");
    puts("    * -f <name>: brainfuck file to be compiled.");
    puts("    * -s: Save the assembly file (optional).");
    exit(0);
}

void readArgs (char** args, int narg) {
    for (int i = 1; i < narg - 1; i++) {;
        if ( !strcmp(args[i], "-f") ) { Args::bfcompile = std::string(args[++i]); }
        if ( !strcmp(args[i], "-s") ) { Args::saveasm_as = std::string(args[++i]); }
    }

    if ( Args::bfcompile.empty() ) { 
        puts("E: No -f argument found, the compiler needs that argument!");
        exit(1);
    }
}

void readFile () {
    FILE* bfile = fopen(Args::bfcompile.c_str(), "r");
    if ( !bfile ) {
        puts("E: File given as argument doesn't exist!");
        exit(1);
    }

    char token;
    while ( (token = fgetc(bfile)) != EOF ) {
        switch ( token ) {
            case '+' : case '-':
            case '.' : case ',':
            case '<' : case '>': { Prgm::new_token(token, true); break; }

            case '[' : case ']': { Prgm::new_token(token, false); break; }
        }
    }
    fclose(bfile);
}

int main (int argc, char** argv) {
    if ( argc < 2 ) { usage(); }

    readArgs(argv, argc);
    readFile();
    asm_init(Args::saveasm_as + ".s", Prgm::input);
    Prgm::pass_tokens();
    return 0;
}
