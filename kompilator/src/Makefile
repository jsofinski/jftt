all: kompilator

gram.tab.c gram.tab.h: gram.y
	bison -d gram.y

lex.yy.c: lex.l gram.tab.h
	flex lex.l

kompilator: lex.yy.c gram.tab.c gram.tab.h codegen.cpp
	g++ -o kompilator gram.tab.c lex.yy.c codegen.cpp

clean:
	rm kompilator gram.tab.c lex.yy.c gram.tab.h

testg: kompilator
	./kompilator ./tests/gebala/7-loopiii.imp test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna-cln test.mr

test: kompilator
	./kompilator test.imp test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr