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

#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif

void sleep_ms(int milliseconds)
{
    #ifdef WIN32
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        sleep_ms(milliseconds * 1000);
    #endif
}

sem_t* show;

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
    sem_t* acesso;
    int n;
} GrupoPedidos;

typedef struct DadosVisuais{
    Armazem* armazem;
    GrupoPedidos* pedidos;
    int entregues[99];
    int index;
    int status_cozinheiros[N_COZINHEIROS]; // 0 nada, 1 esperando faca, 2 usando faca, 3 esperando fogão, 4 usando fogão
} DadosVisuais;

typedef struct {
    int* working;
    sem_t* panelas;
    sem_t* facas;
    Armazem* armazem;
    GrupoTarefas* tarefas;
    DadosVisuais* dados_visuais;
    sem_t* checagem;
    int id;
} Args;

void show_data(DadosVisuais* data)
{
    clrscr();
    char mainstring[15][70] = {
        "+   ___________                           |Arroz:             xx +\n\0",
        "+  |    ____   |                          |Camarao:           xx +\n\0",
        "+  |   |    |_ |                          |Peixe:             xx +\n\0",
        "+  |   |    |_||                          |______________________+\n\0",
        "+  |   |____|  |                          |Arroz cozido:      xx +\n\0",
        "+  |     /\\    |                          |Peixe cortado:     xx +\n\0",
        "+  |___________|                          |Camarao preparado: xx +\n\0",
        "+   ___________                           |Peixe frito:       xx +\n\0",
        "+  |    ____   |                          |______________________+\n\0",
        "+  |   |    |  |                                                 +\n\0",
        "+  |   |    |  |                                                 +\n\0",
        "+  |   |____|  |                                                 +\n\0",
        "+  |    ____   |                                                 +\n\0",
        "+  |___________|                                                 +\n\0",
        "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\0"
    };

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("+|                    |                    |                    |+\n");
    int vet_pedidos[3] = {-1,-1,-1};
    sem_wait(data->pedidos->acesso);
    ListaPedidos* current_pedido = data->pedidos->primeira;
    int i = 0;
    while(i<3 && current_pedido != NULL)
    {
        vet_pedidos[i] = current_pedido->receita->id;
        i++;
        current_pedido = current_pedido->next;
    }
    sem_post(data->pedidos->acesso);
    printf("+");
    for (i = 0; i < 3; i++)
    {
        switch (vet_pedidos[i]) {
            case 0:
                printf("|       camarao      ");
                break;
            case 1:
                printf("|        sushi       ");
                break;
            case 2:
                printf("|     peixe frito    ");
                break;
            default:
                printf("|                    ");

        }
    }
    printf("|+\n");
    printf("+|____________________|____________________|____________________|+\n");
    printf("+                                           _____________________+\n");

    int ingr[N_INGREDIENTES];
    int pronto[N_PRONTOS];
    sem_wait(data->armazem->acesso);
    for (i = 0; i < N_INGREDIENTES; i++)
    {
        ingr[i] = data->armazem->ingredientes[i]+data->armazem->reservados[i];
    }
    for(i = 0; i < N_PRONTOS; i++)
    {
        pronto[i] = data->armazem->prontos[i];
    }
    sem_post(data->armazem->acesso);
    mainstring[0][62] = '0'+(ingr[0]/10);
    mainstring[0][63] = '0'+(ingr[0]%10);
    mainstring[1][62] = '0'+(ingr[1]/10);
    mainstring[1][63] = '0'+(ingr[1]%10);
    mainstring[2][62] = '0'+(ingr[2]/10);
    mainstring[2][63] = '0'+(ingr[2]%10);
    mainstring[4][62] = '0'+(pronto[0]/10);
    mainstring[4][63] = '0'+(pronto[0]%10);
    mainstring[5][62] = '0'+(pronto[1]/10);
    mainstring[5][63] = '0'+(pronto[1]%10);
    mainstring[6][62] = '0'+(pronto[2]/10);
    mainstring[6][63] = '0'+(pronto[2]%10);
    mainstring[7][62] = '0'+(pronto[3]/10);
    mainstring[7][63] = '0'+(pronto[3]%10);

    if (data->status_cozinheiros[0] == 2 || data->status_cozinheiros[1] == 2)
    {
        mainstring[12][8] = ' ';
        mainstring[12][9] = ' ';
        mainstring[12][10] = ' ';
        mainstring[12][11] = ' ';
    }
    if (data->status_cozinheiros[0] != 4 && data->status_cozinheiros[1] != 4)
    {
        mainstring[5][8] = ' ';
        mainstring[5][9] = ' ';
        mainstring[5][10] = ' ';
    }
    int x, y, id = 0;
    for (id = 0; id < 2; id ++)
    {
        if (data->status_cozinheiros[id] == 0)
        {
            y = 10;
            x = 49 + 8*id;
        }
        else if (data->status_cozinheiros[id] == 1)
        {
            y = 9;
            x = 26;
        }
        else if (data->status_cozinheiros[id] == 2)
        {
            y = 9;
            x = 19;
        }
        else if (data->status_cozinheiros[id] == 3)
        {
            y = 2;
            x = 26;
        }
        else if (data->status_cozinheiros[id] == 4)
        {
            y = 2;
            x = 19;
        }
        mainstring[y][x+2] = 'o';
        mainstring[y+1][x] = '/';
        mainstring[y+1][x+1] = '|';
        mainstring[y+1][x+2] = '0'+id;
        mainstring[y+1][x+3] = '|';
        mainstring[y+1][x+4] = '\\';
        mainstring[y+2][x+1] = '|';
        mainstring[y+2][x+2] = '_';
        mainstring[y+2][x+3] = '|';
        mainstring[y+3][x+1] = '/';
        mainstring[y+3][x+3] = '\\';
        // tabela ja foi
    }
    for (i = 0; i <15; i++)
    {
        printf(mainstring[i]);
    }
    printf("Pratos entregues:\n");
    for(i = 0; i < data->index; i++)
    {
        if (data->entregues[i] == 0){
            printf("Camarão.\n");
        }
        else if (data->entregues[i] == 1){
            printf("Sushi.\n");
        }
        else if (data->entregues[i] == 2){
            printf("Peixe Frito.\n");
        }
        else{
            printf("Todos os pedidos aceitos foram entregues.\n");
        }
    }
}

