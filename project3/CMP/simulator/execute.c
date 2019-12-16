#include<stdio.h>
#include"instruction.h"
#include"execute.h"

#define ERROR_CODE -1
#define FINISH_CODE 1



int R_mode(instruction *inst){

	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);
	int tmp;

	switch(inst->func){
	case 32:
	//--add--//
		setReg(inst->rd,reg_rs + reg_rt);
		
		return 0;
	case 33:
	//--addu--//
		setReg(inst->rd,reg_rs + reg_rt);
		return 0;
	case 34:
	//--sub--//
		reg_rt = reg_rt*-1;
		setReg(inst->rd,reg_rs + reg_rt);
		return 0;
		
	case 36:
	//--and--//
		setReg(inst->rd,reg_rs & reg_rt);
		return 0;

	case 37:
	//--or--//
		setReg(inst->rd,reg_rs | reg_rt);
		return 0;

	case 38:
	//--xor--//
		setReg(inst->rd,reg_rs ^ reg_rt);
		return 0;

	case 39:
	//--nor--//
		setReg(inst->rd,~(reg_rs | reg_rt));
		return 0;
	
	case 40:
	//--nand--//
		setReg(inst->rd,~(reg_rs & reg_rt));
		return 0;
	
	case 42:
	//--slt--//
		setReg(inst->rd,((signed int)reg_rs < (signed int)reg_rt)); 
		return 0;
	
	case 0:
	//--sll--//
		setReg(inst->rd,reg_rt<<(inst->shamt));
		return 0;

	case 2:
	//--srl--//
		setReg(inst->rd,reg_rt>>(inst->shamt));
		return 0;
	
	case 3:
	//--sra--//
		setReg(inst->rd,(int)reg_rt>>(inst->shamt));
		return 0;
	case 8:
	//--jr--//
		setPc(reg_rs);
		return 0;
	default:
		printf("[x]R mode func not found. func = 0x%02x\n",inst->func);
		return ERROR_CODE; //interrupt
	}
}

int execute(instruction *inst){
	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);
	
	int i;
	unsigned int un_tmp;
	int tmp;
	switch(inst->opcode){
	//----R mode----//
	case 0: 
		return R_mode(inst);
	
	//----I mode----//
	case 8:
	//--addi--//
		setReg(inst->rt,reg_rs+(int)inst->constant_16);
		return 0;
	case 9:
	//--addiu--//
		setReg(inst->rt,reg_rs+(unsigned int)inst->constant_16);
		return 0;
	case 35:
	//--lw--//
		un_tmp = 0;
		for(i=0;i<4;i++){ //read 4 bytes
			un_tmp |= getDDisk(reg_rs+inst->constant_16+i);
			if(i<3)un_tmp = un_tmp<<8;
		}
		setReg(inst->rt,un_tmp);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
	case 33:
	//--lh--//
		tmp = (char)getDDisk(reg_rs+inst->constant_16);	
		tmp = tmp<<8;
		tmp = tmp | getDDisk(reg_rs+inst->constant_16+1); 
		setReg(inst->rt,tmp);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
	case 37:
	//--lhu--//
		tmp = getDDisk(reg_rs+inst->constant_16);
		tmp = tmp<<8;
		tmp |= getDDisk(reg_rs+inst->constant_16+1);
		setReg(inst->rt,tmp);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
	case 32:
	//--lb--//
		tmp = (char)getDDisk(reg_rs+inst->constant_16);
		setReg(inst->rt,tmp);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
	case 36:
	//--lbu--//
		tmp = getDDisk(reg_rs+inst->constant_16);
		setReg(inst->rt,tmp);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
		
	case 43:
	//--sw--//
		setDDisk(reg_rs+inst->constant_16,(reg_rt&0xFF000000)>>24);
		setDDisk(reg_rs+inst->constant_16+1,(reg_rt&0x00FF0000)>>16);
		setDDisk(reg_rs+inst->constant_16+2,(reg_rt&0x0000FF00)>>8);
		setDDisk(reg_rs+inst->constant_16+3,reg_rt&0x000000FF);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;

	case 41:
	//--sh--//
		setDDisk(reg_rs+inst->constant_16,(reg_rt&0x0000FF00)>>8);
		setDDisk(reg_rs+inst->constant_16+1,reg_rt&0x000000FF);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;
	
	case 40:
	//--sb--//
		setDDisk(reg_rs+inst->constant_16,reg_rt&0x000000FF);
		countMissHit_D(reg_rs+inst->constant_16);
		return 0;

	case 15:
	//--lui--//
		setReg(inst->rt,inst->constant_16<<16);
		return 0;

	case 12:
	//--andi--//
		setReg(inst->rt,reg_rs&(inst->constant_16&0x0000FFFF));
		return 0;

	case 13:
	//--ori--//
		setReg(inst->rt,reg_rs|(inst->constant_16&0x0000FFFF));
		return 0;

	case 14:
	//--nori--//
		setReg(inst->rt,~(reg_rs|(inst->constant_16&0x0000FFFF)));
		return 0;

	case 10:
	//--slti--//
		if((int)reg_rs<(int)inst->constant_16) setReg(inst->rt,1);
		else setReg(inst->rt,0);
		return 0;

	case 4:
	//--beq--//
		if(reg_rs==reg_rt) setPc(getPc()+4*inst->constant_16);
		return 0;

	case 5:
	//--bne--//
		if(reg_rs!=reg_rt) setPc(getPc()+4*inst->constant_16);
		return 0;
	
	case 7:
	//--bgtz--//
		if((int)reg_rs>0) setPc(getPc()+4*inst->constant_16);
		return 0;

//----J mode----//
	case 2:
	//--j--//
		setPc( (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26);
		return 0;

	case 3:
	//--jal--//
		setReg(31,getPc());	//pc should not +4 here... or when call 'jr' $ra
							//pc will +4 again.
		setPc( (getPc()&0xF0000000) | 4*(unsigned int)inst->constant_26);
		return 0;
//--S mode--//
	case 63:
		return FINISH_CODE;
	default :
		printf("[x]Instruction not found. op-code = 0x%02X\n",inst->opcode);
		return ERROR_CODE;
	}
}

