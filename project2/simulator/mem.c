#include"mem.h"

void mem(instruction *inst){
    unsigned int un_tmp;
    int tmp;
    int i;
    unsigned int reg_rt = getReg(inst->rt);
    if(inst->forwardFlag_rt) reg_rt = inst->rtValue;
    switch(inst->opcode){
    case 35:
	//--lw--//
		un_tmp = 0;
		for(i=0;i<4;i++){ //read 4 bytes
			un_tmp |= getDMem(inst->result+i);
			if(i<3)un_tmp = un_tmp<<8;
		}
		inst->result = un_tmp;
		break;
		
	case 33:
	//--lh--//
		tmp = (char)getDMem(inst->result);	
		tmp = tmp<<8;
		tmp = tmp | getDMem(inst->result+1);
		inst->result = tmp;
		break;
		
	case 37:
	//--lhu--//
		tmp = getDMem(inst->result);
		tmp = tmp<<8;
		tmp |= getDMem(inst->result+1);
		inst->result = tmp;
		break;
		
	case 32:
	//--lb--//
		tmp = (char)getDMem(inst->result);
		inst->result = tmp;
		break;
		
	case 36:
	//--lbu--//
		tmp = getDMem(inst->result);
		inst->result = tmp;
		break;
		
	case 43:
	//--sw--//
		setDMem(inst->result,(reg_rt&0xFF000000)>>24);
		setDMem(inst->result+1,(reg_rt&0x00FF0000)>>16);
		setDMem(inst->result+2,(reg_rt&0x0000FF00)>>8);
		setDMem(inst->result+3,reg_rt&0x000000FF);
		break;

	case 41:
	//--sh--//
		setDMem(inst->result,(reg_rt&0x0000FF00)>>8);
		setDMem(inst->result+1,reg_rt&0x000000FF);
		break;
	
	case 40:
	//--sb--//
        setDMem(inst->result,reg_rt&0x000000FF);
		break;
    default:
        puts("[x] Opcode not found in mem().");
    }
}