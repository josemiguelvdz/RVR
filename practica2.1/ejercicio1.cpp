#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

using namespace std;

int main(int argc, char** argv){
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << "<domainName>\n";
        return -1;
    }

    char* domainName = argv[1];

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;

    int status = getaddrinfo(domainName, NULL, &hints, &result);

    if (status != 0){
        cerr << "Error en getaddrinfo: " << gai_strerror(status) << "\n";
        return -1;
    }

    for(struct addrinfo* i = result; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST];

        getnameinfo (i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, NULL,
        0, NI_NUMERICHOST);

        cout << host << "\t" << i->ai_family << "\t" << i->ai_socktype << "\n";
    }

    freeaddrinfo(result);

    return 0;
}
