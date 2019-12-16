#include <stdio.h>
#include "instruction.h"
#include "decoder.h"
#include "execute.h"
#include "TLB.h"
#include "cache.h"
#include "pageTable.h"
#include "memory.h"


#define CYCLE_LIMIT 500000
#define SIZE 1024

#define ERROR_CODE -1
#define FINISH_CODE 1


int Register[32];
unsigned int pc,sp;
unsigned int IDiskSize,DDiskSize;
unsigned char I_disk[SIZE];
unsigned char D_disk[SIZE];

int cycle_counter;
int getCycleCounter(){return cycle_counter;}

unsigned int I_memory_size;
unsigned int I_page_size;
unsigned int I_cache_size;
unsigned int I_block_size;
unsigned int I_set_associativity;

unsigned int D_memory_size;
unsigned int D_page_size;
unsigned int D_cache_size;
unsigned int D_block_size;
unsigned int D_set_associativity;

unsigned int I_TLB_page_num;
unsigned int D_TLB_page_num;
unsigned int I_cache_block_num;
unsigned int D_cache_block_num;
unsigned int I_page_num;
unsigned int D_page_num;

TLB I_TLB[SIZE];
TLB D_TLB[SIZE];
cache I_cache[SIZE];
cache D_cache[SIZE];
pageTable I_pageTable[SIZE];
pageTable D_pageTable[SIZE];
Memory I_memory[SIZE];
Memory D_memory[SIZE];

unsigned int I_pageTable_hit_counter;
unsigned int I_cache_hit_counter;
unsigned int I_TLB_hit_counter;

unsigned int I_pageTable_miss_counter;
unsigned int I_cache_miss_counter;
unsigned int I_TLB_miss_counter;

unsigned int D_pageTable_hit_counter;
unsigned int D_cache_hit_counter;
unsigned int D_TLB_hit_counter;

unsigned int D_pageTable_miss_counter;
unsigned int D_cache_miss_counter;
unsigned int D_TLB_miss_counter;



FILE *fSnapshot;
FILE *fReport;


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

int min(int a,int b){
	if(a>b) return b;
	else return a;
}

//--This function initialize all variable--//
int init(int argc,char **argv)
{
	int i;
	for(i=0;i<32;i++) Register[i] = 0;
	for(i=0;i<1024;i++){
		I_disk[i] = 0;
		D_disk[i] = 0;
	}
	pc = 0;
	sp = 0;
	
	//--read instructions and pc in--//
	FILE *fptr;
	
	fptr = fopen("./iimage.bin","rb");
	if(fptr==NULL){
		puts("[x]Fail to read \"iimage.bin\" in.");
		return 1;
	}
	
	pc = read_4_bytes(fptr);
	IDiskSize = read_4_bytes(fptr);
	if(pc>1023){
		puts("[x] Illegal I image, pc is over 1K");
		return 1;
	}
	if(IDiskSize > (1024-pc)/4){
		puts("[x]I disk : over the bound.");
		IDiskSize = (1024 - pc)/4;
	}
	
	for(i=pc;i<IDiskSize*4+pc;i++){
		fread(&I_disk[i],1,1,fptr);
	}
	fclose(fptr);
	puts("[v] Read I disk successfully.");
	
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
		DDiskSize = read_4_bytes(fptr);
		
		if(DDiskSize>1024/4){	// 1024/4 : byte -> word
			puts("[x]D disk : over the bound.");
			DDiskSize = 1024/4;
		}
		for(i=0;i<DDiskSize*4;i++)
			fread(&D_disk[i],1,1,fptr);
	}else puts("[!] Dimage.bin is empty!");
	fclose(fptr);
	puts("[v] Read D disk successfully.");
	
	//--init output pointer--//
	fSnapshot = fopen("./snapshot.rpt","w");
	if(fSnapshot==NULL){
		puts("[x]Fail to open \"snapshot.rpt\" file.");
		return 1;
	}
	
	
	fReport = fopen("./report.rpt","w");
	if(fReport==NULL){
		puts("[x]Fail to open \"report.rpt\" file.");
		fclose(fSnapshot);
		return 1;
	}
	
	
	//--set argument value--//
	if(argc!=11){
		// default mode
		puts("[v] Argument : default mode.");
		I_memory_size = 64;
		D_memory_size = 32;
		I_page_size = 8;
		D_page_size = 16;
		I_cache_size = 16;
		I_block_size = 4;
		I_set_associativity = 4;
		D_cache_size = 16;
		D_block_size = 4;
		D_set_associativity = 1;
	}else{
		// configure mode
		puts("[v] Argument : configure mode.");
		I_memory_size = atoi(argv[1]);
		D_memory_size = atoi(argv[2]);
		I_page_size = atoi(argv[3]);
		D_page_size = atoi(argv[4]);
		I_cache_size = atoi(argv[5]);
		I_block_size = atoi(argv[6]);
		I_set_associativity = atoi(argv[7]);
		D_cache_size = atoi(argv[8]);
		D_block_size = atoi(argv[9]);
		D_set_associativity = atoi(argv[10]);
	}
	
	//--calculate number of structure--//
	I_page_num = SIZE/I_page_size;
	D_page_num = SIZE/D_page_size;
	I_TLB_page_num = min(I_page_num/4,I_memory_size/I_page_size);
	D_TLB_page_num = min(D_page_num/4,D_memory_size/D_page_size);
	I_cache_block_num = I_cache_size/I_block_size;
	D_cache_block_num = D_cache_size/D_block_size;
	
	//--init strcuture--//
	initPageTable(I_pageTable,I_page_num);
	initPageTable(D_pageTable,D_page_num);
	
	initTLB(I_TLB,I_TLB_page_num);
	initTLB(D_TLB,D_TLB_page_num);
	
	initCache(I_cache,I_cache_block_num,I_set_associativity);
	initCache(D_cache,D_cache_block_num,D_set_associativity);
	
	initMemory(I_memory,I_memory_size/I_page_size);
	initMemory(D_memory,D_memory_size/D_page_size);
	
	//--init counter--//
	I_TLB_hit_counter = 0;
	I_cache_hit_counter = 0;
	I_pageTable_hit_counter = 0;
	I_TLB_miss_counter = 0;
	I_cache_miss_counter = 0;
	I_pageTable_miss_counter = 0;
	
	D_TLB_hit_counter = 0;
	D_cache_hit_counter = 0;
	D_pageTable_hit_counter = 0;
	D_TLB_miss_counter = 0;
	D_cache_miss_counter = 0;
	D_pageTable_miss_counter = 0;
	
	
	
	return 0;
}

