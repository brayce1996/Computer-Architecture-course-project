#ifndef Instruction_h
#define Instruction_h

struct instructions {
    unsigned int opcode;
    unsigned int rs,rt,rd;
    unsigned int shamt;
    unsigned int func;
    
    unsigned int constant_16;
    unsigned int constant_26;
    
    
    //--Warning : some variable might be (unsigned int)--//
    int result;         // store the result after EX stage
    
    int forwardFlag_rs; // if rs need to forwarding , set this flag to 1, 
                        // and the value will store in rsValue,
                        // or this flag will be 0.
    int rsValue;
    
    int forwardFlag_rt; // if rt need to forwarding , set this flag to 1,
                        // and the value will store in rtValue,
                        // or this flag will be 0.
    int rtValue;
    
    int stallFlag;      // if need to stall, set this flag to stall times,
                        // or this flag will be 0.
    
    int nopFlag;        // if want to flush this inst, set this flag to 1,
                        // or this flag will be 0.
    
    int memAccessFlag;  // if need to read memory at MEM stage, set this flag to 1,
                        // if need to write memory at MEM stage, set this flag to 2,
                        // or this flag will be 0.
                        
    int wbFlag;         // if this is R mode inst and need write back , set this flag to 1,
                        // if this is R mode inst and need write back , set this flag to 2,
                        // or this flag will be -1.
    
    int nextPc;         // if jump pc, set nextPc to the value,
                        // or set this to -1;
                        
};
typedef struct instructions instruction;

#endif