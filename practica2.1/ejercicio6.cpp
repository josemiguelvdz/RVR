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

#define N_THREADS 3
#define BUFFER_SIZE 256

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

class MessageThread
{
public:
    bool thread_end;

    MessageThread(int socket_descriptor) : sd(socket_descriptor), thread_end(false) {}

    ~MessageThread() {
        thread_end = true;
    }

    void thread_work()
    {
        char entryBuffer[BUFFER_SIZE];
        memset(&entryBuffer, '0', BUFFER_SIZE);

        while (!thread_end)
        {
            struct sockaddr client_addr;
            socklen_t client_len = sizeof(struct sockaddr);

            memset(&entryBuffer, '0', BUFFER_SIZE);

            ssize_t bytes = recvfrom(sd, entryBuffer, (BUFFER_SIZE - 1) * sizeof(char), 0, &client_addr, &client_len);

            if (bytes == 1)
                continue;
            else if (bytes == -1) {
                std::cerr << "Error en recv.\n";
                thread_end = true;
                continue;
            }

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

            char exitBuffer[BUFFER_SIZE];
            memset(&exitBuffer, '0', BUFFER_SIZE);

            if (entryBuffer[0] == 't')
                get_time(exitBuffer);
                
            else if (entryBuffer[0] == 'd')
                get_date(exitBuffer);

            else if (entryBuffer[0] == 'q'){
                thread_end = true;
                continue;
            }
                
            sendto(sd, exitBuffer, strlen(exitBuffer), 0, &client_addr, client_len);

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
    struct addrinfo hints, *result, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
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

    std::vector<std::thread> threads;
    std::vector<MessageThread*> msgThreads;

    std::cout << "Creando los hilos...\n";
    for (int i = 0; i < N_THREADS; i++)
    {
        std::shared_ptr<MessageThread> threadsManager(new MessageThread(sd));
        threads.push_back(std::thread(&MessageThread::thread_work, threadsManager));
        msgThreads.push_back(threadsManager.get());
        std::cout << "Hilo creado | Thread_id: " << threads[i].get_id() << "\n";
    }

    std::string user_input = "";
    while (strcmp(user_input.c_str(), "q")) {
        std::cin >> user_input;
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        std::thread::id id = threads[i].get_id();
        msgThreads[i]->thread_end = true;
        threads[i].join();
        std::cout << "Hilo cerrado | Thread_id: " << id << "\n";
    }

    freeaddrinfo(result);

    return 0;
}