#include<iostream>
#include<fstream>
#include<cstdio>
#define INST_NUM 34

using namespace std;

void writeToFile(FILE *fptr,int data){
    unsigned int mask[] = {0x000000FF,0x0000FF00,0x00FF0000,0xFF000000};
    unsigned int tmp;
    for(int i=3;i>=0;i--){
        tmp = (mask[i]&data)>>8*i;
        //printf("tmp[%d] = 0x%02x\n",i,tmp);
        fwrite(&tmp,sizeof(char),1,fptr);
    }
    //printf("\n");
}

int findIndex(string *data,string key){
    int i;
    const int regSize = 32;
    for(i=0;i<regSize;i++){
        if(data[i].compare(key)==0) break;
    }
    if(i>=regSize) return -1;
    else return i;
}


int main(){
    
    FILE *fptr = fopen("iimage.bin","wb");
    if(fptr==NULL){
        puts("[x]Fail to open file.");
        return 1;
    }
    
    string operation;
    string rs,rt,rd;
    int constant;
    string opList[] = { 
        "add"   ,   "addu"  ,   "sub"   ,   "and"   ,   "or",
        "xor"   ,   "nor"   ,   "nand"  ,   "slt"   ,   "sll",
        "srl"   ,   "sra"   ,   "jr"    ,   "addi"  ,   "addiu",
        "lw"    ,   "lh"    ,   "lhu"   ,   "lb"    ,   "lbu",
        "sw"    ,   "sh"    ,   "sb"    ,   "lui"   ,   "andi",
        "ori"   ,   "nori"  ,   "slti"  ,   "beq"   ,   "bne",
        "bgtz"  ,   "j"     ,   "jal"   ,   "halt"
    };
    int opcodeList[] = {
        0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x08,0x09,
        0x23,0x21,0x25,0x20,0x24,
        0x2B,0x29,0x28,0x0F,0x0C,
        0x0D,0x0E,0x0A,0x04,0x05,
        0x07,0x02,0x03,0x3F
    };
    
    string regList[] = {
        "$0",   "$at",  "$v0",  "$v1",  "$a0",
        "$a1",  "$a2",  "$a3",  "$t0",  "$t1",  
        "$t2",  "$t3",  "$t4",  "$t5",  "$t6",  
        "$t7",  "$s0",  "$s1",  "$s2",  "$s3",  
        "$s4",  "$s5",  "$s6",  "$s7",  "$t8",  
        "$t9",  "$k0",  "$k1",  "$gp",  "$sp", 
        "$fp",  "$ra"
    };
    
    int funcList[] = { //for R mode
        0x20,0x21,0x22,0x24,0x25,
        0x26,0x27,0x28,0x2A,0x00,
        0x02,0x03,0x08,
    };
    
    int i;
    int rt_int;
    int rd_int;
    int rs_int;
    
    
    unsigned int mechineCode;
    
    int pc;
    cin>>pc;
    int input_size;
    cin>>input_size;
    
    writeToFile(fptr,pc);
    writeToFile(fptr,input_size);
    while(cin>>operation){
        for(i=0;i<INST_NUM;i++)
            if(operation.compare(opList[i])==0) break;
        
        if(i==INST_NUM){
            cout<<"[x]Invalid instruction("<<operation<<").";
            continue;
        }
        
        //--R mode--//
        if(i<9){ //op-rs-rt-rd-00000-func
            cin>>rd>>rs>>rt;
            rd_int = findIndex(regList,rd);
            if(rd_int==-1)  cout<<"[x] Register word("<<rd<<") not found."<<endl;
            rs_int = findIndex(regList,rs);
            if(rs_int==-1)  cout<<"[x] Register word("<<rs<<") not found."<<endl;
            rt_int = findIndex(regList,rt);
            if(rt_int==-1)  cout<<"[x] Register word("<<rt<<") not found."<<endl;
            
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rs_int<<21;
            mechineCode |= rt_int<<16;
            mechineCode |= rd_int<<11;
            mechineCode |= funcList[i];
            
        }else if(i<12){ //op-00000-rt-rd-unsigned shamt-func
            cin>>rd>>rt>>constant;
            rd_int = findIndex(regList,rd);
            if(rd_int==-1)  cout<<"[x] Register word("<<rd<<") not found."<<endl;
            rt_int = findIndex(regList,rt);
            if(rt_int==-1)  cout<<"[x] Register word("<<rt<<") not found."<<endl;
            
            if(i<11&&constant<0){//constant should be unsigned 
                puts("[!] Constant value should be unsigned.");
            }
            if(i==11&&(constant<-16||constant>15)){
                puts("[!] Singed constant value should between -16 and 15.");
            }
            
            if(constant>=32){
               puts("[!] Constant value over 5 bits. set to 11111");
            }
            constant &= 0x0000001F; 
            
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rt_int<<16;
            mechineCode |= rd_int<<11;
            mechineCode |= constant<<6;
            mechineCode |= funcList[i];
            
        }else if(i==12){ //op-rs-000000000000000...-func
            cin>>rs;
            rs_int = findIndex(regList,rs);
            if(rs_int==-1)  cout<<"[x] Register word("<<rs<<") not found."<<endl;
            
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rs_int<<21;
            mechineCode |= funcList[i];
        }else if(i==23){ //op--00000--rt--constant
            cin>>rt>>constant;
            rt_int = findIndex(regList,rt);
            if(rt_int==-1)  cout<<"[x] Register word("<<rt<<") not found."<<endl;
            
            constant &= 0x0000FFFF; 
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rt_int<<16;
            mechineCode |= constant;
            
        }else if(i<28){ //op--rs--rt--constant
            cin>>rt>>rs>>constant;
            rs_int = findIndex(regList,rs);
            if(rs_int==-1)  cout<<"[x] Register word("<<rs<<") not found."<<endl;
            rt_int = findIndex(regList,rt);
            if(rt_int==-1)  cout<<"[x] Register word("<<rt<<") not found."<<endl;
            
            constant &= 0x0000FFFF; 
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rs_int<<21;
            mechineCode |= rt_int<<16;
            mechineCode |= constant;
        }else if(i<30){
            cin>>rs>>rt>>constant;
            
            rs_int = findIndex(regList,rs);
            if(rs_int==-1)  cout<<"[x] Register word("<<rs<<") not found."<<endl;
            rt_int = findIndex(regList,rt);
            if(rt_int==-1)  cout<<"[x] Register word("<<rt<<") not found."<<endl;
            constant &= 0x0000FFFF; 
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rs_int<<21;
            mechineCode |= rt_int<<16;
            mechineCode |= constant;
        }else if(i==30){    //op--rs--00000--constant
            cin>>rs>>constant;
            rs_int = findIndex(regList,rs);
            if(rs_int==-1)  cout<<"[x] Register word("<<rs<<") not found."<<endl;
            
            constant &= 0x0000FFFF; 
            mechineCode = opcodeList[i]<<26;
            mechineCode |= rs_int<<21;
            mechineCode |= constant;
        }else if(i<33){
            cin>>constant;
            constant &= 0x03FFFFFF;
            mechineCode = opcodeList[i]<<26;
            mechineCode |= constant;
        }else if(i==33){
            mechineCode = opcodeList[i]<<26;
        }
        else {
            puts("[x] Invalid instruction.");
        }
        
        writeToFile(fptr,mechineCode);
    }
    
    fclose(fptr);
    
    return 0;
}