void cozinhar_arroz(int id, DadosVisuais* dados_visuais){
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 4;
    show_data(dados_visuais);
    sem_post(show);
    sleep_ms(4000);
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 0;
    show_data(dados_visuais);
    sem_post(show);
}

void cozinhar_camarao(int id, DadosVisuais* dados_visuais){
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 4;
    show_data(dados_visuais);
    sem_post(show);
    sleep_ms(1000);
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 0;
    show_data(dados_visuais);
    sem_post(show);
}

void cortar_camarao(int id, DadosVisuais* dados_visuais){
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 2;
    show_data(dados_visuais);
    sem_post(show);
    sleep_ms(3000);
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 0;
    show_data(dados_visuais);
    sem_post(show);
}

void cortar_peixe(int id, DadosVisuais* dados_visuais){
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 2;
    show_data(dados_visuais);
    sem_post(show);
    sleep_ms(2000);
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 0;
    show_data(dados_visuais);
    sem_post(show);
}

void fritar(int id, DadosVisuais* dados_visuais){
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 4;
    show_data(dados_visuais);
    sem_post(show);
    sleep_ms(2000);
    sem_wait(show);
    dados_visuais->status_cozinheiros[id] = 0;
    show_data(dados_visuais);
    sem_post(show);
}

void entregar(Armazem* armazem,int resultado,sem_t* checagem){//

    sem_wait(armazem->acesso);
    (armazem->prontos)[resultado]++;
    sem_post(armazem->acesso);
    sem_post(checagem);
}

void pegar_ingrediente(Armazem* armazem,int ingrediente){//

    sem_wait(armazem->acesso);
    (armazem->reservados)[ingrediente]--;
    sem_post(armazem->acesso);
}

