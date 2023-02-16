#include "parse.hpp"

typedef struct Token {
    const char data;
    unsigned times;
} Token;

namespace Prgm {
    std::vector<Token> tokens;
}

void parse_read (FILE *bfile) {
    char token;
    while ( (token = fgetc(bfile)) != EOF ) {
        switch ( token ) {
            case '+' : case '-' :
            case '<' : case '>' :
            case '[' : case ']' :
            case '.' : case ',' : { parse_new(token); }
        }
    }
    fclose(bfile);
}

void parse_new (char token) {
    static char prevtype = '0';
    if ( token == prevtype ) {
        Prgm::tokens.back().times++;
        return;
    }
    
    Prgm::tokens.push_back(Token {
        .data = token,
        .times = 1
    });
    prevtype = token;
}
