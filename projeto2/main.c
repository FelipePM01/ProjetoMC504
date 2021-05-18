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
} ListaPedidos;

typedef struct{
    ListaPedidos* primeira;
    ListaPedidos* ultima;
    int n;
} GrupoPedidos;

typedef struct {
    int* working;
    sem_t* panelas;
    sem_t* facas;
    Armazem* armazem;
    GrupoTarefas* tarefas;
    sem_t* checagem;
} Args;

void cozinhar(){
    printf("Cozinhando...");
    usleep(2000);
}

void cortar(){
    printf("Cortando...");
    usleep(2000);
}

void preparar_peixe_frito(){
    printf("Preparando Peixe Frito...");
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

void pegar_receita_pronta(Armazem* armazem, Receita* receita){
    sem_wait(armazem->acesso);
    for(int i=0;i<receita->n;i++){
        armazem->prontos[receita->etapas[i]]-=1;
    }
    sem_post(armazem->acesso)
}

bool tem_receita_pronta(Armazem* armazem, Receita* receita){
    int tem=1;
    sem_wait(armazem->acesso);
    for(int i=0;i<receita->n;i++){
        if(armazem->prontos[receita->etapas[i]]<=0)
            tem=0;
    }
    sem_post(armazem->acesso)
    return tem;
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
            cozinhar();
            sem_pos(panelas);
            entregar(armazem,tarefa_atual,checagem);
        }
        else if(tarefa_atual==1){//preparar camarao
            pegar(armazem,tarefa_atual);
            sem_wait(facas);
            cortar();
            sem_pos(facas);
            sem_wait(panelas);
            cozinhar();
            sem_pos(panelas);
            entregar(armazem,tarefa_atual,checagem);
        }
        else if(tarefa_atual==2){//cortar peixe
            pegar(armazem,tarefa_atual);
            sem_wait(facas);
            cortar();
            sem_pos(facas);
            entregar(armazem,tarefa_atual,checagem);
        }
        else if(tarefa_atual==3){//preparar peixe frito
            pegar(armazem,tarefa_atual);
            sem_wait(panelas);
            preparar_peixe_frito();
            sem_pos(panelas);
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
    sem_init(&(armazem.acesso),0,1);
    sem_init(&(tarefas.acesso),0,1);
    sem_init(&(tarefas.possui_tarefas),0,0);


//todo
    Receita camarao;
    Receita sushi;
    Receita peixe_frito;
    int camarao_etapas[1]={1};
    int sushi_etapas[2]={0,2};//cozinhar arroz=0,preparar camarao=1,cortar peixe=2, preparar peixe frito=3
    int peixe_frito_etapas[1]={3};
    camarao.etapas=camarao_etapas;
    sushi.etapas=sushi_etapas;
    peixe_frito.etapas=peixo_frito_etapas;
    camarao.n=1;
    peixe_frito.n=1;
    sushi.n=2;
    Receita* receitas[3]={&camarao,&sushi,&peixe_frito};

//todo
    
    scanf()
    int n_pedidos;
    printf("Insira o numero de pedidos:\n");
    scanf("%d", &n_pedidos);
    int* pedidos_entrada = malloc(n_pedidos*sizeof(int));
    printf("Insira os pedidos (0 para camarao, 1 para sushi e 2 para peixe frito):\n");
    for (int i = 0; i < n_pedidos; i++)
    {
        int pedido;
        scanf("%d", &pedido);
        pedidos_entrada[i] = pedido;
    }
    GrupoPedidos pedidos_aceitos;
    pedidos_aceitos.primeira = NULL;
    pedidos_aceitos.ultima = NULL;
    pedidos_aceitos.n = 0;

    int n_aceitos = 0;
    int n_completos = 0;
    int n_recusados = 0;

    int count = 0;
    int pedido_atual = 0;
    while (count < 3 && pedido_atual < n_pedidos)
    {
        int res = adicionar_pedidos(receitas[pedidos_entrada[pedido_atual]],&armazem,
        &tarefas, &pedidos_aceitos);
        if (res == 1){
            n_aceitos += 1;
            count += 1;
        }
        else{
            n_recusados += 1;
        }
        pedido_atual++;
    }

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
        ListaPedidos* current = pedidos_aceitos.primeira;
        bool entregue = false;
        if (tem_receita_pronta(&armazem, current->receita))
        {
            pegar_receita_pronta(&armazem, current->receita);
            pedidos_aceitos.primeira = current->proximo;
            if (pedidos_aceitos.ultima == current)
                pedidos_aceitos.ultima = NULL;
            pedidos_aceitos.n -= 1
            n_completos+=1;
            free(current);
            entregue = true;
        }
        else while (current->proximo != NULL)
        {
            if (tem_receita_pronta(&armazem, current->proximo->receita))
            {
                pegar_receita_pronta(&armazem, current->proximo->receita);
                ListaPedidos dump = current->proximo;
                current->proximo = current->proximo->proximo;
                if (pedidos_aceitos.ultima = current->proximo)
                    pedidos_aceitos.ultima = current;
                pedidos_aceitos.n -= 1
                n_completos+=1;
                free(dump);
                entregue = true;
                break;
            }
        }
        if (entregue)
        {
            count = 0;
            while (count < 1 && pedido_atual < n_pedidos)
            {
                int res = adicionar_pedidos(receitas[pedidos_entrada[pedido_atual]],&armazem,
                &tarefas, &pedidos_aceitos);
                if (res == 1){
                    n_aceitos += 1;
                    count += 1;
                }
                else{
                    n_recusados += 1;
                }
                pedido_atual++;
            }
        }

    }
    working = 0;
    /* dar sempos aqui nos pedidos)*/
    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        sem_pos(tarefas.possui_tarefas);
    }
    /* acaba logica main */
    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        pthread_join(cozinheiros[i], NULL);
    }
    free(pedidos_entrada);
    return 0;

}

