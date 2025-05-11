#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include "funciones.c"
#include "log_rpc.h" 

#define MAX_BUFFER 2048

struct argumentos { // Nesetamos mandar el sd y la ip, puerto del cliente
        int sd;
        struct sockaddr_in addr;
};

struct argumentos_rpc {
    CLIENT * cliente;
    struct arg * argumentos;
};
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

void *log_operation_encapsulados(void *data) {
    struct argumentos_rpc *arg = (struct argumentos_rpc *)data;
    log_operation_1(arg->argumentos, arg->cliente);
    return NULL;
}

void *tratar_cliente(void *arg) {
    // Pasamos como argumentos el sd, ip y puerto
    struct argumentos *info = (struct argumentos *)arg;
    int sd = info->sd;
    struct sockaddr_in addr = info->addr;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    free(info); 

    char buffer[2048];
    int n = recv(sd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
    }
    int res;
    char user[256];
    char fileName[256];
    char description[256];
    char remote_FileName[256];
    char local_FileName[256];
    char status;
    char * string;
    char operacion = buffer[0];
    char datetime[20];
    CLIENT *clnt;  // Puntero al RPC
    struct arg arg_rpc;  // Argumentos RPC
    arg_rpc.user = malloc(256 * sizeof(char));
    arg_rpc.operacion = malloc(256 * sizeof(char));
    arg_rpc.datetime = malloc(256 * sizeof(char));
    strncpy(user, buffer + 1, 256);
    strncpy(datetime, buffer + 257, 19);
    strncpy(arg_rpc.user, user, 256);
    strncpy(arg_rpc.datetime, datetime, 256);

    char *LOG_RPC_IP = getenv("LOG_RPC_IP");
    clnt = clnt_create(LOG_RPC_IP, LOGRPC, LOGRPC_V1, "udp");

    switch (operacion) {
        case '0':
            printf("OPERACION REGISTER FROM %s\n", user);
            status = '0' + registrar(user);
            arg_rpc.operacion = "REGISTER";
            break;
        case '1':
            printf("OPERACION UNREGISTER FROM %s\n", user);
            status = '0' + unregistrar(user);
            arg_rpc.operacion = "UNREGISTER";
            break;
        case '2':
            printf("OPERACION CONNECT FROM %s\n", user); 
            uint16_t temp;           
            memcpy(&temp, buffer + 257, 2);
            strncpy(datetime, buffer + 259, 19);
            int puerto = ntohs(temp);
            status = '0' + conectar(user, ip, puerto);
            arg_rpc.operacion = "CONNECT";
            break;
        case '3':
            printf("OPERACION DISCONNECT FROM %s\n", user);
            status = '0' + desconectar(user);
            arg_rpc.operacion = "DISCONNECT";
            break;
        case '4':
            strncpy(fileName, buffer + 257, 256);
            strncpy(description, buffer + 513, 256);
            strncpy(datetime, buffer + 769, 19);
            printf("OPERACION PUBLISH FROM %s\n", user);
            status = '0' + publish(user, fileName, description);
            sprintf(arg_rpc.operacion, "PUBLISH %s",fileName);
            break;
        case '5':
            strncpy(fileName, buffer + 257, 256);
            strncpy(datetime, buffer + 513, 19);
            printf("OPERACION DELETE FROM %s\n", user);
            status = '0' + delete(user, fileName);
            sprintf(arg_rpc.operacion, "DELETE %s",fileName);
            break;
        case '6':
            printf("OPERACION LIST_USERS FROM %s\n", user);  
            res = listusers(user, &string);
            if (res == 0) {
                status = 'm';
            } else {
                status = '0' + res;
            }
            arg_rpc.operacion = "LIST_USERS";
            break;
        case '7':
            printf("OPERACION LIST_CONTENT FROM %s\n", user);
            res = listcontent(user, &string);
            if (res == 0) {
                status = 'm';
            } else {
                status = '0' + res;
            }
            arg_rpc.operacion = "LIST_CONTENT";
            break;
        case '8':
            strncpy(remote_FileName, buffer + 257, 256);
            strncpy(local_FileName, buffer + 513, 256);
            strncpy(datetime, buffer + 769, 19);
            printf("OPERACION GET_FILE FROM %s\n", user);
            status = '0' + getfile(user, remote_FileName, local_FileName);
            arg_rpc.operacion = "GET_FILE";
            break;
        default:
            printf("Operación no reconocida: %c\n", operacion);
    }
    struct argumentos_rpc argumentos_rpc;
    
    argumentos_rpc.cliente = clnt;
    argumentos_rpc.argumentos = &arg_rpc;

    pthread_t hilo;
    pthread_create(&hilo, NULL, log_operation_encapsulados, (void *)&argumentos_rpc);
    pthread_detach(hilo);  // Iba muy lento si lo haciamos en el main
    printf("%s\n", datetime);
    
    if (status == 'm') {
        size_t tamano = strlen(string);
        char *mensaje = malloc(tamano + 2);
    
        mensaje[0] = status;             // Metemos el status lo primero. Aquí estará m como la flag de more que se utiliza en UDP
        mensaje[1] = (char)strlen(string);
        strcpy(mensaje + 2, string);
    
        sendMessage(sd, mensaje, tamano + 2);

        free(string);
        free(mensaje);
    } else {
        sendMessage(sd, &status, 1);
    }
    close(sd);
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "Uso: %s -p <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    int puerto = atoi(argv[2]);
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
        struct argumentos *argumentos;
        argumentos = malloc(sizeof(struct argumentos));
        argumentos->addr = client_addr;
        client_len = sizeof(client_addr);
        argumentos->sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
        if (argumentos->sd < 0) {
            perror("Error en accept");
            free(client_sd);
            continue;
        }
        
        pthread_t hilo;
        pthread_create(&hilo, NULL, tratar_cliente, (void *)argumentos);
        pthread_detach(hilo);  // Liberar recursos automáticamente
    }
    
    close(server_sd);
    return 0;
}
