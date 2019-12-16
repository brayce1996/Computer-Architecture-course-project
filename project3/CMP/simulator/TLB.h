#ifndef TLB_H
#define TLB_H

#include<stdio.h>

struct _TLB{
    //--determine miss/hit--//
    int vaild;
    int dirty;
    int tag;
    int lastUse;
    
    //--store address number--//
    int PPN;
};
typedef struct _TLB TLB;

void initTLB(TLB root[],int len){
    int i;
    
    for(i=0;i<len;i++){
        root[i].vaild = 0;
        root[i].dirty = 0;
        root[i].tag = -1;
        root[i].lastUse = -1;
        
        root[i].PPN = -1;
    }
}

int searchTLB(TLB root[],unsigned int addr,unsigned int pageSize,unsigned int TlbNum){
    unsigned int pageTag = addr/pageSize;
    int i;
    
    //search tag of every entry in TLB
    for(i=0;i<TlbNum;i++){
        if(root[i].vaild&&root[i].tag == pageTag){
            root[i].lastUse = getCycleCounter();
            return root[i].PPN; //hit
        }
    }
    
    return -1; //miss
}

void replaceTLB(TLB root[],unsigned int ppn,unsigned int pageTag,unsigned int pageNum){
    int i;
    int earlyest = 0x7FFFFFFF;
    int index;
    // swap out the origin ppn-vpn pair(might not found)
    for(i=0;i<pageNum;i++){
        if(root[i].PPN==ppn){
            root[i].PPN = -1;
            root[i].tag = -1;
            root[i].vaild = 0;
        }
    }
    
    // try to find the first empty entry
    for(i=0;i<pageNum;i++){
        if(root[i].vaild==0){
            root[i].PPN = ppn;
            root[i].tag = pageTag;
            root[i].vaild = 1;
            root[i].lastUse = getCycleCounter();
            return;
        }
        if(root[i].lastUse!=-1&&root[i].lastUse<earlyest){
            index = i;
            earlyest = root[i].lastUse;
        }
    }
    
    // TLB is full , need to discard the entry index point to
    root[index].PPN = ppn;
    root[index].vaild = 1;
    root[index].lastUse = getCycleCounter();
    root[index].tag = pageTag;
    
    
    
}

#endif