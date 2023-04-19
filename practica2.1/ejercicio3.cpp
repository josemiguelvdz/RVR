#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Uso: " << argv[0] << " <IPAddress> <port> <(t|d|q)>\n";
        exit(EXIT_FAILURE);
    }

    char* ip_address = argv[1];
    char* port = argv[2];
    char* c = argv[3];

    struct addrinfo hints, *result, *p;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = getaddrinfo(ip_address, port, &hints, &result);

    if (status != 0) {
        cerr << "Error en getaddrinfo: " << gai_strerror(status) << "\n";
        exit(EXIT_FAILURE);
    }
    
    int socket_fd;
    for (p = result; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        break;
    }
    if (p == NULL) {
        cerr << "Error: no se pudo conectar.\n";
        exit(EXIT_FAILURE);
    }

    ssize_t bytes = sendto(socket_fd, c, strlen(c) + 1, 0, result->ai_addr, result->ai_addrlen);
    if(bytes == -1){
        cerr << "Error en sendto.\n";
        exit(EXIT_FAILURE);
    }

    int size = 256;
    char buffer[size];
    memset(&buffer, '0', size);
    
    bytes = recvfrom(socket_fd, buffer, size, 0, result->ai_addr, &result->ai_addrlen);

    if (bytes == -1) {
        cerr << "Error en recvfrom error\n";
        exit(EXIT_FAILURE);
    }

    buffer[bytes] = '\0';

    cout << buffer << "\n";
    freeaddrinfo(result);
    close(socket_fd);

    return 0;
}