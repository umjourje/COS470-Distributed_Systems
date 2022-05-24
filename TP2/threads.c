#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

//gcc -o threads threads.c -pthread
//./threads

const int K = 1; //Número de threads paralelas. Deve ser divisor de N/2
//const int N = 100;
//const int N = 10000000; //10^7
//const int N = 10000000; //10^8
const int N = 100000000; //10^9


long resultado = 0;
char *valores;


//Lock e mutex
volatile atomic_flag lock = ATOMIC_FLAG_INIT;
pthread_mutex_t mutex;

int acquire() {
    while(atomic_flag_test_and_set_explicit(&lock, memory_order_acquire));
}

int release() {
    atomic_flag_clear_explicit(&lock, memory_order_release);
}

//Forma errada e ineficiente de fazer o lock, conforme foi solicitado
void lock_errado(int value) {
    acquire();
    resultado = resultado + value;
    release();
}

//Forma correta e mais eficiente de fazer o lock, mas que não foi solicitada
void lock_correto(int value) {
	pthread_mutex_lock(&mutex);   // Entra na regiao critica
	{
		resultado = resultado + value;
	}
	pthread_mutex_unlock(&mutex); // Sai da regiao critica
}

//Function que será rodada pela thread
void* soma_multiplos(void *args) {

   long tamanho = N / K;
   long index = (*(int*)args) - 1;

   long offset = tamanho * index;

   long soma = 0;
   printf("Thread started witn index n: %i\n", index);

   for(long i = 0; i < tamanho; i++) {
       soma = soma + valores[offset + i];
       //printf("indice %i valor %i\n", offset + i, valores[offset + i]);
   }
   lock_errado(soma);
   // lock_certo(soma);
}

int main(int argc, char* argv[]) {

    valores = malloc(N);

    //Gera os números a serem somados
    for(long i = 0; i < N; i++) {
        //valores[i] = (char) (rand() % 200) - 100;
        valores[i] = (char) 1; //Debug
    }

    pthread_t th[K];
    clock_t t;
    t = clock();

    //Main loop
    for(int i = 0; i < K; i++) {
        pthread_create(&th[i], NULL, &soma_multiplos, &i);
    }

    for(int i = 0; i < K; i++) {
        pthread_join(th[i], NULL);
    }

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // ms
    printf("Tempo para calcular: %f ms\n", time_taken * 1000);
    printf("Valor total: %i\n", resultado);

    return 0;
}



