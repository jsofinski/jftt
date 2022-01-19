all: gram

gram.tab.c gram.tab.h: gram.y
	bison -d gram.y

lex.yy.c: lex.l gram.tab.h
	flex lex.l

gram: lex.yy.c gram.tab.c gram.tab.h codegen.cpp
	g++ -o gram gram.tab.c lex.yy.c codegen.cpp

clean:
	rm gram gram.tab.c lex.yy.c gram.tab.h

test: gram
	./gram test1.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr