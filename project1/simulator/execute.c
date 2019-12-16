#include<stdio.h>
#include"instruction.h"
#include"execute.h"

#define ERROR_CODE -1
#define FINISH_CODE 1


/**
 * 		Detect error and report error, and determine whether halt or not.
 * 
 * 
 * @param	W0E_flag	if need to detect "write $0 error" ,set this flag to 1.
 * 			NO_flag		if need to detect "number overflow" ,set this flag to 1.
 * 			AO_flag		if need to detect "address overflow" ,set this flag to the length.
 * 			ME_flag		if need to detect "MISALIGNMENT ERROR",	set this flag to the length.
 * 
 * 			leftReg		the register number of the register at left hand side of '='.
 * 			right_val_1	first value	at right hand side of '='.
 * 			right_val_2	second value at right hand side of '='.
 * 
 * 			fError		the "error_dump.rpt" file pointer.
 * 
 * @return	if it's going to halt, return 1.
 * */
int errorDetect(int W0E_flag,int NO_flag,int AO_flag,int ME_flag,
				int leftReg,int right_val_1,int right_val_2 ,FILE *fError){
	int halt = 0;
	int cycle_counter = getCycleCounter();
	
	if(W0E_flag){
		if(leftReg==0) fprintf(fError,"In cycle %d: Write $0 Error\n",cycle_counter);
	}
	if(NO_flag){
		if(((unsigned int)right_val_1>>31)==((unsigned int)right_val_2>>31) &&
		(((unsigned int)right_val_1+(unsigned int)right_val_2)>>31)	!=
												((unsigned int)right_val_1>>31))
			fprintf(fError,"In cycle %d: Number Overflow\n",cycle_counter);
	}
	if(AO_flag){
		if(right_val_1+right_val_2+AO_flag-1>=1024||right_val_1+right_val_2<0||right_val_1+right_val_2+AO_flag-1<0){
			fprintf(fError,"In cycle %d: Address Overflow\n",cycle_counter);
			halt = 1;	
		}
	}
	if(ME_flag){
		if((right_val_1+right_val_2)%ME_flag!=0){
			fprintf(fError,"In cycle %d: Misalignment Error\n",cycle_counter);
			halt = 1;
		}
	}
	return halt;
}

