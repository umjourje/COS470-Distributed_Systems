#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#define N 2

const int K = 2; //Número de threads paralelas. Deve ser divisor de N/2
const int M = 10000; //10^5
// const int Np = 1;
// const int Nc = 1;

sem_t mutex;
sem_t empty;
sem_t full;

int contador = M;
int position = 0;
int BUFFER[N];

// Para o estudo de caso, considere que o programa termina sua 
// execução após o consumidor processar M = 10^5 números.  
// Considere ainda os valores N = 1,2,4,16,32, com os seguintes
// combinações de número de threads produtor/consumidor:
// (Np, Nc) = {(1,1),(1,2),(1,4),(1,8),(2,1),(4,1),(8,1)}

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
        sem_wait(&empty);
        sem_wait(&mutex);
            int num = generate_random();
            BUFFER[position] = num; // escreve no array
            position++;
        sem_post(&mutex); // libera semaforo
        sem_post(&full);
    }
}

void* consumidor (void* args) {
    int i;
    while (contador > 0) {
        sem_wait(&full);
        sem_wait(&mutex); // lock no semaforo de Leitura
            position--;
            int temp = BUFFER[position];
        sem_post(&mutex); // libera semaforo de Leitura
        sem_post(&empty);

        if (check_prime(temp)) {
            // printf("%i É primo!\n", temp);
            contador = contador-1;
        };
    };
    sem_post(&mutex);
    sem_post(&empty);
}


int main(int argc, char* argv[]) {

    const int Np = atoi(argv[1]);
    const int Nc = atoi(argv[2]);

    clock_t t;
    pthread_t tp[Np];
    pthread_t tc[Nc];
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    t = clock();
    
    //Loop Criação Produtor
    for(int i = 0; i < Np; i++) {
        int params = 0;
        pthread_create(&tp[i], NULL, &produtor, &params);
    }

    //Loop Criação Consumidor
    for(int i = 0; i < Nc; i++) {
        int params = 0;
        pthread_create(&tc[i], NULL, &consumidor, &params);
    }

    // Join Produtor
    for(int i = 0; i < Np; i++) {
        pthread_join(tp[i], NULL);
    }

    // Join Consumidor
    for(int i = 0; i < Nc; i++) {
        pthread_join(tc[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // ms
    printf("Tempo para calcular: %f ms\n", time_taken * 1000);

    return 0;




}