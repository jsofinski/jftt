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
	./gram test.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna-cln test.mr

test1: gram
	./gram ./tests/test1.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr

test2: gram
	./gram ./tests/test2.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr

test3: gram
	./gram ./tests/test3.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr

test4: gram
	./gram ./tests/test4.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr

test5: gram
	./gram ./tests/test5_4.imp > test.mr 
	./vm/maszyna_wirtualna/maszyna-wirtualna test.mr