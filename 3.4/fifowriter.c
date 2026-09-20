#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s fifo\n", argv[0]);
        return 1;
    }

    // Создаем FIFO с проверкой ошибок
    if (mkfifo(argv[1], 0666) < 0)
    {
        // Если ошибка не "файл уже существует", то это критическая ошибка
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return 1;
        }
        // Если файл уже существует, просто продолжаем работу
        printf("FIFO '%s' уже существует, используем его\n", argv[1]);
    }
    else
    {
        printf("FIFO '%s' успешно создан\n", argv[1]);
    }

    int fd = open(argv[1], O_WRONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    char msg[] = "Hello via FIFO!";
    write(fd, msg, sizeof(msg));
    printf("Записано в FIFO: %s\n", msg);
    close(fd);
    return 0;
}