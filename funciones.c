int registrar (char *user);
int unregistrar (char *user);
int conectar (char *user,  char * ip, int puerto);
int desconectar (char *user);
int publish (char * user, char *fileName, char *description);
int delete (char * user, char *fileName);
int listusers (char *user, char * string);
int listcontent (char *user, char * string);
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
int capacidad_usuarios;

void actualizar_lista_usuarios(struct Usuarios usuario ) {
    capacidad_usuarios ++;
    lista_usuarios = (struct Usuarios *)realloc(lista_usuarios, capacidad_usuarios * sizeof(struct Usuarios));
    lista_usuarios[capacidad_usuarios] = usuario;
}

struct Usuarios * buscar_usuario(char * user) {
    for (int i; i<capacidad_usuarios; i++) {
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

void actualizar_lista_archivos(struct Archivos * lista, int capacidad, struct Archivos archivo ) {
    capacidad ++;
    lista = (struct Archivos *)realloc(lista, capacidad * sizeof(struct Archivos));
    lista[capacidad] = archivo;
}

struct Archivos * buscar_archivo(struct Archivos * lista, int capacidad, char * fileName) {
    for (int i; i<capacidad; i++) {
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
    if (usuario.ip != NULL) return 2;
    strncpy(usuario.ip, ip, 256);
    usuario.puerto = puerto;
    return 0;
}

int desconectar (char * user) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;
    strcpy(usuario.ip, "");
    usuario.puerto = 0;
    return 0;
}

int publish (char * user, char * fileName, char * description) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;
    if (buscar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName) != NULL) return 3;
    struct Archivos archivo;
    strncpy(archivo.fileName, fileName, 256);
    strncpy(archivo.description, description, 256);
    actualizar_lista_archivos(usuario.lista_archivos, usuario.cantidad_archivos, archivo);
    return 0;
}

int delete (char * user, char * fileName) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;
    if (buscar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName) == NULL) return 3;
    borrar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName);
    return 0;
}

int listusers(char * user, char * string) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;

    int string_size = capacidad_usuarios * 280; // Son 256 del usuario y algo más para los separadores
    string_size ++;
    string = malloc(string_size);
    strcat(string, "\t");
    char *formateo;
    for (int i = 0; i < capacidad_usuarios; i++) {
        sscanf(formateo, "%s\t%s\t%d", lista_usuarios[i].user, lista_usuarios[i].ip, &lista_usuarios[i].puerto);
        strcat(string, formateo);
        if (i < capacidad_usuarios - 1) {
            strcat(string, "\n\t"); 
        }
    }
    return 0;
}

int listcontent(char * user, char * string) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;

    int capacidad = usuario.cantidad_archivos;
    int string_size = capacidad * 280; // Son 256 del usuario y algo más para los separadores
    string_size ++;
    string = malloc(string_size);
    strcat(string, "\t");
    for (int i = 0; i < capacidad; i++) {
        strcat(string, usuario.lista_archivos[i].fileName);
        if (i < capacidad - 1) {
            strcat(string, "\n\t"); 
        }
    }
    return 0;
}

int getfile(char *user, char *remote_FileName, char *local_FileName) {
    struct Usuarios * ptr = buscar_usuario(user);
    if (ptr == NULL) return 1;
    struct Usuarios usuario = *ptr;
    if (usuario.ip == "") return 2;

    for (int j; j<capacidad_usuarios; j++) {
        struct Usuarios usuario = lista_usuarios[j];
        int capacidad = usuario.cantidad_archivos;
        for (int i; i<capacidad; i++) {
            if (strcmp(lista_usuarios[j].lista_archivos[i].fileName, remote_FileName) == 0 ) {
            
                int puerto = lista_usuarios[j].puerto;
                struct sockaddr_in server_addr;
                FILE *fp;

                // Nos conectamos al usuario y mandamos la petición
                int sockfd = socket(AF_INET, SOCK_STREAM, 0);
                memset(&server_addr, 0, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(puerto);
                inet_pton(AF_INET, lista_usuarios[j].ip, &server_addr.sin_addr);
                connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
                send(sockfd, remote_FileName, strlen(remote_FileName), 0);

                // Creamos el archivo a recibir
                fp = fopen(local_FileName, "wb");

                // Recibimos los datos y los guardamos
                char buffer[MAX_BUFFER];
                ssize_t bytes_received;
                while ((bytes_received = recv(sockfd, buffer, MAX_BUFFER, 0)) > 0) {
                    fwrite(buffer, 1, bytes_received, fp);
                }

                fclose(fp);
                close(sockfd);
                return 0;
            }
        }
    }
    return 3;
}