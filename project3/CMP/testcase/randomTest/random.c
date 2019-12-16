#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PC 0

int main(int argc,char **args){
    int seed = atoi(args[1]);
    int I_SIZE = atoi(args[2]);
    srand(seed);
    int i;
    
    // print init pc and # instruction 
    printf("%d\n%d\n",PC,I_SIZE+1);
    
    for(i=0;i<I_SIZE;i++){
        int tmp = rand()%256;
        printf("lw $s0 $0 %d\n",tmp*4);
    }
    printf("halt\n");
    
    return 0;
}