int registrar (char *user);
int unregistrar (char *user);
int conectar (char *user,  char * ip, int puerto);
int desconectar (char *user);
int publish (char * user, char *fileName, char *description);
int delete (char * user, char *fileName);
int listusers (char *user, char ** string);
int listcontent (char *user, char ** string);
int getfile (char *user, char *remote_FileName, char *local_FileName);

#define MAX_BUFFER 2048


struct Archivos {
    char fileName[256];
    char description[256];
};

struct Usuarios {
    char user[256];
    char ip[16];
    int puerto;
    struct Archivos * lista_archivos;
    int cantidad_archivos;
};


// Las funciones y declaraciones asociadas a la lista dinámica de usuarios

struct Usuarios * lista_usuarios = NULL; 
int capacidad_usuarios = 0;

void actualizar_lista_usuarios(struct Usuarios usuario ) {
    capacidad_usuarios ++;
    lista_usuarios = (struct Usuarios *)realloc(lista_usuarios, capacidad_usuarios * sizeof(struct Usuarios));
    lista_usuarios[capacidad_usuarios-1] = usuario;
}

struct Usuarios * buscar_usuario(char * user) {
    for (int i = 0; i<capacidad_usuarios; i++) {
        if (strcmp(lista_usuarios[i].user, user) == 0 )
        return &lista_usuarios[i];
    }
    return NULL;
}

int borrar_usuario(char *user) {
    int encontrado = -1;
    for (int i = 0; i < capacidad_usuarios; i++) {
        if (strcmp(lista_usuarios[i].user, user) == 0) {
            encontrado = i;
            break;
        }
    }
    if (encontrado == -1) return 1;
    
    for (int j = encontrado; j < capacidad_usuarios - 1; j++) {
        lista_usuarios[j] = lista_usuarios[j + 1];
    }
    capacidad_usuarios--;
    lista_usuarios = realloc(lista_usuarios, capacidad_usuarios * sizeof(struct Usuarios)); // Parecido a un free pero con más pasos
    return 0;
}

// Las funciones y declaraciones asociadas a las listas dinámicas de los libros de los usuarios

void actualizar_lista_archivos(struct Archivos **lista, int *capacidad, struct Archivos archivo) {
    (*capacidad)++;
    *lista = realloc(*lista, (*capacidad) * sizeof(struct Archivos));
    (*lista)[(*capacidad) - 1] = archivo;
}


struct Archivos * buscar_archivo(struct Archivos * lista, int capacidad, char * fileName) {
    for (int i =0; i<capacidad; i++) {
        if (strcmp(lista[i].fileName, fileName) == 0 )
        return &lista[i];
    }
    return NULL;
}

int borrar_archivo(struct Archivos * lista, int capacidad, char * fileName) {
    int encontrado = -1;
    for (int i = 0; i < capacidad; i++) {
        if (strcmp(lista[i].fileName, fileName) == 0) {
            encontrado = i;
            break;
        }
    }
    if (encontrado == -1) return 1;
    
    for (int j = encontrado; j < capacidad - 1; j++) {
        lista[j] = lista[j + 1];
    }
    capacidad--;
    lista = realloc(lista, capacidad * sizeof(struct Usuarios)); // Parecido a un free pero con más pasos
    return 0;
}



// ---------------------------------------------------------------
// Definición de funciones del servidor

int registrar (char * user) {
    struct Usuarios usuario;
    strncpy(usuario.user, user, 256);
    strncpy(usuario.ip, "",16);
    usuario.puerto = 0;
    usuario.cantidad_archivos = 0;
    usuario.lista_archivos = malloc(usuario.cantidad_archivos);
    struct Usuarios * prueba = buscar_usuario(user);
    if (prueba == NULL) {
        actualizar_lista_usuarios(usuario);
        return 0;
    }
    return 1;
}

int unregistrar (char * user) {
    return borrar_usuario(user);
}

int conectar (char * user,  char * ip, int puerto) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (strcmp(ptr->ip, "") != 0) return 2;
    strncpy(ptr->ip, ip, 16);
    ptr->puerto = puerto;
    return 0;
}

int desconectar (char * user) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (strcmp(ptr->ip, "") == 0) return 2;
    strcpy(ptr->ip, "");
    ptr->puerto = 0;
    return 0;
}

