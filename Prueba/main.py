import socket

def start_server():
    host = '0.0.0.0'
    port = 0

    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.bind((host, port))
    print(server_sock.getsockname()[1])
    server_sock.listen(1)
    print(f"Servidor escuchando en {host}:{port}")

    while True:
        client_sock, addr = server_sock.accept()
        print(f"Conexión desde {addr}")

        request = client_sock.recv(256)
        if request:
                    with open(request, "rb") as f:
                        data = f.read()
                    client_sock.sendall(data)
        client_sock.close()

start_server()
