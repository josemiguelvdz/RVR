#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <string>
#include <thread>
#include <vector>
#include <memory>

#define N_THREADS 5

class MessageThread
{
public:
    MessageThread(int socket_descriptor) : sd(socket_descriptor), thread_end(false) {}

    ~MessageThread() {
        thread_end = true;
    }

    void thread_work()
    {
        int size = 2;
        char entryBuffer[size];
        memset(&entryBuffer, '0', sizeof(char) * size);

        while (!thread_end)
        {
            struct sockaddr client_addr;
            socklen_t client_len = sizeof(struct sockaddr);

            memset(&entryBuffer, '0', sizeof(char) * size);

            ssize_t bytes = recvfrom(sd, entryBuffer, (size - 1) * sizeof(char), MSG_DONTWAIT, &client_addr, &client_len);

            if (bytes == -1) 
                continue;

            entryBuffer[bytes] = '\0';

            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            memset(&host, '0', sizeof(char) * NI_MAXHOST);
            memset(&service, '0', sizeof(char) * NI_MAXSERV);

            std::thread::id id = std::this_thread::get_id();

            getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            printf("%d bytes from %s:%s | THREAD_ID: ", (int)bytes, host, service);
            std::cout << id << "\n";

            time_t t = time(NULL);
            tm* date_time = localtime(&t);

            int send_size = 20;
            char exitBuffer[send_size];
            memset(&exitBuffer, '0', sizeof(char) * send_size);

            if (recv_buffer[0] == 't')
            {
                ssize_t send_bytes = strftime(exitBuffer, 20, "%r", date_time);
                exitBuffer[send_size] = '\0';
                sendto(sd, exitBuffer, send_bytes, 0, &client_addr, client_len);
            }
            else if (recv_buffer[0] == 'd')
            {
                ssize_t send_bytes = strftime(exitBuffer, 20, "%F", date_time);
                exitBuffer[send_size] = '\0';
                sendto(sd, exitBuffer, send_bytes, 0, &client_addr, client_len);
            }
            else {
                sendto(sd, nullptr, 0, 0, &client_addr, client_len);
                std::cerr << "Comando no soportado: " << recv_buffer << "\n";
            }
            sleep(3);
        }
    }

private:
    int sd;

public:
    bool thread_end;
};

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Invalid number of parameters. Usage: ejercicio4 <IPAddress> <port>\n";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    //Comprobacion de errores en la llamada a getaddrinfo
    if (rc != 0)
    {
        //Mostramos el error por la salida estandar de errores
        std::cerr << "Error en getaddrinfo(): " << gai_strerror(rc) << std::endl;
        return -1;
    }

    //Creacion del descriptor y bind del socket
    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    //Comprobacion de errores en la llamada a bind()
    if (bind(sd, result->ai_addr, result->ai_addrlen) != 0)
    {
        std::cerr << "Error en la llamada a la funcion bind." << std::endl;
        return -1;
    }

    std::vector<std::thread> threads;
    std::vector<MessageThread*> message_threads;

    std::cout << "Creando los hilos...\n";
    for (int i = 0; i < N_THREADS; i++)
    {
        std::shared_ptr<MessageThread> threadsManager(new MessageThread(sd));
        threads.push_back(std::thread(&MessageThread::thread_work, threadsManager));
        message_threads.push_back(threadsManager.get());
        std::cout << "Hilo creado! | Thread_id: " << threads[i].get_id() << "\n";
    }

    //Leer la entrada del usuario hasta que cierre el servidor
    std::string user_input = "";
    while (strcmp(user_input.c_str(), "q")) {
        std::cin >> user_input;
    }

    //Desmontamos los hilos uy los cerramos
    for (int i = 0; i < N_THREADS; i++)
    {
        std::thread::id id = threads[i].get_id();
        message_threads[i]->thread_end = true;
        threads[i].join();
        std::cout << "Hilo cerrado: | Thread_id: " << id << "\n";
    }
    std::cout << "Hilos cerrados.\n"

    freeaddrinfo(result);

    return 0;
}