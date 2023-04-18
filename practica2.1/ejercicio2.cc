#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

using namespace std;

void get_time(char * buffer) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, BUFFER_SIZE, "%r", timeinfo);
}

void get_date(char * buffer) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, BUFFER_SIZE, "%F", timeinfo);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <IPAddress> <port>" << endl;
        exit(EXIT_FAILURE);
    }

    char *ip_address = argv[1];
    char *port = argv[2];

    int status;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(ip_address, port, &hints, &res)) != 0) {
        cerr << "Error en getaddrinfo: " << gai_strerror(status) << endl;
        exit(EXIT_FAILURE);
    }

    int sockfd;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }
    if (p == NULL) {
        cerr << "Error en bind" << endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);

    cout << "Servidor escuchando en " << ip_address << ":" << port << endl;

    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof(their_addr);
    char buffer[BUFFER_SIZE];
    int numbytes;

    while (true) {
        numbytes = recvfrom(sockfd, buffer, BUFFER_SIZE-1, 0, (struct sockaddr *)&their_addr, &addr_len);
        buffer[numbytes] = '\0';

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        int status = getnameinfo((struct sockaddr *)&their_addr, addr_len, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (status != 0) {
            cerr << "Error en getnameinfo: " << gai_strerror(status) << endl;
            continue;
        }
        cout << numbytes << " bytes de " << host << ":" << service << endl;
if (buffer[0] == 't') {
            get_time(buffer);
        }
        else if (buffer[0] == 'd') {
            get_date(buffer);
        }
        else if
        (buffer[0] == 'q') {
        break;
    }
    else {
        cout << "Comando no soportado " << buffer[0] << endl;
        continue;
    }
    if ((numbytes = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&their_addr, addr_len)) == -1) {
        cerr << "Error en sendto" << endl;
        exit(EXIT_FAILURE);
    }
}

cout << "Saliendo..." << endl;
close(sockfd);
return 0;
}