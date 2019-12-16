#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include "cache.h"

struct _MEM{
    //--determine miss/hit--//
    int vaild;
    int dirty;
    int lastUse;
    int offset;
};
typedef struct _MEM Memory;

void initMemory(Memory root[],int len){
    int i;
    
    for(i=0;i<len;i++){
        root[i].vaild = 0;
        root[i].dirty = 0;
        root[i].lastUse = -1;
        root[i].offset = -1;
    }
}

int replaceMem(Memory root[],int memPageNum,int pageSize,cache myCache[],unsigned int virAddr,unsigned int cacheSize,unsigned int blockSize,unsigned int asso){
    int i;
    int earlyest = 0x7FFFFFFF;
    int index;
    for(i=0;i<memPageNum;i++){
        //-- find a empty entry --//
        if(root[i].vaild==0){
            root[i].vaild = 1;
            root[i].lastUse = getCycleCounter();
            root[i].offset = virAddr%pageSize;
            return i;
        }
        if(root[i].lastUse!=-1&&root[i].lastUse<earlyest){
            index = i;
            earlyest = root[i].lastUse;
        }
    }
    
    //--memory is full, need to swap out a page(index point to this page)--//
    updateCache(myCache,index,cacheSize,blockSize,asso,pageSize);
    
    root[index].lastUse = getCycleCounter();
    
    
    
    return index;
}
#endif