#include <stdio.h>
#include <mips.h>

void mux(Signal in1, Signal in2, Signal ctl, Signal *out1)
{
    Signal inner1[2], inner2[2], inner3[2];

    /*ctl != true*/
    not_gate(ctl, inner1 + 0);
    inner1[1] = in1;
    andn_gate(inner1, 2 ,inner3 + 0);

    /*ctl == true*/
    inner2[0] = ctl;
    inner2[1] = in2;
    andn_gate(inner2, 2, inner3 + 1);

    orn_gate(inner3, 2, out1);
}

void mux4(Signal in1, Signal in2, Signal in3, Signal in4, Signal ctl1, Signal ctl2, Signal *out1)
{
    Signal inner1[3], inner2[3], inner3[3], inner4[3], inner5[4];

    /* ctl1 != true && ctl2 != true) */
    not_gate(ctl1, inner1 + 0);
    not_gate(ctl2, inner1 + 1);
    inner1[2] = in1;
    andn_gate(inner1, 3, inner5 + 0);

    /* ctl1 == true && ctl2 != true) */
    inner2[0] = ctl1;
    not_gate(ctl2, inner2 + 1);
    inner2[2] = in2;
    andn_gate(inner2, 3, inner5 + 1);

    /* (ctl1 != true && ctl2 == true) */
    not_gate(ctl1, inner3 + 0);
    inner3[1] = ctl2;
    inner3[2] = in3;
    andn_gate(inner3, 3, inner5 + 2);

    /* (ctl1 == true && ctl2 == true) */
    inner4[0] = ctl1;
    inner4[1] = ctl2;
    inner4[2] = in4;
    andn_gate(inner4, 3, inner5 + 3);

    orn_gate(inner5, 4, out1);
}

void alu(Signal *ops, Signal a, Signal b, Signal less, Signal carry_in, Signal *s, Signal *carry_out)
{
    Signal inner1, inner2, inner3, inner4 = less, inner5, inner6;
    not_gate(b, &inner5);
    mux(b, inner5, *(ops + 2), &inner6);
    and_gate(a, b, &inner1);
    or_gate(a, b, &inner2);
    full_adder(a, inner6, carry_in, &inner3, carry_out);
    mux4(inner1, inner2, inner3, inner4, *ops, *(ops + 1), s);
}

void alu_msb(Signal *ops, Signal a, Signal b, Signal less, Signal carry_in, Signal *s, Signal *carry_out, Signal *set, Signal *overflow)
{
    Signal inner1, inner2, inner3, inner4 = less, inner5, inner6;
    not_gate(b, &inner5);
    mux(b, inner5, *(ops + 2), &inner6);
    and_gate(a, b, &inner1);
    or_gate(a, b, &inner2);
    full_adder(a, inner6, carry_in, &inner3, carry_out);
    *set = inner3;
    mux4(inner1, inner2, inner3, inner4, *ops, *(ops + 1), s);

    //check overflow
    Signal in1[4], in2[4];

    /*(a>=0 + b>=0) <0*/
    not_gate(*(ops + 2), in1 + 0);
    not_gate(a, in1 + 1);
    not_gate(b, in1 + 2);
    in1[3] = *set;
    andn_gate(in1, 4, in2 + 0);

    /*(a<0 + b<0) >=0*/
    not_gate(*(ops + 2), in1 + 0);
    in1[1] = a;
    in1[2] = b;
    not_gate(*set, in1 + 3);
    andn_gate(in1, 4, in2 + 1);

    /*(a>=0 - b<0) <0*/
    in1[0] = *(ops + 2);
    not_gate(a, in1 + 1);
    in1[2] = b;
    in1[3] = *set;
    andn_gate(in1, 4, in2 + 2);

    /*(a<0 - b>=0) >=0*/
    in1[0] = *(ops + 2);
    in1[1] = a;
    not_gate(b, in1 + 2);
    not_gate(*set, in1 + 3);
    andn_gate(in1, 4, in2 + 3);

    orn_gate(in2, 4, overflow);
}

void alu32(Signal *ops, Word a, Word b, Word *s, Signal *zero, Signal *overflow)
{
    Signal c = *(ops + 2), set, inner;
    for(int i = 0; i < 31; i++){
      alu(ops, a.bit[i], b.bit[i], 0, c, &s->bit[i], &c);
    }
    alu_msb(ops, a.bit[31], b.bit[31], 0, c, &s->bit[31], &c, &set, overflow);
    alu(ops, a.bit[0], b.bit[0], set, *(ops + 2), &s->bit[0], &c);
    orn_gate(&s->bit[0], 32, &inner);
    not_gate(inner, zero);
}

void test_mux(){
    Signal a, b, c, out;
    for(a = 0; a <= 1; a++){
      for(b = 0; b <= 1; b++){
        for(c = 0; c <= 1; c++){
          mux(a, b, c, &out);
          printf("MUX(%d, %d, %d) => (%d)\n", a, b, c, out);
        }
      }
    }
}