int adicionar_pedidos(Receita* receita,Armazem* armazem,GrupoTarefas* tarefa, GrupoPedidos* pedidos_aceitos){

    int n = receita->n;

    Tarefa** vetor=malloc(n*sizeof(Tarefa*));
    int* vet_ingred = (int*)malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        int id_tarefa=receita->etapas[i];
        vetor[i]=malloc(sizeof(Tarefa));
        vetor[i]->tipo=id_tarefa;
        switch (id_tarefa){
            case 0: // arroz
                vet_ingred[i] = 0;
                break;
            case 1: //camarao
                vet_ingred[i] = 1;
                break;
            case 2: //peixe
                vet_ingred[i] = 2;
                break;
            default:
                break;
        }
    }
    int checador=checar_ingrediente(armazem,vet_ingred,n);
    if(checador){
        for(int i=0;i<n-1;i++){
            vetor[i]->proximo=vetor[i+1];
        }
        vetor[n-1]->proximo=NULL;

        reservar_ingrediente(armazem,vet_ingred,n);

        if(tarefa->primeira==NULL)
            tarefa->primeira=vetor[0];
        else
            tarefa->ultima->proximo = vetor[0];
        tarefa->ultima=vetor[n-1];
        if (pedidos_aceitos->primeira == NULL){
            pedidos_aceitos->primeira = (ListaPedidos*)malloc(sizeof(ListaPedidos));
            pedidos_aceitos->primeira->receita = receita;
            pedidos_aceitos->ultima = pedidos_aceitos->primeira;
            pedidos_aceitos->ultima->proximo = NULL;
        }
        else{
            pedidos_aceitos->ultima->proximo = (ListaPedidos*)malloc(sizeof(ListaPedidos));
            pedidos_aceitos->ultima->proximo->receita = receita;
            pedidos_aceitos->ultima = pedidos_aceitos->ultima->proximo;
            pedidos_aceitos->ultima->proximo = NULL;
        }
    }
    else{
        for(int i=0;i<n-1;i++){
            free(vetor[i]);
        }
    }
    free(vet_ingred);
    return checador;

}

int checar_ingrediente(Armazem* armazem,int* ingredientes,int n){
    int tem=1;
    sem_wait(armazem->acesso);
    for(int i=0;i<n;i++){
        if(armazem->ingredientes[ingredientes[i]]-armazem->reservados[*ingredientes[i]]<=0)
            tem=0;
    }
    sem_post(armazem->acesso)
    return tem;
}

void reservar_ingredientes(Armazem* armazem,int* ingredientes,int n){
    sem_wait(armazem->acesso);
    for(int i=0;i<n;i++){
        armazem->reservados[ingredientes[i]]+=1;
    }
    sem_post(armazem->acesso)
}
