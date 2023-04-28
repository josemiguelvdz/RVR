#include "Chat.h"
#include <algorithm>
#include <memory>

#define BUFFER_SIZE 256

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char* index = _data;

    memcpy(index, &type, sizeof(uint8_t));
    index += sizeof(uint8_t);

    memcpy(index, nick.c_str(), 8 * sizeof(char));
    index += 8 * sizeof(char);

    memcpy(index, message.c_str(), 80 * sizeof(char));
    index += 80 * sizeof(char);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* index = _data;

    memcpy(&type, index, sizeof(uint8_t));
    index += sizeof(uint8_t);

    nick = index;
    index += 8 * sizeof(char);

    message = index;
    index += 80 * sizeof(char);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    char entryBuffer[BUFFER_SIZE];

    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        memset(&entryBuffer, '0', BUFFER_SIZE * sizeof(char));

        ChatMessage receivedMessage;
        Socket* sender;

        int success = socket.recv(receivedMessage, sender);
        if (success < 0) continue;

        switch(receivedMessage.type){
            case ChatMessage::LOGIN:
                clients.push_back(std::move(std::unique_ptr<Socket>(sender)));
                std::cout << "LOGIN: " << receivedMessage.nick << "\n";
                break;
            case ChatMessage::LOGOUT:
                for(auto it = clients.begin(); it != clients.end(); it++)
                    if(**it == *sender){
                        clients.erase(it);
                        break;
                    }
                std::cout << "LOGOUT: " << receivedMessage.nick << "\n";
                break;
            case ChatMessage::MESSAGE:
                std::cout << "MESSAGE from " << receivedMessage.nick << ": " << receivedMessage.message << "\n";
                for(auto it = clients.begin(); it != clients.end(); it++)
                    if(!(**it == *sender)){
                        socket.send(receivedMessage,**it);
                    }
                break;
            default:
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg = "";

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string inputText;
        std::getline(std::cin, inputText);

        ChatMessage message(nick, inputText);

        if(inputText == "logout"){
            message.type = ChatMessage::LOGOUT;
            socket.send(message, socket);
            break;
        }
        else
            message.type = ChatMessage::MESSAGE;

        socket.send(message, socket);
    }
}

void ChatClient::net_thread()
{
    Socket* s = new Socket(socket);
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage message;

        //Socket* sender;
        socket.recv(message, s);

        std::cout << message.nick << ": " << message.message << "\n";
    }
}