void test_alu()
{
    /* Exercise 6-1 */
    Signal a, b, carry_in, s, carry_out;
    Signal ops[3];

    /* AND */
    ops[0] = false;
    ops[1] = false;
    ops[2] = false;
    for (carry_in = 0; carry_in <= 1; ++carry_in) {
        for (a = 0; a <= 1; ++a) {
            for (b = 0; b <= 1; ++b) {
                alu(ops, a, b, false, carry_in, &s, &carry_out);
                printf("ALU(AND, %d, %d, %d) => (%d, %d)\n", a, b, carry_in, s, carry_out);
            }
        }
    }
    /* OR */
    ops[0] = true;
    ops[1] = false;
    ops[2] = false;
    for (carry_in = 0; carry_in <= 1; ++carry_in) {
        for (a = 0; a <= 1; ++a) {
            for (b = 0; b <= 1; ++b) {
                alu(ops, a, b, false, carry_in, &s, &carry_out);
                printf("ALU(OR, %d, %d, %d) => (%d, %d)\n", a, b, carry_in, s, carry_out);
            }
        }
    }
    /* ADD */
    ops[0] = false;
    ops[1] = true;
    ops[2] = false;
    for (carry_in = 0; carry_in <= 1; ++carry_in) {
        for (a = 0; a <= 1; ++a) {
            for (b = 0; b <= 1; ++b) {
                alu(ops, a, b, false, carry_in, &s, &carry_out);
                printf("ALU(ADD, %d, %d, %d) => (%d, %d)\n", a, b, carry_in, s, carry_out);
            }
        }
    }
}

void test_alu32(int val1, int val2)
{
    /* Exercise 6-1 */
    Signal ops[3];
    Word in1, in2, out1;
    Signal carry_in, carry_out, overflow;
    word_set_value(&in1, val1);
    word_set_value(&in2, val2);
    carry_in = false;

    printf("a = %d, b = %d\n", val1, val2);

    /* AND */
    ops[0] = false;
    ops[1] = false;
    ops[2] = false;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("ALU32(AND, %x, %x, %d) => (%x, %d, %d)\n", val1, val2, carry_in, word_get_value(out1), carry_out, overflow);

    /* OR */
    ops[0] = true;
    ops[1] = false;
    ops[2] = false;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("ALU32(OR, %x, %x, %d) => (%x, %d, %d)\n", val1, val2, carry_in, word_get_value(out1), carry_out, overflow);
    /* ADD */
    ops[0] = false;
    ops[1] = true;
    ops[2] = false;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("ALU32(ADD, %x, %x, %d) => (%x, %d, %d)\n", val1, val2, carry_in, word_get_value(out1), carry_out, overflow);
    /* SUB (also check zero?) */
    ops[0] = false;
    ops[1] = true;
    ops[2] = true;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("ALU32(SUB, %x, %x, %d) => (%x, %d, %d) ", val1, val2, carry_in, word_get_value(out1), carry_out, overflow);
    if(carry_out) printf("a = b\n");
    else printf("a != b\n");
    /* SLT */
    ops[0] = true;
    ops[1] = true;
    ops[2] = true;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("ALU32(SLT, %x, %x, %d) => (%x, %d, %d)\n", val1, val2, carry_in, word_get_value(out1), carry_out, overflow);
}

void test_overflow(){
    Signal ops[3], carry_in = false, carry_out, overflow;
    Word in1, in2, out1;

    /*(a>=0 + b>=0) <0*/
    word_set_value(&in1, (1<<30));
    word_set_value(&in2, (1<<30));
    ops[0] = false;
    ops[1] = true;
    ops[2] = false;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("OVERFLOW(ADD, %x >=0, %x >=0, %d) => (%x <0, %d, %d)\n", (1<<30), (1<<30), carry_in, word_get_value(out1), carry_out, overflow);

    /*(a<0 + b<0) >=0*/
    word_set_value(&in1, (1<<31) + 1);
    word_set_value(&in2, (1<<31));
    ops[0] = false;
    ops[1] = true;
    ops[2] = false;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("OVERFLOW(ADD, %x <0, %x <0, %d) => (%x >=0, %d, %d)\n", (1<<31) + 1, (1<<31), carry_in, word_get_value(out1), carry_out, overflow);

    /*(a>=0 - b<0) <0*/
    word_set_value(&in1, (1<<30));
    word_set_value(&in2, (1<<31));
    ops[0] = false;
    ops[1] = true;
    ops[2] = true;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("OVERFLOW(SUB, %x >=0, %x <0, %d) => (%x <0, %d, %d)\n", (1<<30), (1<<31), carry_in, word_get_value(out1), carry_out, overflow);

    /*(a<0 - b>=0) >=0*/
    word_set_value(&in1, (1<<31));
    word_set_value(&in2, (1<<30));
    ops[0] = false;
    ops[1] = true;
    ops[2] = true;
    alu32(ops, in1, in2, &out1, &carry_out, &overflow);
    printf("OVERFLOW(SUB, %x <0, %x >=0, %d) => (%x >=0, %d, %d)\n", (1<<31), (1<<30), carry_in, word_get_value(out1), carry_out, overflow);
}
