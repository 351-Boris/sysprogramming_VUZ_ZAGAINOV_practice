#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h> // Добавлено для strlen

int main(int argc, char **argv)
{
    // 1. Проверка количества аргументов
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <filename> <permissions>\n", argv[0]);
        fprintf(stderr, "Example: %s /home/student/newfile rwxr--r--\n", argv[0]);
        return 1;
    }

    // 2. Проверка длины строки прав доступа (должна быть ровно 9 символов)
    if (strlen(argv[2]) != 9)
    {
        fprintf(stderr, "Error: permissions string must be exactly 9 characters long (e.g., rwxr--r--)\n");
        return 1;
    }

    mode_t mode = 0;
    char *p = argv[2];

    // 3. Парсинг прав для владельца (User)
    if (p[0] == 'r')
        mode |= S_IRUSR;
    if (p[1] == 'w')
        mode |= S_IWUSR;
    if (p[2] == 'x')
        mode |= S_IXUSR;

    // 4. Парсинг прав для группы (Group)
    if (p[3] == 'r')
        mode |= S_IRGRP;
    if (p[4] == 'w')
        mode |= S_IWGRP;
    if (p[5] == 'x')
        mode |= S_IXGRP;

    // 5. Парсинг прав для остальных (Other)
    if (p[6] == 'r')
        mode |= S_IROTH;
    if (p[7] == 'w')
        mode |= S_IWOTH;
    if (p[8] == 'x')
        mode |= S_IXOTH;

    // 6. Сброс umask, чтобы права применились точно так, как указано в строке
    umask(0);

    // 7. Создание файла
    int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, mode);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    // 8. Закрытие файлового дескриптора
    close(fd);

    printf("File '%s' created successfully with permissions '%s'.\n", argv[1], argv[2]);
    return 0;
}