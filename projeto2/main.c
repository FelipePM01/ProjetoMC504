#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N_COZINHEIROS 2
#define N_FACAS 1
#define N_PANELAS 1
#define N_CAMARAO 10
#define N_ARROZ 10
#define N_PEIXE 10

typedef struct tarefa{
    int tipo;
    struct tarefa* prox;
} Tarefa

typedef struct grupo_tarefas{
    Tarefa primeira;
    sem_t* acesso;
    sem_t* possui_tarefas;
} GrupoTarefas;

int f_cozinheiro(int* working, sem_t* panelas,sem_t* facas,sem_t* cozinheiros, sem_t* armazem, GrupoTarefas* tarefas){ //working=variavel q guarda se acabou o programa ou nao
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
           usleep(500);
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
    GrupoTarefas *tarefas=malloc(sizeof(GrupoTarefas));
    tarefa->acesso=malloc(sizeof(sem_t));
    tarefa->possui_tarefas=malloc(sizeof(sem_t));


    sem_init(&panelas,0,N_PANELAS);
    sem_init(&cozinheiros,0,N_COZINHEIROS);
    sem_init(&facas,0,N_FACAS);
    sem_init(&armazem,0,1);
    sem_init(tarefa->acesso,0,1);
    sem_init(tarefa->possui_tarefas,0,0);
    

    int camarao[1]={1};
    int sushi[2]={0,2};//arroz=0,camarao=1,peixe=2
    int* receitas[2]={camarao,sushi};
    int disponiveis[2]={1,1};
    while(disponiveis[0] || disponiveis[1]){
        sem_trywait(&cozinheiros);

    }

    
    srand(time(NULL));
    return 0;

}

void adicionar_receitas(int* receitas,int n,GrupoTarefas* tarefa){
    int r =rand();
    sem_wait(tarefa->acesso);



}