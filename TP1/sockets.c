#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX     80
#define PORT    8080
#define BUSY    1
#define AMMOUNT 10
#define SA struct sockaddr

// Parte Produtor (Servidor) - Gera e Envia Primo; Recebe de volta resposta bool

int generate_random(int n) {
    int r = rand() % 100;
    return n + r;
}

// Parte Consumidor (Cliente) - Espera Sinal; quando receber, verifica numero primo

int check_prime(int n) {
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

// Produtor - Inicia Conexao
void produtor(int connection, int process, int socket) {

        int product = 100;
        int productNew = 0;
        int i = 0;

        for (i = 1; i < AMMOUNT; ++i) {
            // open connection
            if (BUSY == 1) {
                write(connection, &product, sizeof(product));
            }
            else {
                // kill(process, SIGSTOP);
                write(connection, &product, sizeof(product));
                // kill(process, SIGCONT);    
            }
            
            product = generate_random(product);
            printf("Produtor: %i disponibilizado\n", product);
            usleep(5000);
            close(socket);
            // read(connection, &productNew, sizeof(productNew));
            // if (productNew == product) {
            //     printf("Achei um primo: %i \n", product);
            // }
        }

        product = 0;
        
        write(connection, &product, sizeof(product));
        
        printf("End of production\n");
}

void consumidor(int connection) {

    int product = 1;
    int productNew = 1;

    while(product != 0) {
        
        read(connection, &productNew, sizeof(productNew));
            
        printf("Consumidor: analisando produto %i \n", product);
        if(productNew != product) {
            product = productNew;

            if(check_prime(product) == 1) {
                printf("Consumidor %i avaliado. Enviando resultado! \n", product);
                //write(connection, &product, sizeof(product));
            }                
        }
        usleep(50);
    }
}


int main()
{

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    int sock, conn, len;
    struct sockaddr_in servaddr, cli;
    
    // instancia os processos
    int pid = fork();

    if (pid == -1) {
        return 1;
    }

    if (pid == 0) {
        //Consumidor

        // IPv4, data stream, TCP Protocol
        sock = socket(AF_INET, SOCK_STREAM, 6);
        // Error socket creation
        if (sock == -1) {
            return 5;
        }

        // Assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        // connect the client socket to server socket
        if (connect(sock, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            return 6;
        }

        consumidor(sock);

        close(sock);
        printf("Conexao consumidor fechada! \n");

    } else {
        //Produtor
        // IPv4, data stream, TCP Protocol
        sock = socket(AF_INET, SOCK_STREAM, 6);
        // Error socket creation
        if (sock == -1) {
            return 1;
        }
        // Assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        if ((bind(sock, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            return 2;
        }

        // Now server is ready to listen and verification
        if ((listen(sock, 5)) != 0) {
            return 3;
        }

        len = sizeof(cli);

        // Accept the data packet from client and verification
        conn = accept(sock, (SA*)&cli, &len);

        // connection error
        if (conn < 0) {
            return 4;
        }
        // Function for produtor logic
        produtor(conn, pid, sock);
        
        printf("Conexao produtor fechada! \n");
    }

    return 0;
}