int publish (char * user, char * fileName, char * description) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    if (strcmp(ptr->ip, "") == 0) return 2;
    if (buscar_archivo(ptr->lista_archivos, ptr->cantidad_archivos, fileName) != NULL) return 3;
    struct Archivos archivo;
    strncpy(archivo.fileName, fileName, 256);
    archivo.fileName[255] = '\0';
    strncpy(archivo.description, description, 256);
    archivo.description[255] = '\0';
    actualizar_lista_archivos(&ptr->lista_archivos, &ptr->cantidad_archivos, archivo);
    return 0;
}

int delete (char * user, char * fileName) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    printf("%s/%s", ptr->ip, user);
    if (strcmp(ptr->ip, "") == 0) return 2;
    if (buscar_archivo(ptr->lista_archivos, ptr->cantidad_archivos, fileName) == NULL) return 3;
    borrar_archivo(ptr->lista_archivos, ptr->cantidad_archivos, fileName);
    return 0;
}

int listusers(char * user, char ** string) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (strcmp(ptr->ip, "") == 0) return 2;

    int string_size = capacidad_usuarios * 280; // Son 256 del usuario y algo más para los separadores
    string_size ++;
    *string = malloc(string_size);
    *string[0] = '\0';
    char formateo[800];
    for (int i = 0; i < capacidad_usuarios; i++) {
        snprintf(formateo, sizeof(formateo), "\t%s\t%s\t%d", lista_usuarios[i].user, lista_usuarios[i].ip, lista_usuarios[i].puerto);
        strcat(*string, formateo);
        if (i < capacidad_usuarios - 1) {
            strcat(*string, "\n"); 
        }
    }
    return 0;
}

int listcontent(char * user, char ** string) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    if (strcmp(ptr->ip, "") == 0) return 2;

    int capacidad = ptr->cantidad_archivos;
    int string_size = capacidad * 280; // Son 256 del usuario y algo más para los separadores
    string_size ++;
    *string = malloc(string_size);
    *string[0] = '\0';
    strcat(*string, "\t");
    for (int i = 0; i < capacidad; i++) {
        strcat(*string, ptr->lista_archivos[i].fileName);
        strcat(*string, "\n"); 
        if (i < capacidad - 1) {
            strcat(*string, "\t"); 
        }
    }
    return 0;
}

int getfile(char *user, char *remote_FileName, char *local_FileName) {
    printf("Dentro");
    fflush(stdout);
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    if (strcmp(ptr->ip, "") == 0) return 2;

    for (int j = 0; j<capacidad_usuarios; j++) {
        int capacidad = lista_usuarios[j].cantidad_archivos;
        for (int i = 0; i<capacidad; i++) {
            if (strcmp(lista_usuarios[j].lista_archivos[i].fileName, remote_FileName) == 0 ) {
            
                int puerto = lista_usuarios[j].puerto;
                struct sockaddr_in server_addr;
                FILE *fp;
                printf("Datos Sacados");
                fflush(stdout);
                // Nos conectamos al usuario y mandamos la petición
                int sockfd = socket(AF_INET, SOCK_STREAM, 0);
                memset(&server_addr, 0, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(puerto);
                printf("Trying to connect to IP: %s, Port: %d\n", lista_usuarios[j].ip, puerto);
                fflush(stdout);

                if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                    perror("Connection failed");
                    close(sockfd);
                    return 6;
                }

                //inet_pton(AF_INET, lista_usuarios[j].ip, &server_addr.sin_addr);
                //connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
                send(sockfd, remote_FileName, strlen(remote_FileName), 0);
                printf("Socket Creado");
                fflush(stdout);
                // Creamos el archivo a recibir
                fp = fopen(local_FileName, "wb");
                printf("Archivo abierto");
                fflush(stdout);
                // Recibimos los datos y los guardamos
                char buffer[MAX_BUFFER];
                ssize_t bytes_received;
                while ((bytes_received = recv(sockfd, buffer, MAX_BUFFER, 0)) > 0) {
                    fwrite(buffer, 1, bytes_received, fp);
                }
                printf("Archivo enviado");
                fflush(stdout);
                fclose(fp);
                close(sockfd);
                return 0;
            }
        }
    }
    return 3;
}