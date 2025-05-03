#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

int main() {
    int server_sd, client_sd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind"); exit(1);
    }

    if (listen(server_sd, 10) < 0) {
        perror("listen"); exit(1);
    }

    printf("Esperando conexiones en puerto 8080...\n");

    client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sd < 0) { perror("accept"); exit(1); }

    printf("✔ Cliente conectado\n");

    int n = recv(client_sd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Mensaje: %s\n", buffer);
    }

    close(client_sd);
    close(server_sd);
    return 0;
}
