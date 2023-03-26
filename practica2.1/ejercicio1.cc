#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char** argv){
    // Leer argumentos
    char* node = argv[1];
    char* service = argv[2];

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;

    int nc = getaddrinfo(node, service, &hints, &result);

    if (nc != 0){
        std::cerr << "Error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    for(struct addrinfo* i = result; i != nullptr; i = i->ai_next){
        char host [NI_MAXHOST];
        char server [NI_MAXSERV];

        getnameinfo (i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, server,
        NI_MAXSERV, NI_NUMERICHOST);

        std::cout << host << "\t" << i->ai_family << "\t" << i->ai_socktype << "\n";
    }

    freeaddrinfo(result);

    return 0;
}