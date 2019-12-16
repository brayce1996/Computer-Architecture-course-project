#include<stdio.h>
#include"instruction.h"
int decode(unsigned char raw_inst[] , int pc , instruction *inst){
    inst->opcode = (raw_inst[pc])>>2;
    inst->rs = ((raw_inst[pc]&0x03)<<3) + (raw_inst[pc+1]>>5);
    inst->rt = (raw_inst[pc+1]&0x1F);
    inst->rd = (raw_inst[pc+2]>>3);
    inst->shamt = ((raw_inst[pc+2]&0x07)<<2) + (raw_inst[pc+3]>>6);
    inst->func = raw_inst[pc+3]&0x3F;
    
    inst->constant_16 = ((char)raw_inst[pc+2]<<8) | raw_inst[pc+3]; //<----- negative handling...
    
    inst->constant_26 = ((char)(raw_inst[pc]&0x03)<<24) | ((raw_inst[pc+1])<<16) |
                                        ((raw_inst[pc+2])<<8) | raw_inst[pc+3]; //<-- here too
    return 0;
}
