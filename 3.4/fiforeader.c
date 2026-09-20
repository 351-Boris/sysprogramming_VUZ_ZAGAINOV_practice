#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s fifo\n", argv[0]);
        return 1;
    }

    printf("Ожидание открытия FIFO на запись...\n");
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }
    printf("FIFO открыт для чтения\n");

    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0)
    {
        buf[n] = '\0';
        printf("Прочитано: %s\n", buf);
    }
    close(fd);
    return 0;
}