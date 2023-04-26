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

    std::cout << (index - _data) << " " <<  unsigned(type) << "\n";

    memcpy(index, &type, sizeof(uint8_t));
    index += sizeof(uint8_t);

std::cout << (index - _data) << " " << nick << "\n";

    memcpy(index, nick.c_str(), 8 * sizeof(char));
    index += 8 * sizeof(char);

    std::cout << (index - _data) << " " << message << "\n";

    memcpy(index, message.c_str(), 80 * sizeof(char));
    index += 80 * sizeof(char);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);
    std::cout << "_data: " << _data << "\n";

    //Reconstruir la clase usando el buffer _data
    char* index = _data;

    memcpy(&type, index, sizeof(uint8_t));
    std::cout << (index - _data) << " " << unsigned(type)<< "\n";
    index += sizeof(uint8_t);

    char nick_buf[8];
    memcpy(nick_buf, index, 8 * sizeof(char));
    nick = std::string(nick_buf);
    std::cout << (index - _data) << " " << nick << "\n";
    index += 8 * sizeof(char);

    char message_buf[80];
    memcpy(message_buf, index, 80 * sizeof(char));
    message = std::string(message_buf);
    std::cout << (index - _data) << " " << message << "\n";
    index += 80 * sizeof(char);

    return index - _data;
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

        std::cout << "Lets go\n";
        int success = socket.recv(receivedMessage, sender);

        if (success < 0) continue;

        std::cout << "Recivi un mensaje! " << receivedMessage.nick << ": " << receivedMessage.message << "\n";

        switch(receivedMessage.type){
            case ChatMessage::LOGIN:
                clients.push_back(std::move(std::unique_ptr<Socket>(sender)));
                std::cout << "Login\n";
                break;
            case ChatMessage::LOGOUT:
                for(auto it = clients.begin(); it != clients.end(); it++)
                    if(it->get() == sender){
                        clients.erase(it);
                        break;
                    }
                std::cout << "Logout\n";
                break;
            case ChatMessage::MESSAGE:
                for(auto it = clients.begin(); it != clients.end(); it++){
                    if(it->get() == sender){
                        std::cout << "A mi no\n";
                        continue;
                    }
                    
                    std::cout << "Envio mensaje\n";
                    it->get()->send(receivedMessage, *sender);
                }
                break;
            default:
            std::cout << "xd\n";
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg = "login";

    std::cout << "login\n";
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    std::cout << "Type " << unsigned(em.type) << "\n";

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

