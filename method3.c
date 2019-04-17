/*Este programa cria N threads que alteram o conteúdo
de uma variável global. Como não estão sendo utilizados
mecanismos de sincronização, o conteúdo da variável
se tornará inconsistente ao executar o programa.*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#define NUM_THREADS 5

int x = 0;
int *lock;
int waiting[5];

void *threadBody(void *id)
{
    long tid = (long)id;
    long nextTid;
    long key;

    int i;
    //Atualiza a variável global
    for (i = 0; i < 10000; i++)
    {
        waiting[tid] = 1;
        key = 1;
        while (waiting[tid] && key){
            key = (long) __sync_lock_test_and_set(&lock, 1);
        }
        waiting[tid] = 0;
        x++;

        nextTid = (tid + 1) % NUM_THREADS;
        while (nextTid != tid && !waiting[nextTid]){
            nextTid = (nextTid + 1) % NUM_THREADS;
        }

        if (nextTid == tid)
            __sync_lock_release(&lock);
        else
            waiting[nextTid] = 0;

    }
        pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t thread[NUM_THREADS];
    long i, status;
    for (i = 0; i < NUM_THREADS; i++)
    {
        status = pthread_create(&thread[i], NULL, threadBody, (void *)i);
    }
    for (i = 0; i < NUM_THREADS; i++)
    {
        status = pthread_join(thread[i], NULL);
    }

    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    printf("\nInvoluntarias: %5ld \nVoluntarias: %5ld", ru.ru_nivcsw, ru.ru_nvcsw);

    printf("\n\nValor final de x: %02d\n\n", x);
    pthread_exit(NULL);
}