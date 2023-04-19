#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <memory>

#define BUFFER_SIZE 256

class ConnectionThread
{
public:
    ConnectionThread(int socket_descriptor) : socket_descriptor(socket_descriptor), thread_end(false) {}

    ~ConnectionThread() {
        thread_end = true;
    }

    void thread_work()
    {
        char buffer[BUFFER_SIZE];
        memset(&buffer, '\0', BUFFER_SIZE);

        ssize_t bytes = -1;
        while (!thread_end)
        {
            memset(&buffer, '0', BUFFER_SIZE);
            bytes = recv(socket_descriptor, buffer, BUFFER_SIZE, 0);

            if (bytes == -1) return;

            if (bytes == 0)
            {
                printf("Conexion terminada.\n");
                return;
            }

            buffer[bytes] = '\0';

            bytes = send(socket_descriptor, buffer, bytes, 0);

            if (bytes == -1)
            {
                std::cerr << "Error en la llamada a la funcion send." << std::endl;
                return;
            }
            sleep(3);
        }
    }

private:
    int socket_descriptor;
    bool thread_end;
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
    struct addrinfo hints, *result, *p;

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

    if (listen(sd, 16) == -1)
    {
        std::cerr << "listen error\n";
        return -1;
    }

    while (true)
    {
        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        int sd = accept(sd, &client_addr, &client_len);

        if (sd == -1)
        {
            std::cerr << "accept error" << std::endl;
            return -1;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        printf("Conexion desde %s %s\n", host, service);

        std::shared_ptr<ConnectionThread> connection(new ConnectionThread(sd));

        std::thread thread(&ConnectionThread::thread_work, connection);
        thread.detach();
    }

    freeaddrinfo(result);

    return 0;
}