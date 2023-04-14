#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 4) 
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio3 <IPAddress> <port> <(t|d|q)>\n";
        return -1;
    }

    char* node = argv[1];
    char* service = argv[2];
    char* c = argv[3];

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int nc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (nc != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    sendto(sd, c, strlen(c) + 1, 0, result->ai_addr, result->ai_addrlen);

    int size = 256;
    char entryBuffer[size];
    memset(&entryBuffer, '0', size);

    ssize_t bytes = recvfrom(sd, entryBuffer, size, 0, result->ai_addr, &result->ai_addrlen);

    if (bytes == -1) {
        std::cerr << "recvfrom error\n";
        return -1;
    }

    entryBuffer[bytes] = '\0';

    std::cout << entryBuffer;
    freeaddrinfo(result);

    return 0;
}