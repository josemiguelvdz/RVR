#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <IPAddress> <port>\n";
        exit(EXIT_FAILURE);
    }

    char* ip_address = argv[1];
    char* port = argv[2];

    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(ip_address, port, &hints, &result);

    if (status != 0) {
        cerr << "Error en getaddrinfo: " << gai_strerror(status) << "\n";
        exit(EXIT_FAILURE);
    }

    int server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (bind(server_socket, result->ai_addr, result->ai_addrlen) != 0) {
        cerr << "Error en bind.\n";
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    if (listen(server_socket, 16) == -1) {
        cerr << "Error en listen.\n";
        exit(EXIT_FAILURE);
    }

    bool quit = false;
    while (!quit){
        struct sockaddr client_address;
        socklen_t client_len = sizeof(struct sockaddr);
        int client_socket = accept(server_socket, &client_address, &client_len);

        if (client_socket == -1) {
            std::cerr << "Error en accept.\n";
            exit(EXIT_FAILURE);
        }

        char client_host[NI_MAXHOST];
        char client_port[NI_MAXSERV];

        getnameinfo(&client_address, client_len, client_host, NI_MAXHOST, client_port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "Conexion desde " << client_host << " " << client_port << "\n";

        const int size = 256;
        char buffer[size];

        ssize_t bytes = -1;
        while (bytes != 0)
        {
            memset(&buffer, '0', sizeof(buffer));

            bytes = recv(client_socket, buffer, size, 0);

            if (bytes == 0 || (bytes == 1 && buffer[0] == 'Q')) {
                std::cout << "Conexion terminada\n";
                bytes = 0;
                continue;
            }
            else if (bytes == -1) {
                std::cerr << "Error en recv.\n";
                return -1;
            }

            buffer[bytes] = '\0';

            bytes = send(client_socket, buffer, bytes, 0);

            if (bytes == -1) {
                std::cerr << "send error\n";
                return -1;
            }
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
}