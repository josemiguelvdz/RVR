#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <memory>

#define N_THREADS 3
#define BUFFER_SIZE 256

class ThreadListener
{
public:
    ThreadListener(int sd_) : sd(sd_) {}

    ~ThreadListener() {
    }

    void thread_main()
    {
        char entryBuffer[BUFFER_SIZE];
        memset(&entryBuffer, '\0', BUFFER_SIZE);

        while (true)
        {
            memset(&entryBuffer, '0', BUFFER_SIZE);
            ssize_t entryBytes = recv(sd, entryBuffer, BUFFER_SIZE, 0);

            if (entryBytes == 0) {
                std::cout << "Conexion terminada\n";
                break;
            }
            else if (entryBytes < 0) {
                continue;
            }

            entryBuffer[entryBytes] = '\0';

            ssize_t sendBytes = send(sd, entryBuffer, entryBytes, 0);

            if (sendBytes == -1)
            {
                std::cerr << "Error en la llamada a la funcion send." << std::endl;
                continue;
            }

            sleep(3);
        }
    }
private:
    int sd;
};

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio6 <IPAddress> <port>\n";
        return -1;
    }
    
    char *ip_address = argv[1];
    char *port = argv[2];

    int status;
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(ip_address, port, &hints, &result)) != 0) {
        std::cerr << "Error en getaddrinfo: " << gai_strerror(status) << "\n";
        exit(EXIT_FAILURE);
    }

    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (bind(sd, result->ai_addr, result->ai_addrlen) != 0)
    {
        std::cerr << "Error en bind\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on " << ip_address << ":" << port << std::endl;

    if (listen(sd, N_THREADS) == -1)
    {
        std::cerr << "listen error\n";
        return -1;
    }

    while (true)
    {
        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        int threadSocket = accept(sd, &client_addr, &client_len);

        if (threadSocket == -1)
            continue;

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "Conexion con " << host << " " << service << "\n";
        std::shared_ptr<ThreadListener> connection(new ThreadListener(threadSocket));
        std::thread thread(&ThreadListener::thread_main, connection);
        thread.detach();
    }

    freeaddrinfo(result);
    return 0;
}