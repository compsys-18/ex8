#include <mips.h>

void control_unit(Signal opcode[6], Signal *register_dst, Signal *register_write, Signal *aluop1)
{
    Signal inner[6], in;
    not_gate(opcode[0], inner);
    not_gate(opcode[1], inner + 1);
    not_gate(opcode[2], inner + 2);
    not_gate(opcode[3], inner + 3);
    not_gate(opcode[4], inner + 4);
    not_gate(opcode[5], inner + 5);
    andn_gate(inner, 6, &in);
    *register_dst = in;
    *register_write = in;
    *aluop1 = in;
}

void alu_control_unit(Signal *funct, Signal aluop1, Signal *ops)
{
    Signal inner1, inner2;
    or_gate(*funct, *(funct + 3), &inner1);
    and_gate(inner1, aluop1, ops);

    not_gate(*(funct + 2), &inner1);
    not_gate(aluop1, &inner2);
    or_gate(inner1, inner2, ops + 1);

    and_gate(*(funct + 1), aluop1, ops + 2);
}
