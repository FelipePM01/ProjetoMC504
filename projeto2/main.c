#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N_COZINHEIROS 2
#define N_FACAS 1
#define N_PANELAS 1
typedef struct tarefas{
    int* vetor;
    int ini;
    int fim;
    sem_t* acesso;
    sem_t* tarefas_disponiveis;
} Tarefas;

int f_cozinheiro(int* working , sem_t* panelas,sem_t* facas,sem_t* cozinheiros,sem_t* armazem,Tarefas* tarefa){ //working=variavel q guarda se acabou o programa ou nao
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
void cortar(){
    usleep(2000);
}
void entregar(){//

}

int main(){
    sem_t panelas;
    sem_t facas;
    sem_t cozinheiros;
    sem_t armazem;
    Tarefas  *tarefa=malloc(sizeof(Tarefas));
    tarefa->ini=0;
    tarefa->fim=0;
    tarefa->acesso=malloc(sizeof(sem_t));
    tarefa->tarefas_disponiveis=malloc(sizeof(sem_t));
    tarefa->vetor=malloc(9*sizeof(int));


    sem_init(&panelas,0,N_PANELAS);
    sem_init(&cozinheiros,0,N_COZINHEIROS);
    sem_init(&facas,0,N_FACAS);
    sem_init(&armazem,0,1);
    sem_init(tarefa->acesso,0,1);
    sem_init(tarefa->tarefas_disponiveis,0,0);
    

    int camarao[1]={1};
    int sushi[2]={0,2};//arroz=0,camarao=1,peixe=2
    int* receitas[2]={camarao,sushi};

    int tarefas[9];
    volatile int ini_tarefa=0;
    volatile int fim_tarefa=0;
    srand(time(NULL));
    return 0;

}

void adicionar_receitas(int* receitas,int n,Tarefas* tarefa){
    int r =rand();
    sem_wait(tarefa->acesso);



}