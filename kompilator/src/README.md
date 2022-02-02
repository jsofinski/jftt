Jakub Sofiński
numer indeksu: 254665

Pliki kompilatora:
lex.l - lexer
gram.y - parser

header.h - zawiera symbole (enumy) operacji, relacji i rozkazów maszyny
codegen.h - header codegen.cpp
codegen.cpp - zawiera wszystkie funkcje do budowania drzewa oraz drukowania do języka maszyny


Program kompiluje się Makefilem poleceniem "make". Plikem wykonywalnym jest "kompilator",
który na wejście przyjmuje inputFile i outputFile (np. ./kompilator test.imp test.mr).