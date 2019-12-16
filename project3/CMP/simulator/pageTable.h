#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include<stdio.h>

struct _PAGE_TABLE{
    //--determine miss/hit--//
    int vaild;
    int dirty;
    
    //--store address number--//
    int PPN;
};
typedef struct _PAGE_TABLE pageTable;

void initPageTable(pageTable root[],int len){
    int i;
    
    for(i=0;i<len;i++){
        root[i].vaild = 0;
        root[i].dirty = 0;
        root[i].PPN = -1;
    }
}

int inPageTable(pageTable root[],int addr,unsigned int pageSize){
    unsigned int pageIndex = addr/pageSize;
    if(root[pageIndex].vaild) return 1; //hit
    else return 0;  //page fault
    
}

void updatePageTable(pageTable root[],unsigned int ppn,unsigned int vpn,unsigned int pageNum){
    int i;
    
    for(i=0;i<pageNum;i++){
        if(root[i].vaild && root[i].PPN==ppn) root[i].vaild = 0;
    }
    root[vpn].vaild = 1;
    root[vpn].PPN = ppn;
}

#endif