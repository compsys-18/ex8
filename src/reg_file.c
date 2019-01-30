#include <stdio.h>
#include <mips.h>

void register_run(Register *reg, Signal wctl, Word wdata, Word *rdata)
{
    word_set_value(rdata, register_get_value(reg));
    if(wctl) register_set_value(reg, word_get_value(wdata));
}

void register_set_value(Register *reg, int val)
{
    reg->val = val;
}

int register_get_value(Register *reg)
{
    return reg->val;
}

void decorder5(Signal a[5], Word *b)
{
    int i, val;
    val = 0;
    for (i = 0; i < 5; ++i) {
        if (a[i]) {
            val += (1 << i);
        }
    }
    for(i = 0; i < 32; i++){
      if(i == val) b->bit[i] = true;
      else b->bit[i] = false;
    }
}

void mux32(Word ins[32], Signal ctls[5], Word *out)
{
    int i, val = 0;
    for(i = 0; i < 5; i++){
      if(ctls[i]) val += (1 << i);
    }
    *out = ins[val];
}

void register_file_run(RegisterFile *rf, Signal register_write, Signal *read1, Signal *read2, Signal *write1, Word wdata, Word *rdata1, Word *rdata2)
{
    Word in, ins[32];
    Signal inner;
    decorder5(write1, &in);
    register_run(&(rf->r[0]), 0, wdata, ins);
    for(int i = 1; i < 32; i++){
      and_gate(register_write, in.bit[i], &inner);
      register_run(&(rf->r[i]), inner, wdata, ins + i);
    }
    mux32(ins, read1, rdata1);
    mux32(ins, read2, rdata2);
}

void test_register_file()
{
    Signal register_write;
    Signal read1[5] = {true, false, false, false, false};
    Signal read2[5] = {false, true, false, false, false};
    Signal write1[5] = {true, false, false, false, false};
    Word wdata, rdata1, rdata2;
    RegisterFile rf;

    register_write = true;
    word_set_value(&wdata, 100);

    /*read1はregister1の値(=0)を読み出す
    read2はregister2の値(=0)を読み出す
    register1にwdata(=100)を書き込む*/
    register_file_run(&rf, register_write, read1, read2, write1, wdata, &rdata1, &rdata2);

    printf("first data of rdata1: %d\n", word_get_value(rdata1));
    printf("first data of rdata2: %d\n", word_get_value(rdata2));

    register_write = false;
    word_set_value(&wdata, 200);

    /*read1はregister1の値(=100)を読み出す
    read2はregister2の値(=0)を読み出す
    registerに書き込まない*/
    register_file_run(&rf, register_write, read1, read2, write1, wdata, &rdata1, &rdata2);
    printf("second data of rdata1: %d\n", word_get_value(rdata1));
    printf("second data of rdata2: %d\n", word_get_value(rdata2));

    register_write = true;

    /*read1はregister1の値(=100)を読み出す
    read2はregister2の値(=0)を読み出す
    register1にwdata(=200)を書き込む*/
    register_file_run(&rf, register_write, read1, read2, write1, wdata, &rdata1, &rdata2);
    printf("third data of rdata1: %d\n", word_get_value(rdata1));
    printf("third data of rdata2: %d\n", word_get_value(rdata2));

    write1[0] = false;
    write1[1] = true;

    /*read1はregister1の値(=200)を読み出す
    read2はregister2の値(=0)を読み出す
    register2にwdata(=200)を書き込む*/
    register_file_run(&rf, register_write, read1, read2, write1, wdata, &rdata1, &rdata2);
    printf("fourth data of rdata1: %d\n", word_get_value(rdata1));
    printf("fourth data of rdata2: %d\n", word_get_value(rdata2));

    /*read1はregister1の値(=200)を読み出す
    read2はregister2の値(=200)を読み出す
    register2にwdata(=200)を書き込む*/
    register_file_run(&rf, register_write, read1, read2, write1, wdata, &rdata1, &rdata2);
    printf("fifth data of rdata1: %d\n", word_get_value(rdata1));
    printf("fifth data of rdata2: %d\n", word_get_value(rdata2));
}