void pegar_receita_pronta(Armazem* armazem, Receita* receita){

    sem_wait(armazem->acesso);
    for(int i=0;i<receita->n;i++){
        armazem->prontos[receita->etapas[i]]-=1;
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
    DadosVisuais* dados_visuais = argumentos.dados_visuais;
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
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 0;
            show_data(dados_visuais);
            sem_post(show);
            pegar_ingrediente(armazem,0);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 3;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(panelas);
            cozinhar_arroz(id, dados_visuais);
            sem_post(panelas);
            entregar(armazem,0,checagem);
        }
        else if(tarefa_atual==1){//preparar camarao
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 0;
            show_data(dados_visuais);
            sem_post(show);
            pegar_ingrediente(armazem,1);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 1;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(facas);
            cortar_camarao(id, dados_visuais);
            sem_post(facas);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 3;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(panelas);
            cozinhar_camarao(id, dados_visuais);
            sem_post(panelas);
            entregar(armazem,1,checagem);
        }
        else if(tarefa_atual==2){//cortar peixe
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 0;
            show_data(dados_visuais);
            sem_post(show);
            pegar_ingrediente(armazem,2);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 1;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(facas);
            cortar_peixe(id, dados_visuais);
            sem_post(facas);
            entregar(armazem,2,checagem);
        }
        else if(tarefa_atual==3){//fritar peixe
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 0;
            show_data(dados_visuais);
            sem_post(show);
            pegar_ingrediente(armazem,2);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 1;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(facas);
            cortar_peixe(id, dados_visuais);
            sem_post(facas);
            sem_wait(show);
            dados_visuais->status_cozinheiros[id] = 3;
            show_data(dados_visuais);
            sem_post(show);
            sem_wait(panelas);
            fritar(id, dados_visuais);
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
        sem_wait(pedidos_aceitos->acesso);
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
        sem_post(pedidos_aceitos->acesso);
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

    DadosVisuais dados_visuais;

    for (int i = 0; i < N_COZINHEIROS; i++)
    {
        dados_visuais.status_cozinheiros[i] = 0;
    }

    dados_visuais.index = 0;

    sem_t* panelas = (sem_t*)malloc(sizeof(sem_t));
    sem_t* facas = (sem_t*)malloc(sizeof(sem_t));
    sem_t* checagem = (sem_t*)malloc(sizeof(sem_t));
    show = (sem_t*)malloc(sizeof(sem_t));
    Armazem* armazem = (Armazem*)malloc(sizeof(Armazem));

    dados_visuais.armazem = armazem;

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
    sem_init(show,0,1);
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
    sem_wait(show); printf("Insira o numero de pedidos:\n");
    sem_post(show);
    scanf("%d", &n_pedidos);
    int* pedidos_entrada = (int*)malloc(n_pedidos*sizeof(int));
    sem_wait(show); printf("Insira os pedidos (0 para camarao, 1 para sushi e 2 para peixe frito):\n");
    sem_post(show);
    for (int i = 0; i < n_pedidos; i++)
    {
        int pedido;
        scanf("%d", &pedido);
        pedidos_entrada[i] = pedido;
    }
    sem_wait(show); printf("Todos os pedidos foram recebidos.\n");
    sem_post(show);
    GrupoPedidos pedidos_aceitos;
    dados_visuais.pedidos = &pedidos_aceitos;
    pedidos_aceitos.acesso = (sem_t*)malloc(sizeof(sem_t));
    sem_init(pedidos_aceitos.acesso,0,1);
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
        args[i].dados_visuais = &dados_visuais;
        args[i].id=i;
        pthread_create(&cozinheiros[i], NULL, &f_cozinheiro, &args[i]);
    }
    /* logica main*/
    sem_wait(show);
    show_data(&dados_visuais);
    sem_post(show);
    sleep_ms(2000);
    while(n_aceitos + n_recusados < n_pedidos || n_completos < n_aceitos)
    {
        sem_wait(checagem);
        sem_wait(show);
        show_data(&dados_visuais);
        sem_post(show);

        sem_wait(pedidos_aceitos.acesso);
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
            sem_wait(show);
            dados_visuais.entregues[dados_visuais.index]=dump->receita->id;
            dados_visuais.index++;
            sem_post(show);
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
                sem_wait(show);
                dados_visuais.entregues[dados_visuais.index]=dump->receita->id;
                dados_visuais.index++;
                sem_post(show);
                free(dump);
                entregue +=1;
            }
            else
            {
                current = current->next;
            }
        }
        sem_post(pedidos_aceitos.acesso);
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
        sem_wait(show);
        show_data(&dados_visuais);
        sem_post(show);

    }

    working = 0;
    sem_wait(show);
    dados_visuais.entregues[dados_visuais.index]=999;
    dados_visuais.index++;
    show_data(&dados_visuais);
    sem_post(show);

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
    free(show);
    free(pedidos_aceitos.acesso);
    return 0;

}
