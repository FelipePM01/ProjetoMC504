#include <stdio.h>
#include <stdlib.h>
void entrada(int* saida,int n){
    
    
    for(int i=0;i<n;i++){
        
        if((i<(n-1))&&i>0){

           
            for(int j=1;j<n-2;j++){
                scanf("%d ",saida[i*n+j]);
            }
            scanf("%d\n",saida[i*n+n-2]);
        }
       
    }
    
    
}
void saida(char* entrada){
    printf("+++++++++++++++++++++++\n");
    for(int i=1;i<11;i++){
        printf("+ ");
        for(int j=1;j<11;i++){
            if(entrada[i*12+j]==0)
                printf("  ");
            else 
                printf("@ ");
        }
        printf("+\n");
    }
    printf("+++++++++++++++++++++++\n");
    
}

