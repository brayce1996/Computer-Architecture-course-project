#include<stdio.h>
#include"instruction.h"
#include"decoder.h"
#include"execute.h"

#define CYCLE_LIMIT 500000

#define ERROR_CODE -1
#define FINISH_CODE 1


int Register[32];	//Register should be signed integer,
					//it's better for calculation.
					//so, when load word from memory to register
					//you should put (int) before you read in.
unsigned int pc,sp;
unsigned int IMemSize,DMemSize;
unsigned char I_memory[1024];
unsigned char D_memory[1024];

int cycle_counter;

//--This function initialize all variable--//
void init()
{
	int i;
	for(i=0;i<32;i++) Register[i] = 0;
	for(i=0;i<1024;i++){
		I_memory[i] = 0;
		D_memory[i] = 0;
	}
	pc = 0;
	sp = 0;

}

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

int main()
{
	//--varialbe declare--//
	int i = 0; 
	FILE *fptr;
	
	//--initialize--//
	init();
	
	//--read instructions and pc in--//
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
	
	sp = read_4_bytes(fptr);
	Register[29] = sp;
	DMemSize = read_4_bytes(fptr);
	
	if(DMemSize>1024/4){	// 1024/4 : byte -> word
		puts("[x]D memory : over the bound.");
		//DMemSize = 1024/4;
	}
	for(i=0;i<DMemSize*4;i++)
		fread(&D_memory[i],1,1,fptr);
	
	fclose(fptr);
	puts("[v] Read D memory successfully.");
	
	//--init output pointer--//
	FILE *fSnapshot;
	fSnapshot = fopen("./snapshot.rpt","w");
	if(fSnapshot==NULL){
		puts("[x]Fail to open \"snapshot.rpt\" file.");
		return 1;
	}
	FILE *fError;
	fError = fopen("./error_dump.rpt","w");
	if(fError==NULL){
		puts("[x]Fail to open \"error_dump.rpt\" file.");
		fclose(fSnapshot);
		return 1;
	}
	
	//--start to execute--//
	cycle_counter = 0;
	while(cycle_counter < CYCLE_LIMIT){
		//--print output--//
		fprintf(fSnapshot,"cycle %d\n",cycle_counter);
		for(i=0;i<32;i++)
			fprintf(fSnapshot,"$%02d: 0x%08X\n",i,Register[i]);
		fprintf(fSnapshot,"PC: 0x%08X\n\n\n",pc);
		
		//--decode instruction--//
		instruction inst;
		decode(I_memory,pc,&inst);
		
		// printf("cycle %d: op:0x%02d rs:%d rt:%d rd:%d sh:%d fun:0x%02d\n",
		// 			cycle_counter,inst.opcode,inst.rs,inst.rt,inst.rd,inst.shamt,inst.func);
		
		//--add up pc--//
		pc += 4;
		
		//--cycle counter add up--//
		cycle_counter++;
		
		//--execute--//
		int message = execute(&inst,fError);
		
		//--check returned message--//
		if(message == ERROR_CODE){
			puts("[x]Terminate with ERROR.");
			break;
		}
		if(message == FINISH_CODE) {
			printf("[v]Finish.\n");
			break;
		}
		
	}
	
	fclose(fSnapshot);
	fclose(fError);
	
	if(cycle_counter>CYCLE_LIMIT) puts("[x]Over 500000 cycle.");
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

