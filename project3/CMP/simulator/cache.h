#ifndef CACHE_H
#define CACHE_H

#include<stdio.h>

struct _ENTRY{
    //--determine miss/hit--//
    int vaild;
    int dirty;
    int tag;
    int MRU;
};
typedef struct _ENTRY Entry;

struct _CACHE{
    Entry assoSet[64];  //atmost 64 associate
    
};
typedef struct _CACHE cache;

void initCache(cache root[],int len,int asso){
    int i,j;
    
    for(i=0;i<len;i++){
        for(j=0;j<asso;j++){
            root[i].assoSet[j].vaild = 0;
            root[i].assoSet[j].dirty = 0;
            root[i].assoSet[j].tag = -1;
            root[i].assoSet[j].MRU = 0;
        }
    }
}

void setMRU(cache root[],int entryIndex,int index,int asso){
    int i;
    root[entryIndex].assoSet[index].MRU = 1;
    
    for(i=0;i<asso;i++){
        if(root[entryIndex].assoSet[i].MRU == 0) return;
    }
    for(i=0;i<asso;i++){
        if(i!=index) root[entryIndex].assoSet[i].MRU = 0;
    }
}

int inCache(cache root[],unsigned int addr,unsigned int cacheSize,unsigned int blockSize,unsigned int asso){
    addr/=4;
    int WORD_SIZE = 4;
    int entryNum = cacheSize/(blockSize*asso);
    int blockOffset = addr%(blockSize/WORD_SIZE);
    addr/=(blockSize/WORD_SIZE); // shift out block offset
    
    int entryIndex = addr%entryNum;
    int addrTag = addr/entryNum;
    int i;
    
    for(i=0;i<asso;i++){
        if(root[entryIndex].assoSet[i].vaild && root[entryIndex].assoSet[i].tag==addrTag) {
            setMRU(root,entryIndex,i,asso);
            //root[entryIndex].assoSet[i].MRU = 1;
            return 1; //hit
        }
    }
    
    return 0; //miss
    
}

void replaceCache(cache root[],unsigned int addr,unsigned int cacheSize,unsigned int blockSize,unsigned int asso){
    addr/=4;
    int WORD_SIZE = 4;
    int entryNum = cacheSize/(blockSize*asso);
    int blockOffset = addr%(blockSize/WORD_SIZE);
    addr/=(blockSize/WORD_SIZE); // shift out block offset
    
    int entryIndex = addr%entryNum;
    int addrTag = addr/entryNum;
    int i,j;
    if(asso==1){ //direct map
        root[entryIndex].assoSet[0].vaild = 1;
        root[entryIndex].assoSet[0].MRU = 1;
        root[entryIndex].assoSet[0].tag = addrTag;
        return;
    }
    
    // try to find a empty block
    for(i=0;i<asso;i++){
        if(root[entryIndex].assoSet[i].vaild==0){
            root[entryIndex].assoSet[i].vaild = 1;
            root[entryIndex].assoSet[i].tag = addrTag;
            setMRU(root,entryIndex,i,asso);
            return;
        }
    }
    // all vaild, use bit-pseudo LRU 
    for(i=0;i<asso;i++){
        if(root[entryIndex].assoSet[i].MRU==0){
            root[entryIndex].assoSet[i].vaild = 1;
            root[entryIndex].assoSet[i].tag = addrTag;
            
            //check if other block are all = 1
            // for(j=i+1;j<asso;j++){
            //     if(root[entryIndex].assoSet[j].MRU==0) return;
            // }
            // for(j=0;j<asso;j++){
            //     if(j!=i) root[entryIndex].assoSet[j].MRU=0;
            // }
            setMRU(root,entryIndex,i,asso);
            return;
        }
    }
    
    printf("[?] All blocks MRU = 1 in cache. At cycle(%d)",getCycleCounter());
}

/**
 * do update right after memory page swap out
 * 
 * */

void updateCache(cache root[],unsigned int ppn, unsigned int cacheSize , unsigned int blockSize, unsigned int asso,unsigned int pageSize){
    int WORD_SIZE = 4;
    int entryNum = cacheSize/(blockSize*asso);
    
    int i,j;
    
    
    //-- look up entire cache table to find some address ,which place in the same page as addr place--//
    for(i=0;i<entryNum;i++){
        for(j=0;j<asso;j++){
            if(root[i].assoSet[j].vaild && ((root[i].assoSet[j].tag*entryNum|i)*(blockSize/WORD_SIZE)*4)/pageSize == ppn) {
                root[i].assoSet[j].MRU = 0;
                root[i].assoSet[j].vaild = 0;
            }
        }
    }
    
    
}

#endif