int R_mode(instruction *inst , FILE *fError){

	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);
	int tmp;

	switch(inst->func){
	case 32:
	//--add--//
		errorDetect(1,1,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs + reg_rt);
		
		return 0;
	case 33:
	//--addu--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs + reg_rt);
		return 0;
	case 34:
	//--sub--//
		reg_rt = reg_rt*-1;
		errorDetect(1,1,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs + reg_rt);
		return 0;
		
	case 36:
	//--and--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs & reg_rt);
		return 0;

	case 37:
	//--or--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs | reg_rt);
		return 0;

	case 38:
	//--xor--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rs ^ reg_rt);
		return 0;

	case 39:
	//--nor--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,~(reg_rs | reg_rt));
		return 0;
	
	case 40:
	//--nand--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,~(reg_rs & reg_rt));
		return 0;
	
	case 42:
	//--slt--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,((signed int)reg_rs < (signed int)reg_rt)); 
		return 0;
	
	case 0:
	//--sll--//
		if(inst->rd==0){
			if(!(inst->rt==0&&inst->shamt==0))	//<--NOP instruction, don't print error
				fprintf(fError,"In cycle %d: Write $0 Error\n",getCycleCounter());
		}
		setReg(inst->rd,reg_rt<<(inst->shamt));
		return 0;

	case 2:
	//--srl--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
		setReg(inst->rd,reg_rt>>(inst->shamt));
		return 0;
	
	case 3:
	//--sra--//
		errorDetect(1,0,0,0,inst->rd,reg_rs,reg_rt,fError);
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

int execute(instruction *inst, FILE *fError){
	unsigned int reg_rs = getReg(inst->rs);
	unsigned int reg_rt = getReg(inst->rt);
	
	int i;
	unsigned int un_tmp;
	int tmp;
	int fin = 0;
	switch(inst->opcode){
	//----R mode----//
	case 0: 
		return R_mode(inst,fError);
	
	//----I mode----//
	case 8:
	//--addi--//
		errorDetect(1,1,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,reg_rs+(int)inst->constant_16);
		return 0;
	case 9:
	//--addiu--//
		errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,reg_rs+(unsigned int)inst->constant_16);
		return 0;
	case 35:
	//--lw--//
		fin = errorDetect(1,1,4,4,inst->rt,reg_rs,inst->constant_16,fError);
		
		un_tmp = 0;
		for(i=0;i<4;i++){ //read 4 bytes
			un_tmp |= getDMem(reg_rs+inst->constant_16+i);
			if(i<3)un_tmp = un_tmp<<8;
		}
		setReg(inst->rt,un_tmp);
		return fin;
	case 33:
	//--lh--//
		fin = errorDetect(1,1,2,2,inst->rt,reg_rs,inst->constant_16,fError);
		
		tmp = (char)getDMem(reg_rs+inst->constant_16);	
		tmp = tmp<<8;
		tmp = tmp | getDMem(reg_rs+inst->constant_16+1); 
		setReg(inst->rt,tmp);
		return fin;
	case 37:
	//--lhu--//
		fin = errorDetect(1,1,2,2,inst->rt,reg_rs,inst->constant_16,fError);
		tmp = getDMem(reg_rs+inst->constant_16);
		tmp = tmp<<8;
		tmp |= getDMem(reg_rs+inst->constant_16+1);
		setReg(inst->rt,tmp);
		return fin;
	case 32:
	//--lb--//
		fin = errorDetect(1,1,1,0,inst->rt,reg_rs,inst->constant_16,fError);
		tmp = (char)getDMem(reg_rs+inst->constant_16);
		setReg(inst->rt,tmp);
		return fin;
	case 36:
	//--lbu--//
		fin = errorDetect(1,1,1,0,inst->rt,reg_rs,inst->constant_16,fError);
		tmp = getDMem(reg_rs+inst->constant_16);
		setReg(inst->rt,tmp);
		return fin;
		
	case 43:
	//--sw--//
		fin = errorDetect(0,1,4,4,inst->rt,reg_rs,inst->constant_16,fError);
		setDMem(reg_rs+inst->constant_16,(reg_rt&0xFF000000)>>24);
		setDMem(reg_rs+inst->constant_16+1,(reg_rt&0x00FF0000)>>16);
		setDMem(reg_rs+inst->constant_16+2,(reg_rt&0x0000FF00)>>8);
		setDMem(reg_rs+inst->constant_16+3,reg_rt&0x000000FF);
		return fin;

	case 41:
	//--sh--//
		fin = errorDetect(0,1,2,2,inst->rt,reg_rs,inst->constant_16,fError);
		setDMem(reg_rs+inst->constant_16,(reg_rt&0x0000FF00)>>8);
		setDMem(reg_rs+inst->constant_16+1,reg_rt&0x000000FF);
		return fin;
	
	case 40:
	//--sb--//
		fin = errorDetect(0,1,1,0,inst->rt,reg_rs,inst->constant_16,fError);
		setDMem(reg_rs+inst->constant_16,reg_rt&0x000000FF);
		return fin;

	case 15:
	//--lui--//
		fin = errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,inst->constant_16<<16);
		return 0;

	case 12:
	//--andi--//
		fin = errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,reg_rs&(inst->constant_16&0x0000FFFF));
		return 0;

	case 13:
	//--ori--//
		fin = errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,reg_rs|(inst->constant_16&0x0000FFFF));
		return 0;

	case 14:
	//--nori--//
		fin = errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
		setReg(inst->rt,~(reg_rs|(inst->constant_16&0x0000FFFF)));
		return 0;

	case 10:
	//--slti--//
		fin = errorDetect(1,0,0,0,inst->rt,reg_rs,inst->constant_16,fError);
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

