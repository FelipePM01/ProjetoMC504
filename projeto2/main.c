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
#define N_INGREDIENTES 3
#define N_PRONTOS 3
typedef struct armazem{
    int* ingredientes;
    int* prontos;
    int* reservados;
    sem_t* acesso;
} Armazem;

typedef struct tarefa{
    int tipo;
    struct tarefa* prox;
} Tarefa;

typedef struct grupo_tarefas{
    Tarefa* primeira;
    sem_t* acesso;
    sem_t* possui_tarefas;
} GrupoTarefas;
typedef struct receita{
    int* etapas;
    int n;
} Receita;

int f_cozinheiro(int* working, sem_t* panelas,sem_t* facas,sem_t* cozinheiros, Armazem* armazem, GrupoTarefas* tarefas,sem_t* checagem){ //working=variavel q guarda se acabou o programa ou nao
    int tarefa_atual;
    while(working){
        sem_wait(tarefas->possui_tarefas);
        
        sem_wait(tarefas->acesso);
        Tarefa* nova_tarefa=tarefas->primeira;
        tarefa_atual=nova_tarefa->tipo;
        tarefas->primeira=nova_tarefa->prox;
        free(nova_tarefa);
        sem_post(tarefa->acesso);
        
        if(tarefa_atual==0){//cozinhar arroz
            
            pegar(armazem,tarefa_atual);            
            sem_wait(panelas);
            cortar(tarefa);
            sem_pos(panelas);            
            entregar(armazem,tarefa_atual,checagem);            
            
        }
        else if(tarefa_atual==1){//cortar camarao
            pegar(armazem,tarefa_atual);
            sem_wait(facas);
            cortar(tarefa_atual);
            sem_pos(facas);
            entregar(armazem,tarefa_atual,checagem);
            

        }
        else if(tarefa==2){//cortar peixe
            pegar(armazem,tarefa_atual);
            sem_wait(facas);
            cortar(tarefa_atual);
            sem_pos(facas);
            entregar(armazem,tarefa_atual,checagem);
            
        }
        
        
    }
}

void cozinhar(){
    usleep(2000);
}
void cortar(){
    usleep(2000);
}
void entregar(Armazem* armazem,int resultado,sem_t* checagem){//
    sem_wait(armazem->acesso);
    (armazem->pronto)[resultado]++;
    sem_pos(armazem->acesso);
    sem_pos(checagem);
}
void pegar(Armazem* armazem,int ingrediente){//
    sem_wait(armazem->acesso);
    (armazem->ingredientes)[ingrediente]--;
    (armazem->reservado)[ingrediente]--;
    sem_pos(armazem->acesso);
    
}

int main(){
    sem_t panelas;
    sem_t facas;
    sem_t checagem;
    Armazem* armazem=malloc(sizeof(Armazem));
    armazem->acesso=malloc(sizeof(sem_t));
    armazem->ingredientes=malloc(3*sizeof(int));
    armazem->prontos=malloc(3*sizeof(int));
    armazem->reservados=malloc(3*sizeof(int));
    for(int i=0;i<3;i++){
        armazem->prontos[i]=0;
        armazem->reservados[i]=0;
    }
    armazem->ingredientes[0]=N_ARROZ;
    armazem->ingredientes[1]=N_CAMARAO;
    armazem->ingredientes[2]=N_PEIXE;
    GrupoTarefas *tarefas=malloc(sizeof(GrupoTarefas));
    tarefa->acesso=malloc(sizeof(sem_t));
    tarefa->possui_tarefas=malloc(sizeof(sem_t));



    sem_init(&panelas,0,N_PANELAS);
    sem_init(&checagem,0,0);
    sem_init(&facas,0,N_FACAS);
    sem_init(&armazem,0,1);
    sem_init(tarefa->acesso,0,1);
    sem_init(tarefa->possui_tarefas,0,0);
    

    int camarao[1]={1};
    int sushi[2]={0,2};//arroz=0,camarao=1,peixe=2
    Receita* receitas[2]={camarao,sushi};
    int n_receitas;
    int disponiveis[2]={1,1};
    while(disponiveis[0] || disponiveis[1]){
        sem_trywait(&cozinheiros);

    }

    
    srand(time(NULL));
    return 0;

}

void adicionar_receitas(Receita* receita,Armazem* armazem,int n,GrupoTarefas* tarefa,int** disponiveis,int* waiting){
    
    
    
    // if(disponiveis[0]==0){
    //     if(disponiveis[1]!=0)
    //         receita_a_adicionar=*receitas[1]; 
             
    // }
    // else if(disponiveis[1]==0)
    //     receita_a_adicionar=*receitas[0];
        
    
    // else{
    //     int random=rand()%n;
    //     receita_a_adicionar=*receitas[random];
        
    // }
    
    
    Tarefa** vetor=malloc(receita->n*sizeof(Tarefa*));
    for(int i=0;i<receita_a_adicionar->n;i++){
        int id_tarefa=receita->etapas[i];
        vetor[i]=malloc(sizeof(Tarefa));
        vetor[i]->tipo=id_tarefa;
        if(id_tarefa)
    }
    

    



}

int checar_ingrediente(Armazem* armazem,int* ingredientes,int n){
    int tem=1;
    sem_wait(armazem->acesso);
    for(int i=0;i<n;i++){
        if()
    }
    
}