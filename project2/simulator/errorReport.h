#ifndef errorReport_h
#define errorReport_h
#include "instruction.h"
#include<stdio.h>

int calcLen(int key);
void write$0Error(instruction *inst ,FILE *fError);
void overflowDetect(int right_val_1,int right_val_2 ,FILE *fError);
int accessOverflowDetect(instruction *inst ,FILE *fError);
int misalignmentDetect(instruction *inst ,FILE *fError);



#endif