import os
os.system('./gram ./tests/test1.imp > test.mr ')
os.system('./vm/maszyna_wirtualna/maszyna-wirtualna-cln test.mr')
