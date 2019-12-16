#include "errorReport.h"

int calcLen(int key){
    if(key==0x23||key==0x2B) return 4;                  //lw or sw
    else if(key==0x21||key==0x25||key==0x29) return 2;  //lh or lhu or sh 
    else if(key==0x20||key==0x24||key==0x28)return 1;   //lb or lbu or sb
    else return 0;                                      //others
}

void write$0Error(instruction *inst ,FILE *fError){
    int wbReg;
    if(inst->wbFlag==1) wbReg = inst->rd;
    else if(inst->wbFlag==2) wbReg = inst->rt;
    else return;
    if(wbReg==0) fprintf(fError,"In cycle %d: Write $0 Error\n",getCycleCounter());
}

void overflowDetect(int right_val_1,int right_val_2 ,FILE *fError){
	if(((unsigned int)right_val_1>>31)==((unsigned int)right_val_2>>31) &&
	(((unsigned int)right_val_1+(unsigned int)right_val_2)>>31)	!=
											((unsigned int)right_val_1>>31))
		fprintf(fError,"In cycle %d: Number Overflow\n",getCycleCounter());
}

int accessOverflowDetect(instruction *inst ,FILE *fError){
    int len = calcLen(inst->opcode);
    
    if(len==0) return 0; //lb or lbu or sb
    if(inst->result+len-1>=1024||inst->result<0||inst->result+len-1<0){
		fprintf(fError,"In cycle %d: Address Overflow\n",getCycleCounter());
        
        return 1;
    }
    return 0;
}

int misalignmentDetect(instruction *inst ,FILE *fError){
    int len = calcLen(inst->opcode);
    
    if(len==0) return 0; //lb or lbu or sb
    if((inst->result)%len!=0){
		fprintf(fError,"In cycle %d: Misalignment Error\n",getCycleCounter());
		
		return 1;
	}
	return 0;
}


