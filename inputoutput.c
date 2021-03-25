#include <stdio.h>
#include <stdlib.h>
char* entrada(){
    char* saida=malloc(sizeof(char)*144);
    
    for(int i=1;i<11;i++){
        
        scanf("%d %d %d %d %d %d %d %d %d %d",saida[i*12+1],saida[i*12+2],saida[i*12+3],saida[i*124],saida[i*12+5],saida[i*12+6],saida[i*12+7],saida[i*12+8],saida[i*12+9],saida[i*12+10]);
    }
    return saida;
}
void saida(char** matriz);
