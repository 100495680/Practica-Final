#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>

#define MAX_BUFFER 2048

// Funciones tipo de mandado y recepción de mensajes

int sendMessage(int socket, char * buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = write(socket, buffer, l);
		l = l -r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* full length has been sent */
}
int recvMessage(int socket, char *buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = read(socket, buffer, l);
		l = l -r ;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo */
	else
		return(0);	/* full length has been receive */
}


void *tratar_cliente(void *arg) {
    int sd = *(int *)arg;
    free(arg);
    char buffer[MAX_BUFFER];

    int n = recv(sd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Mensaje: %s\n", buffer);
    }

    char status = 0;
    
    sendMessage(sd, &status, 1);
    close(sd);
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    int puerto = atoi(argv[1]);
    int server_sd, *client_sd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0) {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(puerto);

    if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_sd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sd, 10) < 0) {
        perror("Error en listen");
        close(server_sd);
        exit(EXIT_FAILURE);
    }
    printf("init server %s: %d\n",inet_ntoa(server_addr.sin_addr), puerto);
    fflush(stdout);

    while (1) {
        client_sd = malloc(sizeof(int));
        client_len = sizeof(client_addr);
        *client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
        if (*client_sd < 0) {
            perror("Error en accept");
            free(client_sd);
            continue;
        }
        printf("Ejecutando funcion..");
        fflush(stdout);
        
        pthread_t hilo;
        pthread_create(&hilo, NULL, tratar_cliente, client_sd);
        pthread_detach(hilo);  // Liberar recursos automáticamente
    }
    
    close(server_sd);
    return 0;
}
