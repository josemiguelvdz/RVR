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

    if (connect(sd, result->ai_addr, result->ai_addrlen) == -1) {

        std::cerr << "connect error\n";
        return -1;
    }

    int size = 256;
    char entryBuffer[size];
    char exitBuffer[size];

    ssize_t bytes = -1;
    while (1)
    {
        memset(&exitBuffer, '0', sizeof(char) * size);

        std::cin >> exitBuffer;
        bytes = send(sd, exitBuffer, strlen(exitBuffer), 0);

        if(strcmp(exitBuffer, "Q") == 0)
            break;

        if (bytes == -1)
        {
            std::cerr << "send error\n";
            return -1;
        }

        char entryBuffer[size];
        memset(&entryBuffer, '0', size);
        bytes = recv(sd, entryBuffer, (size - 1) * sizeof(char), 0);

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