#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char** argv){
    if (argc < 2)
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio1 <domainName>\n";
        return -1;
    }

    char* node = argv[1];

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;

    int nc = getaddrinfo(node, NULL, &hints, &result);

    if (nc != 0){
        std::cerr << "getaddrinfo error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    for(struct addrinfo* i = result; i != nullptr; i = i->ai_next){
        char host [NI_MAXHOST];

        getnameinfo (i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, NULL,
        0, NI_NUMERICHOST);

        std::cout << host << "\t" << i->ai_family << "\t" << i->ai_socktype << "\n";
    }

    freeaddrinfo(result);

    return 0;
}