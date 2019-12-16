#ifndef Instruction_h
#define Instruction_h

struct instructions {
    unsigned int opcode;
    unsigned int rs,rt,rd;
    unsigned int shamt;
    unsigned int func;
    
    unsigned int constant_16;
    unsigned int constant_26;
};
typedef struct instructions instruction;

#endif