#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define N_COZINHEIROS 2
#define N_FACAS 1
#define N_PANELAS 1
#define N_CAMARAO 10
#define N_ARROZ 10
#define N_PEIXE 10
#define N_INGREDIENTES 3
#define N_PRONTOS 3
typedef struct armazem{
    int ingredientes[N_INGREDIENTES];
    int prontos[N_PRONTOS];
    int reservados[N_INGREDIENTES];
    sem_t acesso;
} Armazem;

typedef struct tarefa{
    int tipo;
    struct tarefa* prox;
} Tarefa;

typedef struct grupo_tarefas{
    Tarefa* primeira;
    Tarefa* ultima;
    sem_t acesso;
    sem_t possui_tarefas;
} GrupoTarefas;
typedef struct receita{
    int* etapas;
    int n;
} Receita;

typedef struct lista_pedidos{
    struct lista_pedidos* next;
    int* receita;
} Lista_pedidos;

typedef struct{
    Lista_pedidos* primeira;
    Lista_pedidos* ultima;
    int n;
} Grupo_pedidos;

typedef struct {
    int* working;
    sem_t* panelas;
    sem_t* facas;
    Armazem* armazem;
    GrupoTarefas* tarefas;
    sem_t* checagem;
} Args;

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

void pegar_ingrediente(Armazem* armazem,int ingrediente){//
    sem_wait(armazem->acesso);
    (armazem->ingredientes)[ingrediente]--;
    (armazem->reservado)[ingrediente]--;
    sem_pos(armazem->acesso);

}

void pegar_receita_pronta(Armazem* armazem, int receita){
    sem_wait(armazem->acesso);
    armazem->prontos[receita] = armazem->prontos[receita] - 1;
    sem_pos(armazem->acesso);
}

bool tem_receita_pronta(Armazem* armazem, int receita){
    sem_wait(armazem->acesso);
    if (armazem->prontos[receita]) > 0){
        sem_pos(armazem->acesso);
        return true;
    }else{
        sem_pos(armazem->acesso);
        return false;
    }
}

void f_cozinheiro(void * args){ //working=variavel q guarda se acabou o programa ou nao
    int* working = (Args*) args->working;
    sem_t* panelas = (Args*) args->panelas;
    sem_t* facas =  (Args*) args->facas;
    Armazem* armazem = (Args*) args->armazem;
    GrupoTarefas* tarefas = (Args*) args-> tarefas;
    sem_t* checagem = (Args*) args->checagem;

    sem_wait(tarefas->possui_tarefas);
    while(working){
        int tarefa_atual;
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
            pegar(armazem,tarefa_atual);
        else if(tarefa_atual==1){//preparar camarao
            sem_wait(facas);
            cortar(tarefa_atual);
            sem_pos(facas);
            sem_wait(panelas);
            cozinhar(tarefa);
            sem_pos(panelas);
            entregar(armazem,tarefa_atual,checagem);


        }
        else if(tarefa==2){//cortar peixe
            pegar(armazem,tarefa_atual);
            sem_wait(facas);
            cortar(tarefa_atual);
            sem_pos(facas);
            entregar(armazem,tarefa_atual,checagem);
        }
        sem_wait(tarefas->possui_tarefas);
    }
}

int main(){
    sem_t panelas;
    sem_t facas;
    sem_t checagem;
    Armazem armazem;

    for(int i=0;i<N_INGREDIENTES;i++){
        armazem.ingredientes[i]=10;
        armazem.reservados[i]=0;
    }
    for(int i=0;i<N_PRONTOS;i++){
        armazem.prontos[i]=0;
    }
    armazem.ingredientes[0]=N_ARROZ;
    armazem.ingredientes[1]=N_CAMARAO;
    armazem.ingredientes[2]=N_PEIXE;

    GrupoTarefas tarefas;

    sem_init(&panelas,0,N_PANELAS);
    sem_init(&checagem,0,0);
    sem_init(&facas,0,N_FACAS);
    sem_init(&armazem,0,1);
    sem_init(&(tarefas.acesso),0,1);
    sem_init(&(tarefas.possui_tarefas),0,0);

    int camarao[1]={1};
    int sushi[2]={0,2};//arroz=0,camarao=1,peixe=2
    int* receitas[2]={camarao,sushi};

    int n_pedidos;
    scanf("%d", &n_pedidos);
    int* pedidos_entrada = malloc(n_pedidos*sizeof(int));
    for (int i = 0; i < n_pedidos; i++)
    {
        int pedido;
        scanf("%d", &pedido);
        pedidos_entrada[i] = pedido;
    }
    Grupo_pedidos pedidos_aceitos;
    pedidos_aceitos.primeira = NULL;
    pedidos_aceitos.ultima = NULL;
    pedidos_aceitos.n = 0;

    int n_aceitos = 0;
    int n_completos = 0;
    int n_recusados = 0;

    int working = 1;
    Args args[N_COZINHEIROS];
    pthread_t cozinheiros[N_COZINHEIROS];

    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        args[i].working = &working;
        args[i].panelas = &panelas;
        args[i].facas = &facas;
        args[i].armazem = &armazem;
        args[i].tarefas = &tarefas;
        args[i].checagem = &checagem;
        pthread_create(&cozinheiros[i], NULL, &f_cozinheiro, &args[i]);
    }
    /* logica main*/
    while(n_aceitos + n_recusados < n_pedidos || n_completos < n_aceitos)
    {
        sem_wait(&checagem);
        

    }
    working = 0;
    /* acaba logica main */
    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        pthread_join(cozinheiros[i], NULL);
    }
    free(pedidos_entrada);
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