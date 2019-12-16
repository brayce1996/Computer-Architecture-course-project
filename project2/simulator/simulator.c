#include<stdio.h>
#include<stdlib.h>
#include"instruction.h"
#include"decoder.h"
#include"execute.h"
#include"hazard.h"
#include"mem.h"
#include"errorReport.h"
#include "reverse.h"

#define CYCLE_LIMIT 500000

#define ERROR_CODE -1
#define FINISH_CODE 1
#define NOP 34

int Register[32];	//Register should be signed integer,
					//it's better for calculation.
					//so, when load word from memory to register
					//you should put (int) before you read in.
unsigned int pc,sp;
unsigned int IMemSize,DMemSize;
unsigned char I_memory[1024];
unsigned char D_memory[1024];

int IF;
instruction *ID;
instruction *EX;
instruction *MEM;
instruction *WB;

char * INST[] = {
    "ADD"   ,   "ADDU"  ,   "SUB"   ,   "AND"   ,   "OR",
    "XOR"   ,   "NOR"   ,   "NAND"  ,   "SLT"   ,   "SLL",
    "SRL"   ,   "SRA"   ,   "JR"    ,   "ADDI"  ,   "ADDIU",
    "LW"    ,   "LH"    ,   "LHU"   ,   "LB"    ,   "LBU",
    "SW"    ,   "SH"    ,   "SB"    ,   "LUI"   ,   "ANDI",
    "ORI"   ,   "NORI"  ,   "SLTI"  ,   "BEQ"   ,   "BNE",
    "BGTZ"  ,   "J"     ,   "JAL"   ,   "HALT" 	,	"NOP"
};

int OpMap[0x40];

int FuncMap[0x30]; //for R mode

FILE *fSnapshot;
FILE *fError;

int cycle_counter;

//--function declare--//
unsigned int getDMem(unsigned int index);
int setDMem(unsigned int index,unsigned char data);
unsigned int getReg(unsigned int index);
int setReg(unsigned int index,unsigned int value);


//--translate from little endian to big endian--//
int read_4_bytes(FILE *fptr){
	unsigned char buf[4];// 4 * 8 bytes
	int i=0;
	int x = 0;	
	fread(buf,1,4,fptr);
	for(i=0;i<4;i++){	
		x = x<<8;
		x += buf[i];
	}
	return x;
}

