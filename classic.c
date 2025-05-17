#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int j = 0;
    char buffer[4];
    int fd = open(argv[1], O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    read(fd, buffer, 4);

    if (buffer[0] == 'b') j++;
    if (buffer[1] == 'a') j++;
    if (buffer[2] == 'd') j++;
    if (buffer[3] == '!') j++;

    if (j == 4) {
        abort();
    }
    return 0;
}

