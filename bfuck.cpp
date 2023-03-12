#include "assembly.hpp"
#include <string.h>
#include <algorithm>

namespace Args {
    std::string bfcompile;
    std::string saveasm_as = "out";
    std::string bytes;
    bool todebug = false;

    bool checkBytes () {
        return std::all_of(Args::bytes.begin(), Args::bytes.end(), isdigit);
    }
}

namespace Prgm {
    typedef struct Token {
        unsigned times;
        const char token;
    } Token;
    std::vector<Prgm::Token> tokens;

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
                case '[' : case '(' : { asm_startLoop(token.token); break; }
                case ']' : case ')' : { asm_endLoop(); break; }
            }
        }
        asm_write();
    }
}

void usage () {
    puts("U: This compiler has these arguments:");
    puts("    * -f <name>: brainfuck file to be compiled (must).");
    puts("    * -s <name>: Save the assembly file with an specific name 'out' by default.");
    puts("    * -b <bytes>: Use a number specific of bytes.");
    puts("    * -d: Get the debug information to debug it after.");
    exit(0);
}

void readArgs (char** args, int narg) {
    for (int i = 1; i < narg; i++) {
        if ( (i + 1) < narg ) {
            if ( !strcmp(args[i], "-f") ) { Args::bfcompile = std::string(args[++i]); }
            if ( !strcmp(args[i], "-s") ) { Args::saveasm_as = std::string(args[++i]); }
            if ( !strcmp(args[i], "-b") ) { Args::bytes = std::string(args[++i]); }
        }
        if ( !strcmp(args[i], "-d") ) { Args::todebug = true; }
    }

    if ( Args::bfcompile.empty() ) { 
        puts("E: No -f argument found, the compiler needs that argument!");
        exit(1);
    }
    if ( Args::bytes.empty() ) { Args::bytes = "30000"; return; }
    if ( !Args::checkBytes() ) {
        puts("E: The number specifed with -b argument isn't a natural number!");
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

            case '[' : case ']':
            case '(' : case ')': { Prgm::new_token(token, false); break; }
        }
    }
    fclose(bfile);
}

int main (int argc, char** argv) {
    if ( argc < 2 ) { usage(); }

    readArgs(argv, argc);
    readFile();

    const std::string name = Args::saveasm_as + ".s";
    asm_init(name, Args::bytes);
    Prgm::pass_tokens();

    if ( Args::todebug ) {
        const std::string compile = "gcc -g " + name + " -o debug_info";
        system(compile.c_str());
    }
    return 0;
}
