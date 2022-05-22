#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


const int K = 2; //Número de threads paralelas. Deve ser divisor de N/2
const int M = 10; //10^5
const int Np = 1;
const int Nc = 1;

// sem_t mutexRead, mutexWrite;
sem_t mutex;

int contador = M;

// transformar em array
int BUFFER;

// Para o estudo de caso, considere que o programa termina sua 
// execução após o consumidor processar M = 10^5 números.  
// Considere ainda os valores N = 1,2,4,16,32, com os seguintes
// combinações de número de threads produtor/consumidor:
// (Np, Nc) = {(1,1),(1,2),(1,4),(1,8),(2,1),(4,1),(8,1)}.

int generate_random() {
    return (rand() % 100)+1;
}

bool check_prime(int n) {
	int i;
	for(i=2;i<=n/2;i++)
	{
		if(n%i!=0)
			continue;
		else
			return 0;
	}
	return 1;
}


void* produtor (void* args) {
    while(contador > 0){
        int num = generate_random();
        sem_wait(&mutex); // lock no semaforo
        BUFFER = num; // escreve no array
        sem_post(&mutex); // libera semaforo
    }
}

void* consumidor (void* args) {
    int i;
    while (contador > 0) {
        sem_wait(&mutex); // lock no semaforo de Leitura
        int temp = BUFFER;
        sem_post(&mutex); // libera semaforo de Leitura

        // sem_wait(&mutexWrite); // lock no semaforo de Escrita
        // BUFFER=0; // libera a mem 
        // sem_post(&mutexWrite); 
        if (check_prime(temp)) {
            printf("%i É primo!\n", temp);
            contador = contador-1;
        };

        
    }
}


int main(int argc, char* argv[]) {

    clock_t t;
    pthread_t tp[Np];
    pthread_t tc[Nc];
    sem_init(&mutex, 0, 1);
    t = clock();
    
    //Loop Criação Produtor
    for(int i = 0; i < Np; i++) {
        int params = 0;
        pthread_create(&tp[i], NULL, &produtor, &params);
    }
    printf("Estou Aqui 1! \n");

    //Loop Criação Consumidor
    for(int i = 0; i < Nc; i++) {
        int params = 0;
        pthread_create(&tc[i], NULL, &consumidor, &params);
    }
    printf("Estou Aqui 2! \n");

    // Join Produtor
    for(int i = 0; i < Np; i++) {
        pthread_join(tp[i], NULL);
    }
    printf("Estou Aqui 3! \n");

    // Join Consumidor
    for(int i = 0; i < Nc; i++) {
        pthread_join(tc[i], NULL);
    }
    printf("Estou Aqui 4! \n");

    sem_destroy(&mutex);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // ms
    printf("Tempo para calcular: %f ms\n", time_taken * 1000);

    return 0;




}