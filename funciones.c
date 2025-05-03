int registrar (char *user);
int unregistrar (char *user);
int connect (char *user);
int disconnect (char *user);
int publish (char *fileName, char *description);
int delete (char *fileName);
char * listusers ();
char * listcontent (char *user);
int getfile (char *user, char *remote_FileName, char *local_FileName);


struct Archivos {
    char fileName[256];
    char description[256];
}

struct Usuarios {
    char user[256];
    struct sockaddr_in ipadd;
    int puerto;
    struct Archivos * lista_archivos;
}

struct * Usuarios actualizar_lista_usuarios(struct * Usuarios lista_usuarios, int capacidad, struct Usuarios usuario ) {
    capacidad ++;
    lista_usuarios = (struct Usuarios *)realloc(lista_usuarios, capacidad * sizeof(struct Usuarios));
    lista_usuarios[capacidad] = usuario;
    return lista_usuarios
}

struct * Archivos actualizar_lista_archivos(struct * Archivos lista_archivos, int capacidad, struct Archivos archivo ) {
    capacidad ++;
    lista_archivos = (struct Archivos *)realloc(lista_archivos, capacidad * sizeof(struct Archivos));
    lista_archivos[capacidad] = archivo;
    return lista_archivos
}

int registrar (char * user) {
    
}