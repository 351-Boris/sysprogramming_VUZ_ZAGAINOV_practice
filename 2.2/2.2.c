#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Функция теперь принимает весь st_mode, чтобы определить и тип файла
void print_mode(mode_t m)
{
    char s[11] = "----------"; // 10 символов + '\0'

    // 1. Определяем тип файла (первый символ)
    if (S_ISREG(m))
        s[0] = '-';
    else if (S_ISDIR(m))
        s[0] = 'd';
    else if (S_ISLNK(m))
        s[0] = 'l';
    else
        s[0] = '?';

    // 2. Определяем права доступа
    if (m & S_IRUSR)
        s[1] = 'r';
    if (m & S_IWUSR)
        s[2] = 'w';
    if (m & S_IXUSR)
        s[3] = 'x';
    if (m & S_IRGRP)
        s[4] = 'r';
    if (m & S_IWGRP)
        s[5] = 'w';
    if (m & S_IXGRP)
        s[6] = 'x';
    if (m & S_IROTH)
        s[7] = 'r';
    if (m & S_IWOTH)
        s[8] = 'w';
    if (m & S_IXOTH)
        s[9] = 'x';

    printf("%s ", s);
}

void print_stat(const char *name, struct stat *st)
{
    print_mode(st->st_mode);

    // Исправление проблемы с \n в ctime: заменяем последний символ на '\0'
    char *time_str = ctime(&st->st_mtime);
    time_str[strcspn(time_str, "\n")] = '\0';

    printf("%ld %d %d %ld %s %s\n",
           (long)st->st_nlink, st->st_uid, st->st_gid,
           (long)st->st_size, time_str, name);
}

mode_t parse_perms(const char *p)
{
    // Добавлена защита от выхода за границы массива
    if (strlen(p) != 9)
    {
        fprintf(stderr, "Error: permissions string must be exactly 9 characters long.\n");
        exit(1);
    }

    mode_t mode = 0;
    if (p[0] == 'r')
        mode |= S_IRUSR;
    if (p[1] == 'w')
        mode |= S_IWUSR;
    if (p[2] == 'x')
        mode |= S_IXUSR;
    if (p[3] == 'r')
        mode |= S_IRGRP;
    if (p[4] == 'w')
        mode |= S_IWGRP;
    if (p[5] == 'x')
        mode |= S_IXGRP;
    if (p[6] == 'r')
        mode |= S_IROTH;
    if (p[7] == 'w')
        mode |= S_IWOTH;
    if (p[8] == 'x')
        mode |= S_IXOTH;

    return mode;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s --fstat|--lstat|--chmod ...\n", argv[0]);
        fprintf(stderr, "Examples:\n");
        fprintf(stderr, "  %s --fstat filename\n", argv[0]);
        fprintf(stderr, "  %s --lstat symlink_name\n", argv[0]);
        fprintf(stderr, "  %s --chmod rwxr-xr-x filename\n", argv[0]);
        return 1;
    }

    struct stat st;

    if (strcmp(argv[1], "--fstat") == 0)
    {
        if (stat(argv[2], &st) < 0)
        {
            perror("stat");
            return 1;
        }
        print_stat(argv[2], &st);
    }
    else if (strcmp(argv[1], "--lstat") == 0)
    {
        if (lstat(argv[2], &st) < 0)
        {
            perror("lstat");
            return 1;
        }
        print_stat(argv[2], &st);
    }
    else if (strcmp(argv[1], "--chmod") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Usage: %s --chmod perms file\n", argv[0]);
            return 1;
        }
        mode_t mode = parse_perms(argv[2]);
        if (chmod(argv[3], mode) < 0)
        {
            perror("chmod");
            return 1;
        }
        printf("Permissions of '%s' successfully changed to '%s'.\n", argv[3], argv[2]);
    }
    else
    {
        fprintf(stderr, "Unknown option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}