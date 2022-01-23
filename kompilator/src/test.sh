# ./gram ./tests/gebala/program0.imp > test.mr 
# ./gram ./tests/gebala/program1.imp > test.mr 
./gram ./tests/gebala/program2.imp > test.mr 
# ./gram ./tests/gebala/0-div-mod.imp > test.mr 
# ./gram ./tests/gebala/1-numbers.imp > test.mr 
# ./gram ./tests/gebala/2-fib.imp > test.mr 
# ./gram ./tests/gebala/3-fib-factorial.imp > test.mr 
# ./gram ./tests/gebala/4-factorial.imp > test.mr 
# ./gram ./tests/gebala/5-tab.imp > test.mr 
# ./gram ./tests/gebala/6-mod-mult.imp > test.mr 
# ./gram ./tests/gebala/7-loopiii.imp > test.mr 
# ./gram ./tests/gebala/8-for.imp > test.mr 
# ./gram ./tests/gebala/9-sort.imp > test.mr 






# ./gram ./tests/slowik/test0.imp > test.mr
# ./gram ./tests/slowik/test1a.imp > test.mr 
# ./gram ./tests/slowik/test1b.imp > test.mr 
# ./gram ./tests/slowik/test1c.imp > test.mr 
# ./gram ./tests/slowik/test1d.imp > test.mr
# ./gram ./tests/slowik/test2.imp > test.mr  



./vm/maszyna_wirtualna/maszyna-wirtualna test.mr
# ./vm/maszyna_wirtualna/maszyna-wirtualna-cln test.mr