#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#define BUFFER_SIZE 256

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio4 <IPAddress> <port>\n";
        return -1;
    }

    char *ip_address = argv[1];
    char *port = argv[2];

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int nc = getaddrinfo(ip_address, port, &hints, &result);

    if (nc != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (connect(sd, result->ai_addr, result->ai_addrlen) == -1) {

        std::cerr << "connect error\n";
        return -1;
    }


    char entryBuffer[BUFFER_SIZE];
    char exitBuffer[BUFFER_SIZE];

    ssize_t bytes = -1;
    while (1)
    {
        memset(&exitBuffer, '0', sizeof(char) * BUFFER_SIZE);

        std::cin >> exitBuffer;
        bytes = send(sd, exitBuffer, strlen(exitBuffer), 0);

        if(strcmp(exitBuffer, "Q") == 0)
            break;

        if (bytes == -1)
        {
            std::cerr << "send error\n";
            return -1;
        }

        char entryBuffer[BUFFER_SIZE];
        memset(&entryBuffer, '0', BUFFER_SIZE * sizeof(char));
        bytes = recv(sd, entryBuffer, (BUFFER_SIZE - 1) * sizeof(char), 0);

        if (bytes == -1)
        {
            std::cerr << "recv error\n";
            return -1;
        }
        entryBuffer[bytes] = '\0';

        std::cout << entryBuffer << "\n";
    }

    freeaddrinfo(result);
    return 0;
}