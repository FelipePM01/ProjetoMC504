#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>

#define N_COZINHEIROS 2
#define N_FACAS 1
#define N_PANELAS 1
#define N_CAMARAO 10
#define N_ARROZ 10
#define N_PEIXE 10
#define N_INGREDIENTES 3
#define N_PRONTOS 4

typedef struct armazem{
    int ingredientes[N_INGREDIENTES];
    int prontos[N_PRONTOS];
    int reservados[N_INGREDIENTES];
    sem_t* acesso;
} Armazem;

typedef struct tarefa{
    int tipo;
    struct tarefa* prox;
} Tarefa;

typedef struct grupo_tarefas{
    Tarefa* primeira;
    Tarefa* ultima;
    sem_t* acesso;
    sem_t* possui_tarefas;
} GrupoTarefas;

typedef struct receita{
    int* etapas;
    int n;
    int id;
} Receita;

typedef struct lista_pedidos{
    struct lista_pedidos* next;
    Receita* receita;
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
    int id;
} Args;

void cozinhar_arroz(int id){
    printf("Cozinheiro %d está cozinhando arroz...\n",id);
    usleep(2000);
    printf("...e o cozinheiro %d terminou de cozinhar arroz.\n",id);
}

void cozinhar_camarao(int id){
    printf("Cozinheiro %d está cozinhando camarão...\n",id);
    usleep(2000);
    printf("...e o cozinheiro %d terminou de cozinhar camarão.\n",id);
}

void cortar_camarao(int id){
    printf("Cozinheiro %d está cortando camarão...\n",id);
    usleep(2000);
    printf("...e o cozinheiro %d terminou de cortar camarão.\n",id);
}

void cortar_peixe(int id){
    printf("Cozinheiro %d está cortando peixe...\n",id);
    usleep(2000);
    printf("...e o cozinheiro %d terminou de cortar peixe.\n",id);
}

void fritar(int id){
    printf("Cozinheiro %d está fritando peixe ...\n",id);
    usleep(2000);
    printf("...e o cozinheiro %d terminou de preparar o peixe frito.\n",id);
}

void entregar(Armazem* armazem,int resultado,sem_t* checagem){//

    sem_wait(armazem->acesso);
    (armazem->prontos)[resultado]++;
    sem_post(armazem->acesso);
    sem_post(checagem);
}

void pegar_ingrediente(Armazem* armazem,int ingrediente){//

    sem_wait(armazem->acesso);
    (armazem->ingredientes)[ingrediente]--;
    (armazem->reservados)[ingrediente]--;
    sem_post(armazem->acesso);
}

void pegar_receita_pronta(Armazem* armazem, Receita* receita){

    sem_wait(armazem->acesso);
    for(int i=0;i<receita->n;i++){
        armazem->prontos[receita->etapas[i]]-=1;
    }
    printf("Prato entregue: ");
    if (receita->id == 0){
        printf("Camarão.\n");
    }
    else if (receita->id == 1){
        printf("Sushi.\n");
    }
    else if (receita->id == 2){
        printf("Peixe Frito.\n");
    }
    else{
        printf("nao deveria acontecer\n");
    }
    sem_post(armazem->acesso);
}

bool tem_receita_pronta(Armazem* armazem, Receita* receita){
    int tem=1;

    sem_wait(armazem->acesso);
    for(int i=0;i<receita->n;i++){
        if(armazem->prontos[receita->etapas[i]]<=0)
            tem=0;
    }
    sem_post(armazem->acesso);
    return tem;
}

