#include "Chat.h"
#include <algorithm>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char* index = _data;

    memcpy(index, &type, sizeof(char));
    index += sizeof(char);

    size_t clamped_size = nick.length() * sizeof(char);
    if(MESSAGE_SIZE - 1  < index - _data)
        clamped_size = MESSAGE_SIZE + index - _data;

    memcpy(index, &nick, clamped_size);
    index += clamped_size;
    index[0] = '\0';
    index++;

    clamped_size = message.length() * sizeof(char);
    if(MESSAGE_SIZE - 1  < index - _data)
        clamped_size = MESSAGE_SIZE + index - _data;

    memcpy(index, &message, sizeof(int16_t));
    index += sizeof(int16_t);
    index[0] = '\0';
    index++;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* index = bobj;

    memcpy(&type, index, sizeof(char));
    index += sizeof(char);

    std::string delimiter = "\0";
    std::string bobjString(bobj);
    std::string token = bobjString.substr(0, bobjString.find(delimiter));

    size_t clamped_size = token.length() * sizeof(char);
    if(MESSAGE_SIZE < index - _data)
        clamped_size = MESSAGE_SIZE + index - _data;

    memcpy(&nick, &token, clamped_size);
    index += clamped_size;

    clamped_size = bobjString.length() * sizeof(char);
    if(MESSAGE_SIZE < index - _data)
        clamped_size = MESSAGE_SIZE + index - _data;

    memcpy(&message, &bobjString, clamped_size);
    index += clamped_size;
    return std::min((int) (index - bobj), (int) MESSAGE_SIZE);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
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
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

