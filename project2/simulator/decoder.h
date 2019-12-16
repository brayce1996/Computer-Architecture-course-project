#ifndef Decoder_h
#define Decoder_h

#include<stdio.h>
#include"instruction.h"

int decode(unsigned char*,int,instruction*);
void initInst(instruction* inst);
void cleanFlag(instruction* inst);
int getWbFlag(instruction *inst);

#endif