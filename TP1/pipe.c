#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define AMMOUNT 1000

int prime(int n) {
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

int generate(int n) {
	int r = rand() % 100;
	return n + r;
}

void signal_handler(int sig) {
    char s1[] = "SIGUSR1\n";
    char s2[] = "SIGUSR2\n";
    if (sig == SIGUSR1) {
        write(STDOUT_FILENO, s1, sizeof(s1));
    } else if (sig == SIGUSR2) {
        write(STDOUT_FILENO, s2, sizeof(s2));
    }
    signal(sig, signal_handler);
}


int main()
{

    int p1[2];
    pipe(p1);

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    int pid = fork();
    if (pid == -1) {
        return 1;
    }

    if (pid == 0) {
        //Produtor
        int product = 100;
        int i = 0;

        for (i = 1; i < AMMOUNT; ++i) {
            
            write(p1[1], &product, sizeof(product));
            
            product = generate(product);
            printf("Produtor: %i disponibilizado\n", product);
            usleep(5000);
        }

        product = 0;
        
        write(p1[1], &product, sizeof(product));
        
        printf("End of production\n");
        kill(pid, SIGKILL);
    
    } else {
        //Consumidor
        int product = 1;
        int productNew = 1;


        while(product != 0) {
            read(p1[0], &productNew, sizeof(productNew));
            
            printf("Consumidor: analisando produto %i \n", product);
            if(productNew != product) {
                product = productNew;

                if(prime(product) == 1) {
                    printf("Consumidor: %i \n", product);
                }                
            }
            usleep(5);
        }
    }

    return 0;
}