void printIMRU(){
	int i;
	printf("Cycle : %d\n\t",getCycleCounter());
	for(i=0;i<4;i++){
		printf("%d ",I_cache[0].assoSet[i].MRU);
	}
	printf("\n\t");
	for(i=0;i<4;i++){
		printf("%d ",I_cache[0].assoSet[i].tag+1);
	}
	printf("\n\n");
}

void printDMRU(){
	int i;
	printf("Cycle : %d\nvaild\t",getCycleCounter());
	for(i=0;i<4;i++){
		printf("%2d ",D_cache[0].assoSet[i].vaild);
	}
	printf("\nMRU  \t");
	for(i=0;i<4;i++){
		printf("%2d ",D_cache[0].assoSet[i].MRU);
	}
	printf("\nTag  \t");
	for(i=0;i<4;i++){
		printf("%2d ",D_cache[0].assoSet[i].tag);
	}
	printf("\nhit = %d\nmiss = %d\n\n",D_cache_hit_counter,D_cache_miss_counter);
	
}
void printITLB(){
	int i;
	printf("Cycle : %d\n",getCycleCounter());
	printf("index\tvaild\tPPN\tTag\tlastUse\n");
	for(i=0;i<I_TLB_page_num;i++){
		printf("%5d\t%5d\t%3d\t%3d\t%7d\n",i,I_TLB[i].vaild,I_TLB[i].PPN,I_TLB[i].tag,I_TLB[i].lastUse);
	}
	printf("\n");
}

void printDTLB(){
	int i;
	printf("Cycle : %d\n",getCycleCounter());
	printf("hit:%d\nmiss:%d\n",D_TLB_hit_counter,D_TLB_miss_counter);
	printf("index\tvaild\tPPN\tTag\tlastUse\n");
	for(i=0;i<D_TLB_page_num;i++){
		printf("%5d\t%5d\t%3d\t%3d\t%7d\n",i,D_TLB[i].vaild,D_TLB[i].PPN,D_TLB[i].tag,D_TLB[i].lastUse);
	}
	printf("\n");
}

void printDMEM(){
	printf("D memory at %d cycle\n",getCycleCounter());
	int i;
	int memPageNum = D_memory_size/D_page_size;
	printf("index\tvaild\tlastUse\n");
	for(i=0;i<memPageNum;i++){
		printf("%5d\t%5d\t%7d\n",i,D_memory[i].vaild,D_memory[i].lastUse);
	}
	printf("\n\n");
}

