#include "parse.hpp"

typedef struct Token {
    const char data;
    unsigned times;
} Token;

namespace Prgm {
    std::vector<Token> tokens;
    unsigned bytes = 1; /* How many memories are made. */
    bool doinput = false;

    void throughMems (char kase) {
        static unsigned prevbyte = 1;
        if ( kase == '<' ) {
            if ( (prevbyte - 1) > 0 ) { prevbyte--; }
            return;
        }

        if ( prevbyte == Prgm::bytes ) { Prgm::bytes++; }
        prevbyte++;
    }
}

void parse_read (FILE *bfile) {
    char token;
    while ( (token = fgetc(bfile)) != EOF ) {
        switch ( token ) {
            case '+' : case '-' :
            case '.' : case ',' : { parse_new(token, true); break; }

            case '<' : case '>' :
            case '[' : case ']' : { parse_new(token, false); break; }
        }
    }

    parse_run();
    fclose(bfile);
}

void parse_new (char token, bool acumulate) {
    static char prevtype = '0';
    if ( token == prevtype && acumulate ) {
        Prgm::tokens.back().times++;
        return;
    }
    
    Prgm::tokens.push_back(Token { .data = token, .times = 1 });
    if ( token == '<' || token == '>' ) { Prgm::throughMems(token); }
    if ( token == ',' ) { Prgm::doinput = true; }
    prevtype = token;
}

void parse_run () {
    asm_do_file(Prgm::bytes, Prgm::doinput);
    for (Token const &token : Prgm::tokens) {
        switch ( token.data ) {
            case '+' : { asm_inc_byte(token.times); break; }
            case '-' : { asm_dec_byte(token.times); break; }
            case '.' : { break; }
            case ',' : { break; }
            case '<' : { break; }
            case '>' : { break; }
            case '[' : { break; }
            case ']' : { break; }
        }
    }
    asm_write();
}
