//Apuntes clase 13/4
//Estan apuntados al vuelo asi que puede que algo este mal

//UDP concurrente
#include <thread>
#include <vector>

void haz_mensaje(int sd) {
    nBytes = recvfrom(sd, &entryBuffer, 80, 0, (struct sockaddr*)&cliente, &cliente_len);

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

    getnameinfo((struct sockaddr*)&cliente, cliente_len, host, NI_MAXHOST, server, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "Conexion desde Host: " << host << "\t" << server << "\n";
    std::cout << "\tMensaje" << nBytes << "\t" << exitBuffer << "\n";

    sendto(sd, &exitBuffer[0], nBytes, 0, (struct sockaddr*)&cliente, cliente_len);
}

int main(int argc, char** argv) {
    // Leer argumentos
    char* node = argv[1];
    char* service = argv[2];

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; // Devolver 0.0.0.0
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int nc = getaddrinfo(node, service, &hints, &result);

    if (nc != 0) {
        std::cerr << "Error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    bind(sd, (struct sockaddr*)result->ai_addr, result->ai_addrlen);

    char c = ' ';

    const int size = 256;
    char entryBuffer[size];
    char exitBuffer[size];

    ssize_t nBytes;

    std::vector<std::thread> threads;

    for (int i = 0 : i < 5; i++)
        threads.push_back(std::thread([sd]() {
        while (true)
        haz_mensaje(sd);
            }));

    for (auto& thr : threads)
        thr.join();

    //COMPILAR:
    //g++ - o udp_1 udp.cc

    freeaddrinfo(result);

    return 0;
}

//TCP concurrente 
void haz_conexion(int sd) {

    while (true) { // para todos los mensajes del cliente
        char buffer[1500];

        ssize_t bytes = recv(client_sd, buffer, 1499, 0);

        if (bytes == 0) {
            std::cout << "FIN CONEXION\n";
            break;
        }

        buffer[bytes] = '\0';

        cout << "\tMSG: " << buffer;

        int num = atoi(buffer) + 1;

        auto s = std::to_string(num);

        send(client_sd, s.c_str(), s.length(), 0);
    }
}

int main(int argc, char** argv) {
    // Leer argumentos
    char* node = argv[1];
    char* service = argv[2];

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; // Devolver 0.0.0.0
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP

    int nc = getaddrinfo(node, service, &hints, &result);

    if (nc != 0) {
        std::cerr << "Error: " << gai_strerror(nc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    int rc = bind(sd, /*(struct sockaddr * )*/ result->ai_addr, result->ai_addrlen);

    if (rc == -1) {
        std::cerr << "[bind:]" << std::errno << "\n";
        return -1;
    }

    freeaddrinfo(result);

    while (true) {
        char buffer 1500;
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        sturct sockaddr_storage client;
        socklen_t clientlen = sizeof(stuct sockaddr_storage);

        int client_sd = accept(sd, (struct sockaddr*)&client, &clientlen);
        getnameinfo((struct sockaddr*)&client, clientlen, host, NI_MAXHOST, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "NUEVA CONEXION IP: " << host << "[: " << serv << "]\n";

        std::thread client_thread(haz_conexion, client_sd);
    }


    client_thread.detach();

    //COMPILAR:
    //g++ - o tcp_1 tcp1.cc

    return 0;
}