void countMissHit_D(unsigned int virAddr){
	//--search TLB--//
	int ppn;
	unsigned int phyAddr;
	ppn=searchTLB(D_TLB,virAddr,D_page_size,D_TLB_page_num);
	if(ppn>=0){
		// tlb hit
		D_TLB_hit_counter++;
		
		//--search cache--//
		phyAddr = ppn*D_page_size | virAddr%D_page_size;
		if(inCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity)){
			// cache hit
			D_cache_hit_counter++;
			
		}else {
			// cache miss
			D_cache_miss_counter++;
			replaceCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity);
			//update memory entry last use time
			//I_memory[ppn].lastUse = getCycleCounter();
		}
	}else{
		// tlb miss
		D_TLB_miss_counter++;
		//--search page table--//
		if(inPageTable(D_pageTable,virAddr,D_page_size)){
			// pagetalbe hit
			D_pageTable_hit_counter++;
			
			replaceTLB(D_TLB , D_pageTable[virAddr/D_page_size].PPN , virAddr/D_page_size , D_TLB_page_num);
			ppn = searchTLB(D_TLB,virAddr,D_page_size,D_TLB_page_num);
			
			// update memory Last use
			D_memory[ppn].lastUse = getCycleCounter();
			
			phyAddr = ppn*D_page_size | virAddr%D_page_size;
			if(inCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity)){
				// cache hit
				D_cache_hit_counter++;
				
			}else {
				// cache miss
				replaceCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity);
				//update memory entry last use time
				//I_memory[ppn].lastUse = getCycleCounter();
				D_cache_miss_counter++;
			}
			
		}else{
			// pagetable miss(page fault)
			D_pageTable_miss_counter++;
			D_cache_miss_counter++;
			
			
			unsigned int swapOutPPN = replaceMem(D_memory,D_memory_size/D_page_size,D_page_size,D_cache,virAddr,D_cache_size,D_block_size,D_set_associativity);
			
			//updateCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity);
			
			// update page table
			updatePageTable(D_pageTable,swapOutPPN,virAddr/D_page_size,D_page_num);
			
			replaceTLB(D_TLB,swapOutPPN,virAddr/D_page_size,D_TLB_page_num);
			
			phyAddr = swapOutPPN*D_page_size | virAddr%D_page_size;
			replaceCache(D_cache,phyAddr,D_cache_size,D_block_size,D_set_associativity);
		}
	}
	//printDMRU();
	//printDTLB();
}
void countMissHit_I(unsigned int virAddr){
	//--search TLB--//
	int ppn;
	unsigned int phyAddr;
	ppn=searchTLB(I_TLB,virAddr,I_page_size,I_TLB_page_num);
	if(ppn>=0){
		// tlb hit
		I_TLB_hit_counter++;
		
		//--search cache--//
		phyAddr = ppn*I_page_size | virAddr%I_page_size;
		if(inCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity)){
			// cache hit
			I_cache_hit_counter++;
			
		}else {
			// cache miss
			I_cache_miss_counter++;
			replaceCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity);
			//update memory entry last use time
			//I_memory[ppn].lastUse = getCycleCounter();
		}
	}else{
		// tlb miss
		I_TLB_miss_counter++;
		//--search page table--//
		if(inPageTable(I_pageTable,virAddr,I_page_size)){
			// pagetalbe hit
			I_pageTable_hit_counter++;
			
			replaceTLB(I_TLB , I_pageTable[virAddr/I_page_size].PPN , virAddr/I_page_size , I_TLB_page_num);
			ppn = searchTLB(I_TLB,virAddr,I_page_size,I_TLB_page_num);
			
			// update memory Last use
			I_memory[ppn].lastUse = getCycleCounter();
			
			phyAddr = ppn*I_page_size | virAddr%I_page_size;
			if(inCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity)){
				// cache hit
				I_cache_hit_counter++;
				
			}else {
				// cache miss
				replaceCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity);
				//update memory entry last use time
				//I_memory[ppn].lastUse = getCycleCounter();
				I_cache_miss_counter++;
			}
			
		}else{
			// pagetable miss(page fault)
			I_pageTable_miss_counter++;
			I_cache_miss_counter++;
			
			
			unsigned int swapOutPPN = replaceMem(I_memory,I_memory_size/I_page_size,I_page_size,I_cache,virAddr,I_cache_size,I_block_size,I_set_associativity);
			
			//updateCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity);
			
			// update page table
			updatePageTable(I_pageTable,swapOutPPN,virAddr/I_page_size,I_page_num);
			
			replaceTLB(I_TLB,swapOutPPN,virAddr/I_page_size,I_TLB_page_num);
			
			phyAddr = swapOutPPN*I_page_size | virAddr%I_page_size;
			replaceCache(I_cache,phyAddr,I_cache_size,I_block_size,I_set_associativity);
		}
	}
	
}


