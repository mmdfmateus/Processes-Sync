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

void *threadBody(void *id)
{
   long tid = (long)id;
   int i;
   //Atualiza a variável global
   for (i = 0; i < 10000; i++)
   {
      while (__sync_lock_test_and_set(&lock, 1))
         ;
      x++;
      __sync_lock_release(&lock);
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