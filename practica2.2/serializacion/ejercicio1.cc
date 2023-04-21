#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        _size = 2 * sizeof(int16_t) + MAX_NAME * sizeof(char);

        alloc_data(_size);

        memset(_data, 0, _size);
        
        char* index = _data;

        memcpy(index, name, MAX_NAME * sizeof(char));
        index += MAX_NAME * sizeof(char);

        memcpy(index, &x, sizeof(int16_t));
        index += sizeof(int16_t);

        memcpy(index, &y, sizeof(int16_t));
        index += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        char* index = data;

        memcpy(name, index, MAX_NAME * sizeof(char));
        index += MAX_NAME * sizeof(char);

        memcpy(&x, index, sizeof(int16_t));
        index += sizeof(int16_t);

        memcpy(&y, index, sizeof(int16_t));
        index += sizeof(int16_t);

        return index - data;
    }


public:
    static const size_t MAX_NAME = 20;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("./jugador.bin", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    write(fd, one_w.data(), one_w.size());

    close(fd);

    // 3. Leer el fichero
    fd = open("./jugador.bin", O_RDONLY, 0666);
    char* buffer = (char *) malloc(256);
    read(fd, buffer, 256);

    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);

    // 5. Mostrar el contenido de one_r
    std::cout << "Jugador " << one_r.name << ". x = " << one_r.x << "\ty = " << one_r.y << "\n";

    close(fd);

    return 0;
}

