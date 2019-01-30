#include <stdio.h>
#include <stdlib.h>
#include <mips.h>

void full_adder(Signal in1, Signal in2, Signal carry_in, Signal *out1, Signal *carry_out)
{
    Signal inner1, inner2, inner3;
    xor_circuit(in1, in2, &inner1);
    xor_circuit(carry_in, inner1, out1);
    and_gate(in1, in2, &inner2);
    and_gate(carry_in, inner1, &inner3);
    or_gate(inner2, inner3, carry_out);
}

void rca(Word in1, Word in2, Signal carry_in, Word *out1, Signal *carry_out)
{
    Signal c = carry_in;
    for(int i = 0; i < 32; i++){
      full_adder(in1.bit[i], in2.bit[i], c, &out1->bit[i], &c);
    }
    *carry_out = c;
}

void test_full_adder()
{
    Signal a, b, ci, sum, co;
    for(ci = 0; ci <= 1; ci++){
      for(a = 0; a <= 1; a++){
        for(b = 0; b <= 1; b++){
          full_adder(a, b, ci, &sum, &co);
          printf("full_adder(in1 = %d, in2 = %d, carry_in = %d) => (sum = %d, carry_out = %d)\n", a, b, ci, sum, co);
        }
      }
    }
}

void test_rca(int val1, int val2)
{
    Word *w1, *w2, *out;
    Signal ci = false, *co;
    int res;
    w1 = (Word *)malloc(sizeof(Word));
    w2 = (Word *)malloc(sizeof(Word));
    out = (Word *)malloc(sizeof(Word));
    co = (Signal *)malloc(sizeof(Signal));
    word_set_value(w1, val1);
    word_set_value(w2, val2);
    rca(*w1, *w2, ci, out, co);
    res = word_get_value(*out);
    printf("RCA(%d, %d) => %d\n", val1, val2, res);
    free(w1);
    free(w2);
    free(out);
    free(co);
}
