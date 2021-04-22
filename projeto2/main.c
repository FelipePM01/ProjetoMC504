#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N_COZINHEIROS 2
#define N_FACAS 1
#define N_PANELAS 1
sem_t panelas;
sem_t facas;
sem_t cozinheiros;
sem_t armazem;
sem_t nova_tarefa;
int f_cozinheiro(int* working , int* tarefa,sem_t* panelas,sem_t* facas,sem_t cozinheiros){ //working=variavel q guarda se acabou o programa ou nao
    while(working){
        //pegar tarefa
        if(tarefa==0){//cozinhar arroz
            sem_wait(armazem);
            pegar(tarefa);
            sem_pos(armazem);
            sem_wait(panelas);
            cortar(tarefa);
            sem_pos(panelas);
            sem_wait(armazem);
            entregar(tarefa);
            sem_pos(armazem);
            sem_pos(cozinheiros);
        }
        else if(tarefa==1){//cortar camarao
            sem_wait(armazem);
            pegar(tarefa);
            sem_pos(armazem);
            sem_wait(facas);
            cortar(tarefa);
            sem_pos(facas);
            sem_wait(armazem);
            entregar(tarefa);
            sem_pos(armazem);
            sem_pos(cozinheiros);

        }
        else if(tarefa==2){//cortar peixe
            sem_wait(armazem);
            pegar(tarefa);
            sem_pos(armazem);
            sem_wait(facas);
            cortar(tarefa);
            sem_pos(facas);
            sem_wait(armazem);
            entregar(tarefa);
            sem_pos(armazem);
            sem_pos(cozinheiros);
        }
        else{
           //dormir ate ter tarefa 
        }
    }
}

void cozinhar(){
    usleep(2000);
}
void entregar(){//

}