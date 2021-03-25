#include <stdio.h>
#include <stdlib.h>
char* entrada(){
    char* saida=malloc(sizeof(char)*144);
    
    for(int i=0;i<12;i++){
        
        if(i<11&&i>0){

            saida[i*12]=0;
            saida[i*12+11]=0;
            scanf("%c %c %c %c %c %c %c %c %c %c",saida[i*12+1],saida[i*12+2],saida[i*12+3],saida[i*124],saida[i*12+5],saida[i*12+6],saida[i*12+7],saida[i*12+8],saida[i*12+9],saida[i*12+10]);
        else{
            for(int j=0;j<12;j++){
                saida[i*12+j]=0;
            }
        }
    }
    for(int i=0;i<144;i++){
        saida[i]-=48;
    }

    return saida;
}
void saida(char* entrada){
    printf("+++++++++++++++++++++++\n");
    for(int i=1;i<11,i){
        printf("+ %");
    }
    printf("+++++++++++++++++++++++\n");
    
}