int main(int argc , char ** argv)
{
	//--varialbe declare--//
	int i = 0;
	
	//--initialize--//
	if(init(argc,argv)) return 1;
	
	
	//--start to execute--//
	cycle_counter = 0;
	while(cycle_counter <= CYCLE_LIMIT){
		//--print output--//
		fprintf(fSnapshot,"cycle %d\n",cycle_counter);
		for(i=0;i<32;i++)
			fprintf(fSnapshot,"$%02d: 0x%08X\n",i,Register[i]);
		fprintf(fSnapshot,"PC: 0x%08X\n\n\n",pc);
		
		//--count miss/hit--//
		countMissHit_I(pc);
		//printMRU();
		//printTLB();
		//printDMEM();
		
		//--decode instruction--//
		instruction inst;
		decode(I_disk,pc,&inst);
		
		//--add up pc--//
		pc += 4;
		
		//--cycle counter add up--//
		cycle_counter++;
		
		//--execute--//
		int message = execute(&inst);
		
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
	
	fprintf( fReport, "ICache :\n");
	fprintf( fReport, "# hits: %u\n", I_cache_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", I_cache_miss_counter );
	fprintf( fReport, "DCache :\n");
	fprintf( fReport, "# hits: %u\n", D_cache_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", D_cache_miss_counter );
	fprintf( fReport, "ITLB :\n");
	fprintf( fReport, "# hits: %u\n", I_TLB_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", I_TLB_miss_counter );
	fprintf( fReport, "DTLB :\n");
	fprintf( fReport, "# hits: %u\n", D_TLB_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", D_TLB_miss_counter );
	fprintf( fReport, "IPageTable :\n");
	fprintf( fReport, "# hits: %u\n", I_pageTable_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", I_pageTable_miss_counter );
	fprintf( fReport, "DPageTable :\n");
	fprintf( fReport, "# hits: %u\n", D_pageTable_hit_counter );
	fprintf( fReport, "# misses: %u\n\n", D_pageTable_miss_counter );
	
	
	fclose(fSnapshot);
	fclose(fReport);
	
	if(cycle_counter>CYCLE_LIMIT) puts("[x]Over 500000 cycle.");
	return 0;
}

/**
*	Get function
*
*/


unsigned int getIDiskSize(){return IDiskSize;}

unsigned int getDDiskSize(){return DDiskSize;}

unsigned int getDDisk(unsigned int index){
	if(index>=1024||index<0) {
		printf("[x] Load D memory fail : out of bound(%d) at %d cycle.\n",index,getCycleCounter());
		return 1; 
	}
	return D_disk[index];
}

unsigned int getPc(){return pc;}

unsigned int getReg(unsigned int index){
	if(index>=32||index<0) return ERROR_CODE;
	return Register[index];
}

unsigned int getIMemSize(){return I_memory_size;}
unsigned int getIPageSize(){return I_page_size;}
unsigned int getICacheSize(){return I_cache_size;}
unsigned int getIBlockSize(){return I_block_size;}
unsigned int getISetAss(){return I_set_associativity;}
unsigned int getITlbPageNum(){return I_TLB_page_num;}
unsigned int getICacheBlockNum(){return I_cache_block_num;}
unsigned int getIPageNum(){return I_page_num;}

unsigned int getDMemSize(){return D_memory_size;}
unsigned int getDPageSize(){return D_page_size;}
unsigned int getDCacheSize(){return D_cache_size;}
unsigned int getDBlockSize(){return D_block_size;}
unsigned int getDSetAss(){return D_set_associativity;}
unsigned int getDTlbPageNum(){return D_TLB_page_num;}
unsigned int getDCacheBlockNum(){return D_cache_block_num;}
unsigned int getDPageNum(){return D_page_num;}

/**
*	Set funciton
*
*
*/
int setDDisk(unsigned int index,unsigned char data){
	if(index>=1024||index<0) {
		puts("[x] Store D memory fail : out of bound.");
		return 1;
	}
	D_disk[index] = data;
	return 0;	
}

int setPc(unsigned int new_pc){
	pc = new_pc;
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

