#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <time.h>

int main(int argc, char** argv){
    if (argc < 3) 
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio2 <IPAddress> <port>\n";
        return -1;
    }

    char* node = argv[1];
    char* service = argv[2];

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int nc = getaddrinfo(node, service, &hints, &result);
    
    if (nc != 0){
        std::cerr << "getaddrinfo error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    if (bind(sd, (struct sockaddr*)result->ai_addr, result->ai_addrlen) != 0) {
        std::cerr << "bind error\n";
        return -1;
    }

    char c = ' ';

    const int size = 256;
    char entryBuffer[size];
    char exitBuffer[size];

    ssize_t nBytes;

    while(c != 'q') {
        nBytes = recvfrom(sd, &entryBuffer, 80, 0, (struct sockaddr *) &cliente, &cliente_len);

        entryBuffer[nBytes] = '\0';

        c = entryBuffer[0];

        switch (c)
        {
        case 't':
        {
            struct tm* timeInfo;
            time_t timer = mktime(timeInfo);
            localtime(&timer);
            
            strftime(&exitBuffer[0], size, "%T", timeInfo);
        }
            break;
        case 'd':
            {
            struct tm* timeInfo;
            time_t timer = mktime(timeInfo);
            localtime(&timer);
            
            strftime(&exitBuffer[0], size, "%D", timeInfo);
        }
            break;
        default:
            break;
        }

        getnameinfo((struct sockaddr* ) &cliente, cliente_len, host, NI_MAXHOST, server, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexion desde Host: " << host << "\t" << server << "\n";
        std::cout << "\tMensaje" << nBytes << "\t" << exitBuffer << "\n";

        exitBuffer[size] = '\0';
        sendto(sd, &exitBuffer[0], nBytes, 0, (struct sockaddr *) &cliente, cliente_len);
    }

    freeaddrinfo(result);

    return 0;
}