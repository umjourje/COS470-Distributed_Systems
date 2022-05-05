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
#define AMMOUNT 100
#define SA struct sockaddr


#define SOL_TCP 6  // socket options TCP level
#define TCP_USER_TIMEOUT 18  // how long for loss retry before timeout [ms]


int timeout = 10000;  // user timeout in milliseconds [ms]

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
    char s1[] = "Ack, iniciando\n";
    char s2[] = "Ack, preparando para encerrar\n";
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
        int result = 0;
        int i = 0;

        kill(process, SIGUSR1);

        for (i = 1; i < AMMOUNT; ++i) {
            // open connection
            write(connection, &product, sizeof(product));

            // Gera um produto e envia no socket
            product = generate_random(product);
            //printf("Produtor: %i disponibilizado\n", product);
            usleep(5000);
            
            // Verifica se recebeu um número diferente de 0 (que será primo)
            read(connection, &result, sizeof(result));
            if (result != 0) {
                printf("Achei um primo: %i \n", result);
            }
            close(socket);
        }

        // Encerra a produção
        kill(process, SIGUSR2);
        product = 0;        
        write(connection, &product, sizeof(product));
        
        printf("End of production\n");
        usleep(5000);
}

void consumidor(int connection) {

    int product = 1;
    int productNew = 1;
    
    while(product != 0) {

        int result = 0;
        read(connection, &productNew, sizeof(productNew));
        
        //printf("Consumidor: analisando produto %i \n", product);
        if(productNew != product) {
            product = productNew;
            
            // Verifica se o produto não é 0
            if(product == 0) {
                printf("Recebido 0. Encerrando\n");
            } else {
                // Verifica se é primo. Se for, prepara o resultado para o retorno
                if(check_prime(product) == 1) {
                    //printf("Primo %i encontrado. Enviando resultado! \n", product);
                    result = product;
                }
            }
        }
        // Envia um resultado para o produtor. Se for primo, envia o número gerado. Se não for, envia 0
        write(connection, &result, sizeof(result));
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

        // Set socket timeout
        // setsockopt (sock, SOL_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof (timeout));

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

        // Set socket timeout
        // setsockopt (sock, SOL_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof (timeout));

        // Assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        int yes=1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            return 7;
        }

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
