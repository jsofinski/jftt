#ifndef HEADER_FILE
#define HEADER_FILE

typedef enum {
    cEQ,
    cNEQ,
    cGE,
    cLE,
    cGEQ,
    cLEQ,
} ConditionSymbols;

typedef enum {
    ePLUS,
    eMINUS,
    eTIMES,
    eDIV,
    eMOD,
} ExpressionSymbols;

typedef enum {
    GET,
    PUT,
    LOAD,
    STORE,
    ADD,
    SUB,
    SHIFT,
    SWAP,
    RESET,
    INC,
    DEC,
    JUMP,
    JPOS,
    JZERO,
    JNEG,
    HALT
} VMInstruction;

// if
// b == c -> b-c -> swap b, sub c, Ra == b-c. -> JPOS + JNEG (poza cialo ifa)
// JPOS 301
// JNEG 300
// b != c -> b-c -> swap b, sub c, Ra == b-c. -> JZERO(poza ciało ifa)
// JZERO 300
// b > c -> b-c > 0 -> swap b, sub c -> JZERO + JNEG (oba poza cialo ifa)
// b < c -> b-c < 0 -> swap b, sub c -> JZERO + JPOS (oba poza cialo ifa)
// b >= c -> b-c >= 0 -> JNEG (poza cialo ifa)
// b <= c -> b-c <= 0 -> JPOS (poza cialo ifa)

// if else
// b == c -> b-c -> swap b, sub c, Ra == b-c. -> JPOS JNEG / JUMP
// b != c -> b-c -> swap b, sub c, Ra == b-c. -> JZERO / JUMP
// b > c -> b-c > 0 -> swap b, sub c -> JNEG JZERO / JUMP
// b < c -> b-c < 0 -> swap b, sub c -> JPOS JZERO / JUMP
// b >= c -> b-c >= 0 -> JNEG (do elsa) JUMP (pod koniec ifa poza elsa)
// b <= c -> b-c <= 0 -> JPOS (do elsa) JUMP (pod koniec ifa poza elsa)
#endif
