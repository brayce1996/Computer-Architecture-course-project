#include "reverse.h"
char* opList[] = { 
    "add"   ,   "addu"  ,   "sub"   ,   "and"   ,   "or",
    "xor"   ,   "nor"   ,   "nand"  ,   "slt"   ,   "sll",
    "srl"   ,   "sra"   ,   "jr"    ,   "addi"  ,   "addiu",
    "lw"    ,   "lh"    ,   "lhu"   ,   "lb"    ,   "lbu",
    "sw"    ,   "sh"    ,   "sb"    ,   "lui"   ,   "andi",
    "ori"   ,   "nori"  ,   "slti"  ,   "beq"   ,   "bne",
    "bgtz"  ,   "j"     ,   "jal"   ,   "halt"
};
char* regList[] = {
    "$0",   "$at",  "$v0",  "$v1",  "$a0",
    "$a1",  "$a2",  "$a3",  "$t0",  "$t1",  
    "$t2",  "$t3",  "$t4",  "$t5",  "$t6",  
    "$t7",  "$s0",  "$s1",  "$s2",  "$s3",  
    "$s4",  "$s5",  "$s6",  "$s7",  "$t8",  
    "$t9",  "$k0",  "$k1",  "$gp",  "$sp", 
    "$fp",  "$ra"
};



void reverse(){
    int pc = getPc();
    int IMemSize = getIMemSize();
    char * I_memory = getIMemArr();
    int * OpMap = getOpMap();
    int * FuncMap = getFuncMap();
    
	FILE * outI = fopen("out.S","w");
	if(outI==NULL){
	    printf("[x] Fail to open out.S\n");
	    return;
	}
	fprintf(outI,"%d\n%d\n",pc,IMemSize);
	int i;
	instruction tmpInst;
	for(i=0;i<IMemSize&&pc<1024;i++,pc+=4){
		decode(I_memory,pc,&tmpInst);
		int index = OpMap[tmpInst.opcode];
		if(index==-1){      //R mode
		    index = FuncMap[tmpInst.func];
    		if(index<9){   //op-rs-rt-rd-x-func
    		    fprintf(outI,"%s\t\t%s\t%s\t%s\n",opList[index],regList[tmpInst.rd],regList[tmpInst.rs],regList[tmpInst.rt]);
		
    		}else if(index<12){     //op-x-rt-rd-shamt-func
    	        fprintf(outI,"%s\t\t%s\t%s\t%d\n",opList[index],regList[tmpInst.rd],regList[tmpInst.rt],tmpInst.shamt);
		
    		    
    		}else if(index==12){    //op-rs-x-x-x-func
    		    fprintf(outI,"%s\t\t%s\n",opList[index],regList[tmpInst.rs]);
    		}
		}else if(index==23){    //op-x-rt-C
		    fprintf(outI,"%s\t\t%s\t%d\n",opList[index],regList[tmpInst.rt],tmpInst.constant_16);
		
		}else if(index<28){     //op-rs-rt-C
		    fprintf(outI,"%s\t\t%s\t%s\t%d\n",opList[index],regList[tmpInst.rt],regList[tmpInst.rs],tmpInst.constant_16);
		}else if(index==28||index==29){  //op-rs-rt-C //rs<.rt
		    fprintf(outI,"%s\t\t%s\t%s\t%d\n",opList[index],regList[tmpInst.rs],regList[tmpInst.rt],tmpInst.constant_16);
		}else if(index==30){    //op-rs-x-C
		    fprintf(outI,"%s\t\t%s\t%d\n",opList[index],regList[tmpInst.rs],tmpInst.constant_16);
		}else if(index==31||index==32){ //op-C
		    fprintf(outI,"%s\t\t%d\n",opList[index],tmpInst.constant_26);
		}else if(index==33){    //op-x
		    fprintf(outI,"halt\n");
		}else printf("[x] Translate fail. op(0x%02X), func(0x%02X), index(%d) .\n",tmpInst.opcode,tmpInst.func,index);
	}
	fclose(outI);
	
	
	
	FILE * outD = fopen("out.dat","w");
    if(outD==NULL){
	    printf("[x] Fail to open out.dat\n");
	    return;
	}
    
    int sp = getSp();
    int DMemSize = getDMemSize();
    int j;
    fprintf(outD,"%d\n%d\n",sp,DMemSize);
    for(i=0;i<DMemSize*4;i+=4){
        unsigned int tmp = 0;
        for(j=0;j<4;j++){
            tmp |= getDMem(i+j);
			if(j<3)tmp = tmp<<8;
        }
        fprintf(outD,"%d\n",tmp);
    }
	fclose(outD);
}