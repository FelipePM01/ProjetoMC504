#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_THR 10

void* f_thread(void *v) {
  int id = (int) v;
  printf("Thread %d\n", id);  
  return NULL; 
} 

int main(){
    int i, j;
    pthread_t thr[N_THR][N_THR];
    for (i = 0; i < N_THR; i++){
        for (j = 0; j < N_THR;j++){
            pthread_create(&thr[i][j], NULL, f_thread, (void*) i);
        }
    }
    printf("Hello World!");
}