#include <stdio.h>
#include <mips.h>

void inst_memory_run(InstMemory *im, Word addr, Word *inst)
{
    int val, offset;
    val = word_get_value(addr);
    offset = (val - INST_MEM_START) / 4;
    word_set_value(inst, im->mem[offset]);
}

void inst_memory_set_inst(InstMemory *im, int addr, int inst)
{
    int offset;
    offset = (addr - INST_MEM_START) / 4;
    im->mem[offset] = inst;
}

void data_memory_run(DataMemory *dm, Signal mem_write, Signal mem_read, Word addr, Word wdata, Word *rdata)
{
    int val, offset;
    val = word_get_value(addr);
    offset = (val - DATA_MEM_START) / 4;

    if (mem_read) {
        word_set_value(rdata, dm->mem[offset]);
    }
    else if (mem_write) {
        dm->mem[offset] = word_get_value(wdata);
    }
}

void test_inst_memory()
{
    Word addr, addr1, addr2, inst, inst1, inst2;
    InstMemory im;

    word_set_value(&addr, 0x04000000);
    word_set_value(&addr1, 0x04000004);
    word_set_value(&addr2, 0x04000008);

    inst_memory_set_inst(&im, 0x04000000, 350);
    inst_memory_set_inst(&im, 0x04000004, 100);
    inst_memory_set_inst(&im, 0x04000008, 200);

    inst_memory_run(&im, addr, &inst);
    inst_memory_run(&im, addr1, &inst1);
    inst_memory_run(&im, addr2, &inst2);

    printf("InstMemory[0x04000000] = %d\n", word_get_value(inst));
    printf("InstMemory[0x04000004] = %d\n", word_get_value(inst1));
    printf("InstMemory[0x04000008] = %d\n", word_get_value(inst2));
}

void test_data_memory()
{
    DataMemory dm;
    Signal mem_write, mem_read;
    Word addr, wdata, rdata;

    word_set_value(&rdata, 0);
    mem_write = true;
    mem_read = false;

    word_set_value(&addr, 0x10000004);
    word_set_value(&wdata, 100);

    /*0x10000004に100を書き込む
    読み出しはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);

    printf("rdata[0x10000004] : %d\n", word_get_value(rdata));

    word_set_value(&addr, 0x10000008);
    word_set_value(&wdata, 200);

    /*0x10000008に200を書き込む
    読み出しはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);

    printf("rdata[0x10000008] : %d\n", word_get_value(rdata));

    word_set_value(&addr, 0x10000004);
    word_set_value(&wdata, 300);

    /*0x10000004に300を書き込む
    読み出しはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);

    printf("rdata[0x10000004] : %d\n", word_get_value(rdata));

    word_set_value(&wdata, 100);

    mem_write = false;
    mem_read = true;

    /*0x10000004(=300)を読み出す
    書き込みはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);
    printf("rdata[0x10000004] : %d\n", word_get_value(rdata));

    word_set_value(&addr, 0x10000008);

    /*0x10000008(=200)を読み出す
    書き込みはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);
    printf("rdata[0x10000008] : %d\n", word_get_value(rdata));

    /*0x10000008(=200)を読み出す
    書き込みはない*/
    data_memory_run(&dm, mem_write, mem_read, addr, wdata, &rdata);
    printf("rdata[0x10000008] : %d\n", word_get_value(rdata));
}
