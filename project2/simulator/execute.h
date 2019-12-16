#ifndef Execute_h
#define Execute_h

#include<stdio.h>
#include "instruction.h"

void writeErrorMessage(FILE *fError,int message);
int R_mode(instruction *inst, FILE *fError);
int execute(instruction *inst, FILE *fError);

/**
 * @return      if jumped , return 1,
 *              or return 0.
 * */
int jumpDetect(instruction *inst);
#endif
