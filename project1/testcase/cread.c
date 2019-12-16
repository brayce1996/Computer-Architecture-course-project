#include<stdio.h>

int read_4_bytes(FILE *fptr){
	unsigned char buf[4];// 4 * 8 bytes
	int i=0;
	int x = 0;	
	fread(buf,1,4,fptr);
	for(i=0;i<4;i++){	
		x = x<<8;
		x += buf[i];
		//printf("buf[%d] : 0x%02x\n",i,buf[i]);		
	}
	//printf("x = 0x%08x\n",x);
	return x;
}

int main(){
    FILE *fptr;
    fptr = fopen("iimage.bin","rb");
    if(fptr==NULL){
        puts("fail to open file.");
        return 1;
    }
    unsigned char a[4];
    int i = 0;
    unsigned int pc;
    unsigned IMemSize;
	pc = read_4_bytes(fptr);
	IMemSize = read_4_bytes(fptr);
    
    for(i=0;i<4;i++){
        fread(&a[i],sizeof(char),1,fptr);
    }
    for(i=0;i<4;i++)    
        printf("a[%d] = 0x%02x\n",i,a[i]);
    
    fclose(fptr);
    return 0;
}