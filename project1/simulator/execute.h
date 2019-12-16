#ifndef Execute_h
#define Execute_h

void writeErrorMessage(FILE *fError,int message);
int R_mode(instruction *inst, FILE *fError);
int execute(instruction *inst, FILE *fError);

#endif
