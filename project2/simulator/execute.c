#include<stdio.h>
#include"instruction.h"
#include"execute.h"

#define ERROR_CODE -1
#define FINISH_CODE 1


int R_mode(instruction *inst,FILE *fError){

	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);

	if(inst->forwardFlag_rs) reg_rs = inst->rsValue;
	if(inst->forwardFlag_rt) reg_rt = inst->rtValue;
	
	switch(inst->func){
	case 32:
	//--add--//
		overflowDetect(reg_rs,reg_rt,fError);
		inst->result = reg_rs + reg_rt;
		return 0;
	case 33:
	//--addu--//
		inst->result = reg_rs + reg_rt;
		return 0;
	case 34:
	//--sub--//
		reg_rt = reg_rt*-1;
		overflowDetect(reg_rs,reg_rt,fError);
		inst->result = reg_rs + reg_rt;
		return 0;
		
	case 36:
	//--and--//
		inst->result = reg_rs & reg_rt;
		return 0;

	case 37:
	//--or--//
		inst->result = reg_rs | reg_rt;
		return 0;

	case 38:
	//--xor--//
		inst->result = reg_rs ^ reg_rt;
		return 0;

	case 39:
	//--nor--//
		inst->result = ~(reg_rs | reg_rt);
		return 0;
	
	case 40:
	//--nand--//
		inst->result = ~(reg_rs & reg_rt);
		return 0;
	
	case 42:
	//--slt--//
		inst->result = ((signed int)reg_rs < (signed int)reg_rt);
		return 0;
	
	case 0:
	//--sll--//
		// if(inst->rd==0){
		// 	if(!(inst->rt==0&&inst->shamt==0))	//<--NOP instruction, don't print error
		// 		fprintf(fError,"In cycle %d: Write $0 Error\n",getCycleCounter());
		// }
		inst->result = reg_rt<<(inst->shamt);
		return 0;

	case 2:
	//--srl--//
		inst->result = reg_rt>>(inst->shamt);
		return 0;
	
	case 3:
	//--sra--//
		inst->result = (int)reg_rt>>(inst->shamt);
		return 0;
	case 8:
	//--jr--//
		//setPc(reg_rs); //should do this at ID stage
		/*flush IF*/
		
		return 0;
	default:
		printf("[x]R mode func not found. func = 0x%02x\n",inst->func);
		return ERROR_CODE; //interrupt
	}
}

int execute(instruction *inst,FILE *fError){
	unsigned int reg_rs = getReg(inst->rs);
	if(inst->forwardFlag_rs) reg_rs = inst->rsValue;
	
	switch(inst->opcode){
	//----R mode----//
	case 0: 
		return R_mode(inst,fError);
	
	//----I mode----//
	case 8:
	//--addi--//
		overflowDetect(reg_rs,(int)inst->constant_16,fError);
		inst->result = reg_rs+(int)inst->constant_16;
		return 0;
	case 9:
	//--addiu--//
		inst->result = reg_rs+(unsigned int)inst->constant_16;
		return 0;
	case 35:
	//--lw--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 1;
		return 0;
		
	case 33:
	//--lh--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 1;
		return 0;
		
	case 37:
	//--lhu--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 1;
		return 0;
		
	case 32:
	//--lb--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 1;
		return 0;
		
	case 36:
	//--lbu--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 1;
		return 0;
		
	case 43:
	//--sw--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 2;
		return 0;
		
	case 41:
	//--sh--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 2;
		return 0;
	
	case 40:
	//--sb--//
		overflowDetect(reg_rs,inst->constant_16,fError);
		inst->result = reg_rs+inst->constant_16;
		inst->memAccessFlag = 2;
		return 0;

	case 15:
	//--lui--//
		inst->result = inst->constant_16<<16;
		return 0;

	case 12:
	//--andi--//
		inst->result = reg_rs&(inst->constant_16&0x0000FFFF);
		return 0;

	case 13:
	//--ori--//
		inst->result = reg_rs|(inst->constant_16&0x0000FFFF);
		return 0;

	case 14:
	//--nori--//
		inst->result = ~(reg_rs|(inst->constant_16&0x0000FFFF));
		return 0;

	case 10:
	//--slti--//
		if((int)reg_rs<(int)inst->constant_16) inst->result = 1;
		else inst->result = 0;
		return 0;

	case 4:
	//--beq--//
		//if(reg_rs==reg_rt) setPc(getPc()+4*inst->constant_16);
		return 0;

	case 5:
	//--bne--//
		//if(reg_rs!=reg_rt) setPc(getPc()+4*inst->constant_16);
		return 0;
	
	case 7:
	//--bgtz--//
		//if((int)reg_rs>0) setPc(getPc()+4*inst->constant_16);
		return 0;

//----J mode----//
	case 2:
	//--j--//
		//setPc( (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26);
		return 0;

	case 3:
	//--jal--//
		// setReg(31,getPc());	//pc should not +4 here... or when call 'jr' $ra
		// 					//pc will +4 again.
		// setPc( (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26);
		return 0;
//--S mode--//
	case 63:
		return FINISH_CODE;
	default :
		printf("[x]Instruction not found. op-code = 0x%02X\n",inst->opcode);
		return ERROR_CODE;
	}
}

//--for beq/bne/jr/j/jal at execute ID stage--//
int jumpDetect(instruction *inst){
	if(inst->stallFlag) return 0;
	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);
	if(inst->forwardFlag_rs) reg_rs = inst->rsValue; 
	if(inst->forwardFlag_rt) reg_rt = inst->rtValue; 
	
	//--another forwarding--//
	//	because jump detection have to be done before 
	//	WB stage, so need to do some forwarding WB->ID,
	//	but if there are MEM forwarding , dont do this.
	instruction *WB = getWB();
	int wbReg;
	if(WB->wbFlag==1) wbReg = WB->rd;
	else if(WB->wbFlag==2)	wbReg = WB->rt;
	else wbReg = -1;
	if(wbReg==0) wbReg = -1;
	if(inst->forwardFlag_rs==0&&wbReg==inst->rs) reg_rs = WB->result;
	if(inst->forwardFlag_rt==0&&wbReg==inst->rt) reg_rt = WB->result;
	
	
	
	switch(inst->opcode){
		case 0:	//jr
			if(inst->func==0x08){
				inst->nextPc = reg_rs;
			}else return 0;
			break;
		case 4: //beq
			if(reg_rs==reg_rt)
				inst->nextPc = getPc()+4*inst->constant_16;
			break;
			
		case 5: //bne
			if(reg_rs!=reg_rt)
				inst->nextPc = getPc()+4*inst->constant_16;
			
			break;
			
		case 7: //bgtz
			if((int)reg_rs>0)
				inst->nextPc = getPc()+4*inst->constant_16;
			break;
		case 2: //j
			inst->nextPc = (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26;
			break;
		case 3: //jal
			inst->result = getPc();
			inst->rt = 31;
			inst->nextPc = (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26;
			break;
		default: return 0;
	}
	if(inst->nextPc!=-1) return 1;
	else return 0;
}