void* f_cozinheiro(void * args){ //working=variavel q guarda se acabou o programa ou nao
    Args argumentos = *((Args*)args);

    int* working = argumentos.working;
    sem_t* panelas = argumentos.panelas;
    sem_t* facas =  argumentos.facas;
    Armazem* armazem = argumentos.armazem;
    GrupoTarefas* tarefas = argumentos.tarefas;
    sem_t* checagem = argumentos.checagem;
    int id= argumentos.id;


    sem_wait(tarefas->possui_tarefas);
    while(*working){
        int tarefa_atual;
        sem_wait(tarefas->acesso);
        Tarefa* nova_tarefa=tarefas->primeira;
        tarefa_atual=nova_tarefa->tipo;
        if(tarefas->primeira==tarefas->ultima){
            tarefas->primeira=NULL;
            tarefas->ultima=NULL;
        }
        else{
            tarefas->primeira=nova_tarefa->prox;
        }
        free(nova_tarefa);
        sem_post(tarefas->acesso);

        if(tarefa_atual==0){//cozinhar arroz
            pegar_ingrediente(armazem,0);
            sem_wait(panelas);
            cozinhar_arroz(id);
            sem_post(panelas);
            entregar(armazem,0,checagem);
        }
        else if(tarefa_atual==1){//preparar camarao
            pegar_ingrediente(armazem,1);
            sem_wait(facas);
            cortar_camarao(id);
            sem_post(facas);
            sem_wait(panelas);
            cozinhar_camarao(id);
            sem_post(panelas);
            entregar(armazem,1,checagem);
        }
        else if(tarefa_atual==2){//cortar peixe
            pegar_ingrediente(armazem,2);
            sem_wait(facas);
            cortar_peixe(id);
            sem_post(facas);
            entregar(armazem,2,checagem);
        }
        else if(tarefa_atual==3){//fritar peixe
            pegar_ingrediente(armazem,2);
            sem_wait(facas);
            cortar_peixe(id);
            sem_post(facas);
            sem_wait(panelas);
            fritar(id);
            sem_post(panelas);
            entregar(armazem,3,checagem);
        }
        sem_wait(tarefas->possui_tarefas);
    }
    return NULL;
}
int checar_ingrediente(Armazem* armazem,int* ingredientes,int n){
    int tem=1;
    sem_wait(armazem->acesso);
    for(int i=0;i<n;i++){
        if(armazem->ingredientes[ingredientes[i]]<=0)
        {
            tem=0;
        }
    }
    sem_post(armazem->acesso);
    return tem;
}

void reservar_ingredientes(Armazem* armazem,int* ingredientes,int n){

    sem_wait(armazem->acesso);
    for(int i=0;i<n;i++){
        armazem->reservados[ingredientes[i]]+=1;
        armazem->ingredientes[ingredientes[i]]-=1;
    }
    sem_post(armazem->acesso);
}
int adicionar_pedidos(Receita* receita,Armazem* armazem,GrupoTarefas* tarefa, GrupoPedidos* pedidos_aceitos){

    int n = receita->n;

    Tarefa** vetor=(Tarefa**)malloc(n*sizeof(Tarefa*));
    int* vet_ingred = (int*)malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        int id_tarefa=receita->etapas[i];
        vetor[i]=(Tarefa*)malloc(sizeof(Tarefa));
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
            case 3:
                vet_ingred[i] = 2;
                break;
        }
    }
    int checador=checar_ingrediente(armazem,vet_ingred,n);
    if(checador){
        for(int i=0;i<n-1;i++){
            vetor[i]->prox=vetor[i+1];
        }
        vetor[n-1]->prox=NULL;

        reservar_ingredientes(armazem,vet_ingred,n);

        if(tarefa->primeira==NULL){
            tarefa->primeira=vetor[0];
        }
        else{
            tarefa->ultima->prox = vetor[0];
        }
        tarefa->ultima=vetor[n-1];
        if (pedidos_aceitos->primeira == NULL){
            pedidos_aceitos->primeira = (ListaPedidos*)malloc(sizeof(ListaPedidos));
            pedidos_aceitos->primeira->receita = receita;
            pedidos_aceitos->ultima = pedidos_aceitos->primeira;
            pedidos_aceitos->ultima->next = NULL;
        }
        else{
            pedidos_aceitos->ultima->next= (ListaPedidos*)malloc(sizeof(ListaPedidos));
            pedidos_aceitos->ultima->next->receita = receita;
            pedidos_aceitos->ultima = pedidos_aceitos->ultima->next;
            pedidos_aceitos->ultima->next = NULL;
        }
        for (int i = 0; i < n; i++)
        {
            sem_post(tarefa->possui_tarefas);
        }
    }
    else{
        for(int i=0;i<n;i++){
            free(vetor[i]);
        }
    }
    free(vet_ingred);
    free(vetor);
    return checador;

}