//--This function initialize all variable--//
int init()
{
	//--initial variable--//
	int i;
	for(i=0;i<32;i++) Register[i] = 0;
	for(i=0;i<1024;i++){
		I_memory[i] = 0;
		D_memory[i] = 0;
	}
	pc = 0;
	sp = 0;
	
	IF = 0;
	ID = (instruction*)malloc(sizeof(instruction));
	initInst(ID);
	ID->nopFlag = 1;
	EX = (instruction*)malloc(sizeof(instruction));
	initInst(EX);
	EX->nopFlag = 1;
	MEM = (instruction*)malloc(sizeof(instruction));
	initInst(MEM);
	MEM->nopFlag = 1;
	WB = (instruction*)malloc(sizeof(instruction));
	initInst(WB);
	WB->nopFlag = 1;
	
	//--read instructions and pc in--//
	FILE *fptr;
	fptr = fopen("./iimage.bin","rb");
	if(fptr==NULL){
		puts("[x]Fail to read \"iimage.bin\" in.");
		return 1;
	}
	
	pc = read_4_bytes(fptr);
	IMemSize = read_4_bytes(fptr);
	if(pc>1023){
		puts("[x] Illegal I image, pc is over 1K");
		return 1;
	}
	if(IMemSize > (1024-pc)/4){
		puts("[x]I memory : over the bound.");
		IMemSize = (1024 - pc)/4;
	}
	
	for(i=pc;i<IMemSize*4+pc;i++){
		fread(&I_memory[i],1,1,fptr);
	}
	fclose(fptr);
	puts("[v] Read I memory successfully.");
	
	//--read data and sp in--//
	fptr = fopen("./dimage.bin","rb");
	if(fptr==NULL){
		puts("[x]Fail to read \"dimage.bin\" in.");
		return 1;
	}
    fseek(fptr, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(fptr);
	if(len>0){
		rewind(fptr);
		sp = read_4_bytes(fptr);
		Register[29] = sp;
		DMemSize = read_4_bytes(fptr);
		
		if(DMemSize>1024/4){	// 1024/4 : byte -> word
			puts("[x]D memory : over the bound.");
			DMemSize = 1024/4;
		}
		for(i=0;i<DMemSize*4;i++)
			fread(&D_memory[i],1,1,fptr);
	}else puts("[!] Dimage.bin is empty!");
	fclose(fptr);
	puts("[v] Read D memory successfully.");
	
	//--init output pointer--//
	fSnapshot = fopen("./snapshot.rpt","w");
	if(fSnapshot==NULL){
		puts("[x]Fail to open \"snapshot.rpt\" file.");
		return 1;
	}
	fError = fopen("./error_dump.rpt","w");
	if(fError==NULL){
		puts("[x]Fail to open \"error_dump.rpt\" file.");
		fclose(fSnapshot);
		return 1;
	}
	
	//--init maping list--//
	for(i=0;i<0x40;i++) OpMap[i] = 0;
	OpMap[0] = -1;
	OpMap[0x08] = 13;
	OpMap[0x09] = 14;
	OpMap[0x23] = 15;
	OpMap[0x21] = 16;
	OpMap[0x25] = 17;
	OpMap[0x20] = 18;
	OpMap[0x24] = 19;
	OpMap[0x2B] = 20;
	OpMap[0x29] = 21;
	OpMap[0x28] = 22;
	OpMap[0x0F] = 23;
	OpMap[0x0C] = 24;
	OpMap[0x0D] = 25;
	OpMap[0x0E] = 26;
	OpMap[0x0A] = 27;
	OpMap[0x04] = 28;
	OpMap[0x05] = 29;
	OpMap[0x07] = 30;
	OpMap[0x02] = 31;
	OpMap[0x03] = 32;
	OpMap[0x3F] = 33;
	
	for(i=0;i<0x30;i++) FuncMap[i] = 0;
	FuncMap[0x20] = 0;
	FuncMap[0x21] = 1;
	FuncMap[0x22] = 2;
	FuncMap[0x24] = 3;
	FuncMap[0x25] = 4;
	FuncMap[0x26] = 5;
	FuncMap[0x27] = 6;
	FuncMap[0x28] = 7;
	FuncMap[0x2A] = 8;
	FuncMap[0x00] = 9;
	FuncMap[0x02] = 10;
	FuncMap[0x03] = 11;
	FuncMap[0x08] = 12;
	
	
	return 0;
}

char * getOpName(instruction *inst){
	int key;
	if(inst->nopFlag) key = NOP;
	else{
		key = OpMap[inst->opcode];
		if(key==-1) key = FuncMap[inst->func]; //R mode
	}
	return INST[key];
}

int main()
{
	//--varialbe declare--//
	int i = 0; 
	
	//--initialize--//
	if(init()) return 1;
	
	//reverse();
	
	//--start to execute--//
	cycle_counter = 0;
	int halt = 0;
	instruction *inst = (instruction*)malloc(sizeof(instruction));
	inst->nopFlag = 1;
	while(cycle_counter <= CYCLE_LIMIT){
		
		//--hazard detection--//
		dataHazardDetect();
		int flush = ctrlHazardDetect();
		
		//--print output--//
		fprintf(fSnapshot,"cycle %d\n",cycle_counter);
		for(i=0;i<32;i++)
			fprintf(fSnapshot,"$%02d: 0x%08X\n",i,Register[i]);
		fprintf(fSnapshot,"PC: 0x%08X\n",pc);
		
		//--TODO : report stall/forward/flush message--//
		
		fprintf(fSnapshot,"IF: 0x%02X%02X%02X%02X",I_memory[pc],I_memory[pc+1],I_memory[pc+2],I_memory[pc+3]);
		if(ID->nopFlag==0&&ID->stallFlag)	fprintf(fSnapshot," to_be_stalled");
		else if(flush) fprintf(fSnapshot," to_be_flushed");
		fprintf(fSnapshot,"\nID: %s",getOpName(ID));
		if(ID->nopFlag==0&&ID->stallFlag)	fprintf(fSnapshot," to_be_stalled");
		else{
			if(ID->nopFlag==0&&ID->forwardFlag_rs) fprintf(fSnapshot," fwd_EX-DM_rs_$%d",ID->rs);
			if(ID->nopFlag==0&&ID->forwardFlag_rt) fprintf(fSnapshot," fwd_EX-DM_rt_$%d",ID->rt);
		}
		fprintf(fSnapshot,"\nEX: %s",getOpName(EX));
		if(EX->nopFlag==0&&EX->forwardFlag_rs) fprintf(fSnapshot," fwd_EX-DM_rs_$%d",EX->rs);
		if(EX->nopFlag==0&&EX->forwardFlag_rt) fprintf(fSnapshot," fwd_EX-DM_rt_$%d",EX->rt);
		fprintf(fSnapshot,"\nDM: %s",getOpName(MEM));
		fprintf(fSnapshot,"\nWB: %s\n\n\n",getOpName(WB));
		
		
		//--cycle counter add up--//
		cycle_counter++;
		
		//--<WB>--//
		if(WB->nopFlag==0&&WB->wbFlag!=-1){
			write$0Error(WB,fError);
			if(WB->wbFlag==1)setReg(WB->rd,WB->result);		//R mode
			else if(WB->wbFlag==2)setReg(WB->rt,WB->result);//I mode
		}
		if(WB->opcode==0x3F) halt = 1; //HALT
		
		
		//--<MEM>--//
		if(MEM->nopFlag==0&&MEM->memAccessFlag!=0){
			int errorOccur = 0;
			//--check Address Overflow--//
			errorOccur = accessOverflowDetect(MEM,fError);
			
			//--check Misalignment Error--//
			errorOccur |= misalignmentDetect(MEM,fError);
			
			if(errorOccur){
				//--notify system is going to halt--//
				halt = 1;
			}else{
				//--memory access--//
				mem(MEM);
			}
		} 
		
		//--<EX>--//
		if(EX->nopFlag==0) execute(EX,fError);
		
		
		//--<ID>--//
		
		inst = (instruction*)malloc(sizeof(instruction));
		initInst(inst);
		if(ID->stallFlag==0){
			decode(I_memory,pc,inst);
			if(inst->opcode==0&&inst->rd==0&&inst->rt==0&&inst->shamt==0&&inst->func==0){
				inst->nopFlag = 1;
				inst->wbFlag = -1;
			}
			
			//--add up pc--//
			pc += 4;
			
			if(flush) {
				initInst(inst);
				pc = ID->nextPc;
				inst->nopFlag = 1;
				inst->wbFlag = -1;
			}
		}
		
		//--refresh 5-stage inst--//
		free(WB);
		WB = MEM;
		MEM = EX;
		if(ID->stallFlag==0){
			EX = ID;
			ID = inst;
		}else{
			inst->nopFlag = 1;
			inst->wbFlag = -1;
			EX = inst;
		}
		
		ID->stallFlag = 0;
		EX->forwardFlag_rs = 0;
		EX->forwardFlag_rt = 0;
		
		//--check returned message--//
		if(halt) {					//Warning : not compelete
			printf("[v]Finish.\n");
			break;
		}
		
	}
	
	fclose(fSnapshot);
	fclose(fError);
	
	if(cycle_counter>CYCLE_LIMIT&&halt!=1) puts("[x]Over 500000 cycle.");
	
	return 0;
	
}

/**
*	Get function
*
*/


unsigned int getIMemSize(){return IMemSize;}

unsigned int getDMemSize(){return DMemSize;}

unsigned int getIMem(unsigned int index){
	if(index>=1024||index<0) return ERROR_CODE; 
	return I_memory[index];
}

unsigned int getDMem(unsigned int index){
	if(index>=1024||index<0) {
		printf("[x] Load D memory fail : out of bound(%d) at %d cycle.\n",index,getCycleCounter());
		return 1; 
	}
	return D_memory[index];
}

unsigned int getPc(){return pc;}

unsigned int getSp(){return sp;}

unsigned int getReg(unsigned int index){
	if(index>=32||index<0) return ERROR_CODE;
	return Register[index];
}
int getCycleCounter(){return cycle_counter;}

char* getIMemArr(){return I_memory;}
char* getDMemArr(){return D_memory;}
int* getOpMap(){return OpMap;}
int* getFuncMap(){return FuncMap;}


instruction * getID(){return ID;}
instruction * getEX(){return EX;}
instruction * getMEM(){return MEM;}
instruction * getWB(){return WB;}

/**
*	Set funciton
*
*
*/
int setDMem(unsigned int index,unsigned char data){
	if(index>=1024||index<0) {
		puts("[x] Store D memory fail : out of bound.");
		return 1;
	}
	D_memory[index] = data;
	return 0;	
}

int setPc(unsigned int new_pc){
	pc = new_pc;
	return 0;
}

int setSp(unsigned int new_sp){
	sp = new_sp;
	return 0;
}

int setReg(unsigned int index,unsigned int value){
	if(index>32||index<0){
		printf("[x] Write register fail : out of bound(%d).",index);
		return ERROR_CODE;
	}
	if(index==0){
		return 0;
	}
	Register[index] = value;
	return 0;
}

