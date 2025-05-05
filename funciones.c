int registrar (char *user);
int unregistrar (char *user);
int connect (char *user);
int disconnect (char *user);
int publish (char *fileName, char *description);
int delete (char *fileName);
char * listusers (char *user);
char * listcontent (char *user);
int getfile (char *user, char *remote_FileName, char *local_FileName);


struct Archivos {
    char fileName[256];
    char description[256];
}

struct Usuarios {
    char user[256];
    char ip[16];
    int puerto;
    struct Archivos * lista_archivos;
    int cantidad_archivos;
}


// Las funciones y declaraciones asociadas a la lista dinámica de usuarios

struct Usuarios * lista_usuarios = NULL; 
int capacidad_usuarios;

void actualizar_lista_usuarios(struct Usuarios usuario ) {
    capacidad_usuarios ++;
    lista_usuarios = (struct Usuarios *)realloc(lista_usuarios, capacidad_usuarios * sizeof(struct Usuarios));
    lista_usuarios[capacidad_usuarios] = usuario;
}

struct Usuarios buscar_usuario(char * user) {
    for (int i; i<capacidad_usuarios; i++) {
        if (strcmp(lista_usuarios[i].user, user) == 0 )
        return &lista_usuarios[i]
    }
    return NULL
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
    lista_usuarios = realloc(lista_usuarios, capacidad_usuarios * sizeof(Usuarios)); // Parecido a un free pero con más pasos
    return 0;
}

// Las funciones y declaraciones asociadas a las listas dinámicas de los libros de los usuarios

void actualizar_lista_archivos(struct Archivos * lista, int capacidad, struct Archivos archivo ) {
    capacidad ++;
    listas = (struct Archivos *)realloc(lista, capacidad * sizeof(struct Archivos));
    lista_archivos[capacidad] = archivo;
}

struct Archivos buscar_archivo(struct Archivos * lista, int capacidad, char * fileName) {
    for (int i; i<capacidad; i++) {
        if (strcmp(lista[i].fileName, fileName) == 0 )
        return &lista_usuarios[i]
    }
    return NULL
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
    lista = realloc(lista, capacidad * sizeof(Usuarios)); // Parecido a un free pero con más pasos
    return 0;
}

// Definición de funciones del servidor

int registrar (char * user) {
    struct Usuarios usuario;
    strncpy(usuario.user, user);
    struct Usuarios prueba = buscar_usuario(user);
    if (prueba == NULL) {
        actualizar_lista_usuarios(usuario);
        return 0
    }
    return 1
}

int unregistrar (char * user) {
    return borrar_usuario(user);
}

int connect (char * user,  char * ip, int puerto) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return 1;
    if (usuario.ip != NULL) return 2;
    strncpy(usuario.ip, ip);
    usuario.puerto = puerto;
    return 0
}

int disconnect (char * user,  char * ip, int puerto) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return 1;
    if (usuario.ip == NULL) return 2;
    usuario.ip = NULL;
    usuario.puerto = NULL;
    return 0
}

int publish (char * user, char * fileName, char * description) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return 1;
    if (usuario.ip == NULL) return 2;
    if (buscar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName) != NULL) return 3;
    struct Archivos archivo;
    strncpy(archivo.fileName, fileName);
    strncpy(archivo.description, description);
    actualizar_lista_archivos(usuario.lista_archivos, usuario.cantidad_archivos, archivo);
    return 0
}

int delete (char * user, char * fileName, char * description) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return 1;
    if (usuario.ip == NULL) return 2;
    if (buscar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName) == NULL) return 3;
    borrar_archivo(usuario.lista_archivos, usuario.cantidad_archivos, fileName);
    return 0
}

char * listusers(char * user) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return '1';
    if (usuario.ip == NULL) return '2';

    int string_size = capacidad_usuarios * 280; // Son 256 del usuario y algo más para los separadores
    int string_size ++;
    char * string = malloc(string_size);
    strcat(string, "\t");
    for (int i = 0; i < capacidad_usuarios; i++) {
        strcat(string, lista_usuarios[i].user);
        if (i < capacidad_usuarios - 1) {
            strcat(string, "\n\t"); 
        }
    }
    return string
}

char * listcontent(char * user) {
    struct Usuarios usuario = buscar_usuario(user);
    if (usuario == NULL) return '1';
    if (usuario.ip == NULL) return '2';

    int capacidad = usuario.cantidad_archivos;
    int string_size = capacidad * 280; // Son 256 del usuario y algo más para los separadores
    int string_size ++;
    char * string = malloc(string_size);
    strcat(string, "\t");
    for (int i = 0; i < capacidad; i++) {
        strcat(string, usuario.lista_archivos[i].user);
        if (i < capacidad - 1) {
            strcat(string, "\n\t"); 
        }
    }
    return string
}

int getfile(char *user, char *remote_FileName, char *local_FileName) {
    
    for (int j; j<capacidad_usuarios; j++) {
        struct Usuarios usuario = lista_usuarios[j];
        int capacidad = usuario.cantidad_archivos;
        for (int i; i<capacidad; i++) {
            if (strcmp(lista_usuarios[j].lista_archivos[i].fileName, remote_FileName) == 0 )
                
        }
    }
}