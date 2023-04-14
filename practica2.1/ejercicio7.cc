#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <memory>

class ConnectionThread
{
public:
    ConnectionThread(int socket_descriptor) : socket_descriptor(socket_descriptor), thread_end(false) {}

    ~ConnectionThread() {
        thread_end = true;
    }

    void thread_work()
    {
        int size = 120;
        char buffer[size];
        memset(&buffer, '\0', sizeof(char) * size);

        ssize_t bytes = -1;
        while (!thread_end)
        {
            memset(&buffer, '0', sizeof(char) * size);

            // -- Apartado de recepcion del mensaje --

            bytes = recv(socket_descriptor, buffer, (size - 1) * sizeof(char), 0);

            if (bytes == -1) return; //Si se produce un error, simplemente se  ignora

            if (bytes == 0) //Se comprueba si el numero de bytes es 0 para determinar si el cliente ha cerrado la conexion
            {
                printf("Conexion terminada.\n");
                return;
            }

            buffer[bytes] = '\0';

            // --Apartado de respuesta del servidor--

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
    if (argc != 3)
    {
        printf("Numero de argumentos no valido\n");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0)
    {
        std::cerr << "getaddinfo error: " << gai_strerror(rc) << "\n";
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

        // -- Apartado de creacion del hilo --

        std::shared_ptr<ConnectionThread> connection(new ConnectionThread(sd));

        std::thread thread(&ConnectionThread::thread_work, connection);
        thread.detach();
    }

    freeaddrinfo(result);

    return 0;
}