int main(){
    sem_t* panelas = (sem_t*)malloc(sizeof(sem_t));
    sem_t* facas = (sem_t*)malloc(sizeof(sem_t));
    sem_t* checagem = (sem_t*)malloc(sizeof(sem_t));
    Armazem* armazem = (Armazem*)malloc(sizeof(Armazem));

    for(int i=0;i<N_INGREDIENTES;i++){
        armazem->ingredientes[i]=10;
        armazem->reservados[i]=0;
    }
    for(int i=0;i<N_PRONTOS;i++){
        armazem->prontos[i]=0;
    }
    armazem->ingredientes[0]=N_ARROZ;
    armazem->ingredientes[1]=N_CAMARAO;
    armazem->ingredientes[2]=N_PEIXE;

    GrupoTarefas* tarefas = (GrupoTarefas*)malloc(sizeof(GrupoTarefas));
    tarefas->primeira = NULL;
    tarefas->ultima = NULL;
    sem_init(panelas,0,N_PANELAS);
    sem_init(checagem,0,0);
    sem_init(facas,0,N_FACAS);
    armazem->acesso = (sem_t*)malloc(sizeof(sem_t));
    sem_init(armazem->acesso,0,1);
    tarefas->acesso = (sem_t*)malloc(sizeof(sem_t));
    sem_init(tarefas->acesso,0,1);
    tarefas->possui_tarefas = (sem_t*)malloc(sizeof(sem_t));
    sem_init(tarefas->possui_tarefas,0,0);

//todo
    Receita camarao;
    Receita sushi;
    Receita peixe_frito;
    int camarao_etapas[1]={1};
    int sushi_etapas[2]={0,2};//cozinhar arroz=0,preparar camarao=1,cortar peixe=2, preparar peixe frito=3
    int peixe_frito_etapas[1]={3};
    camarao.etapas=camarao_etapas;
    camarao.id = 0;
    sushi.etapas=sushi_etapas;
    sushi.id = 1;
    peixe_frito.etapas=peixe_frito_etapas;
    peixe_frito.id = 2;
    camarao.n=1;
    peixe_frito.n=1;
    sushi.n=2;
    Receita* receitas[3]={&camarao,&sushi,&peixe_frito};

//todo


    int n_pedidos;
    printf("Insira o numero de pedidos:\n");
    scanf("%d", &n_pedidos);
    int* pedidos_entrada = (int*)malloc(n_pedidos*sizeof(int));
    printf("Insira os pedidos (0 para camarao, 1 para sushi e 2 para peixe frito):\n");
    for (int i = 0; i < n_pedidos; i++)
    {
        int pedido;
        scanf("%d", &pedido);
        pedidos_entrada[i] = pedido;
    }
    printf("Todos os pedidos foram recebidos.\n");
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
        int res = adicionar_pedidos(receitas[pedidos_entrada[pedido_atual]],armazem,
        tarefas, &pedidos_aceitos);
        if (res != 0){
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
        args[i].panelas = panelas;
        args[i].facas = facas;
        args[i].armazem = armazem;
        args[i].tarefas = tarefas;
        args[i].checagem = checagem;
        args[i].id=i;
        pthread_create(&cozinheiros[i], NULL, &f_cozinheiro, &args[i]);
    }
    /* logica main*/
    while(n_aceitos + n_recusados < n_pedidos || n_completos < n_aceitos)
    {
        printf("Esperando receber ingredientes prontos...\n");
        sem_wait(checagem);
        printf("Recebeu ingrediente pronto\n");
        ListaPedidos* current = pedidos_aceitos.primeira;
        int entregue = 0;
        while (current != NULL && tem_receita_pronta(armazem, current->receita))
        {
            pegar_receita_pronta(armazem, current->receita);
            pedidos_aceitos.primeira = current->next;
            if (pedidos_aceitos.ultima == current)
                pedidos_aceitos.ultima = NULL;
            pedidos_aceitos.n -= 1;
            n_completos+=1;
            ListaPedidos* dump = current;
            current = current->next;
            free(dump);
            entregue += 1;

        }
        while (current != NULL && current->next != NULL)
        {
            if (tem_receita_pronta(armazem, current->next->receita))
            {
                pegar_receita_pronta(armazem, current->next->receita);
                ListaPedidos* dump = current->next;
                current->next = current->next->next;
                if (current->next == NULL){
                    pedidos_aceitos.ultima = current;
                }
                pedidos_aceitos.n -= 1;
                n_completos+=1;
                free(dump);
                entregue +=1;
            }
            else
            {
                current = current->next;
            }
        }
        if (entregue)
        {
            count = 0;
            while (count < entregue && pedido_atual < n_pedidos)
            {
                int res = adicionar_pedidos(receitas[pedidos_entrada[pedido_atual]],armazem,
                tarefas, &pedidos_aceitos);
                if (res != 0){
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
    printf("Todos os pedidos aceitos foram entregues.\n");
    working = 0;
    /* dar sempos aqui nos pedidos)*/
    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        sem_post(tarefas->possui_tarefas);
    }
    /* acaba logica main */
    for(int i = 0; i < N_COZINHEIROS; i++)
    {
        pthread_join(cozinheiros[i], NULL);
    }
    free(pedidos_entrada);
    free(panelas);
    free(facas);
    free(checagem);
    free(armazem->acesso);
    free(armazem);
    free(tarefas->acesso);
    free(tarefas->possui_tarefas);
    free(tarefas);
    return 0;

}
