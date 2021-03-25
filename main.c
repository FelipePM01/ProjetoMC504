#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_THR 10

typedef struct {
    int* matrix;
    int* res;
    int i;
    int j;
} Args;

void *regra(void* arg)
{
    Args valores = *(Args*) arg;
    int* matrix = valores.matrix;
    int* res = valores.res;
    int i = valores.i;
    int j = valores.j;
    int n_vizinhos = 0;
    for (int i2 = i-1; i2 < i+2; i2++)
    {
        for (int j2 = j-1; j2 < j+2; j2++)
        {
            if (i2 == i && j2 == j)
            {
                continue;
            }
            else if (matrix[i2*(N_THR+2)+j2] == 1)
            {
                n_vizinhos++;
            }
        }
    }
    if (matrix[i*(N_THR+2)+j] == 0)
    {
        if (n_vizinhos == 3)
        {
            res[i*(N_THR+2)+j] = 1;
        }
        else
        {
            res[i*(N_THR+2)+j] = 0;
        }
    }
    else
    {
        if (n_vizinhos == 2 || n_vizinhos == 3)
        {
            res[i*(N_THR+2)+j] = 1;
        }
        else
        {
            res[i*(N_THR+2)+j] = 0;
        }
    }
}

int main(){
    pthread_t thr[N_THR][N_THR];
    Args args[N_THR][N_THR];

    int mat[(N_THR+2)*(N_THR+2)];
    int mat2[(N_THR+2)*(N_THR+2)];

    for (int i = 0; i < (N_THR+2); i++)
    {
        for (int j = 0; j < (N_THR+2); j++)
        {
            mat[i*(N_THR+2)+j] = 0;
            mat2[i*(N_THR+2)+j] = 0;
        }
    }
    mat[1*(N_THR+2)+3] = 1;
    mat[2*(N_THR+2)+1] = 1;
    mat[2*(N_THR+2)+3] = 1;
    mat[3*(N_THR+2)+2] = 1;
    mat[3*(N_THR+2)+3] = 1;
    int* p1 = mat;
    int* p2 = mat2;
    while(1)
    {
        for (int i = 0; i < N_THR; i++){
            for (int j = 0; j < N_THR;j++){
                args[i][j].i = i+1;
                args[i][j].j = j+1;
                args[i][j].matrix = p1;
                args[i][j].res = p2;
                pthread_create(&thr[i][j], NULL, &regra, (void*) &args[i][j]);

            }

        }
        for (int i = 0; i < N_THR; i++){
            for (int j = 0; j < N_THR;j++){
                pthread_join(thr[i][j], NULL);
            }
        }
        for (int i = 1; i < N_THR+1; i++){
            for (int j = 1; j < N_THR+1;j++){
                printf("%d", p2[i*(N_THR+2)+j]);
            }
            printf("\n");
        }
        printf("\n");
        int* temp = p1;
        p1 = p2;
        p2 = temp;
        sleep(1);
    }


    printf("Hello World!");
}
