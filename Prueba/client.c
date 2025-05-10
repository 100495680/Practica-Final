#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(46617);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar");
        return 2;
    }

    char local_FileName[256];
    strncpy(local_FileName, "t.txt", 256);
    FILE * fp;
    fp = fopen(local_FileName, "wb");
    // Enviar request de 256 bytes
    char remote_FileName[256];
    strncpy(remote_FileName, "a/text.txt",256);
    send(sockfd, remote_FileName, strlen(remote_FileName), 0);
    // Leer respuesta grande
    ssize_t bytes;
    while ((bytes = recv(sockfd, buffer, MAX_BUFFER, 0)) > 0) {
        fwrite(buffer, 1, bytes, fp);
    }

    close(sockfd);
    return 0;
}
