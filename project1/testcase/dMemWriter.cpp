#include<iostream>
#include<cstdio>
using namespace std;

void writeToFile(FILE *fptr,unsigned int data){
    unsigned int mask[] = {0x000000FF,0x0000FF00,0x00FF0000,0xFF000000};
    unsigned int tmp;
    for(int i=3;i>=0;i--){
        tmp = (mask[i]&data)>>(8*i);
        //printf("tmp[%d] = 0x%02x\n",i,tmp);
        fwrite(&tmp,sizeof(char),1,fptr);
    }
    //printf("\n");
}

int main(){
    FILE *fptr;
    fptr = fopen("dimage.bin","wb");
    if(fptr==NULL){
        puts("[x] Fail to open file.");
        return 1;
    }
    unsigned int sp;
    unsigned int DMemSize;
    cin>>sp;
    cin>>DMemSize;
    
    writeToFile(fptr,sp);
    writeToFile(fptr,DMemSize);
    
    unsigned int input;
    for(int i=0;i<DMemSize;i++){
        cin>>input;
        //cout<<input<<endl;
        writeToFile(fptr,input);
    }
    
    fclose(fptr);
    return 0;
}