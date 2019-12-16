#include"decoder.h"
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
                                        
                                        
    cleanFlag(inst);

    return 0;
}

void initInst(instruction *inst){
    inst->opcode = 0;
    inst->rs = 0;
    inst->rt = 0;
    inst->rd = 0;
    inst->shamt = 0;
    inst->func = 0;
    inst->constant_16 = 0;
    inst->constant_26 = 0;
    cleanFlag(inst);
}

void cleanFlag(instruction* inst){
    inst->forwardFlag_rs = 0;
    inst->forwardFlag_rt = 0;
    inst->stallFlag = 0;
    inst->nopFlag = 0;
    inst->memAccessFlag = 0;
    inst->wbFlag = getWbFlag(inst);
    inst->nextPc = -1;
}

int getWbFlag(instruction *inst){
    switch(inst->opcode){
    case 0:
        if(inst->func==0x08) return -1;
        return 1;
    case 8:
	//--addi--//
		return 2;
	case 9:
	//--addiu--//
		return 2;
	case 35:
	//--lw--//
		return 2;
		
	case 33:
	//--lh--//
		return 2;
		
	case 37:
	//--lhu--//
		return 2;
		
	case 32:
	//--lb--//
		return 2;
		
	case 36:
	//--lbu--//
		return 2;
		
	case 15:
	//--lui--//
		return 2;

	case 12:
	//--andi--//
		return 2;

	case 13:
	//--ori--//
		return 2;

	case 14:
	//--nori--//
		return 2;
	case 10:
	//--slti--//
		return 2;
    case 3:     //this is pectial case
    //--jal--//
        return 2;
    case 0x2B:
    //--sw--//
        return 3;
    case 0x29:
    //--sh--//
        return 3;
    case 0x28:
    //--sb--//
        return 3;
	default: return -1;
    }
}