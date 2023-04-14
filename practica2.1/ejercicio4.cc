#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio4 <IPAddress> <port>\n";
        return -1;
    }

    char* node = argv[1];
    char* service = argv[2];

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int nc = getaddrinfo(node, service, &hints, &result);

    if (nc != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (bind(sd, result->ai_addr, result->ai_addrlen) != 0)
    {
        std::cerr << "bind error\n";
        return -1;
    }

    if (listen(sd, 16) == -1)
    {
        std::cerr << "listen error\n";
        return -1;
    }

    struct sockaddr clientAddr;
    socklen_t clientLen = sizeof(struct sockaddr);
    int sd = accept(sd, &clientAddr, &clientLen);

    if (sd == -1)
    {
        std::cerr << "accept error\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    getnameinfo(&clientAddr, clientLen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout "Conexion desde " << host << " " << service << "\n");

    const int size = 256;
    char entryBuffer[size];

    ssize_t bytes = -1;
    while (bytes != 0)
    {
        memset(&entryBuffer, '0', sizeof(char) * size);

        bytes = recv(sd, entryBuffer, size, 0);

        if (bytes == 0)
        {
            std::cout << "Conexion terminada\n";
            continue;
        }
        else if (bytes == -1)
        {
            std::cerr << "recv error\n";
            return -1;
        }

        entryBuffer[bytes] = '\0';

        bytes = send(sd, entryBuffer, bytes, 0);

        if (bytes == -1)
        {
            std::cerr << "send error\n";
            return -1;
        }
    }

    freeaddrinfo(result);

    return 0;
}