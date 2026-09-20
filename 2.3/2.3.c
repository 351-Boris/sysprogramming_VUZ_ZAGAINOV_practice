#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // Для PATH_MAX

void print_mode(mode_t m)
{
    char s[11] = "----------";

    // 1. Определяем тип файла (первый символ, как в ls -l)
    if (S_ISREG(m))
        s[0] = '-';
    else if (S_ISDIR(m))
        s[0] = 'd';
    else if (S_ISLNK(m))
        s[0] = 'l';
    else if (S_ISCHR(m))
        s[0] = 'c';
    else if (S_ISBLK(m))
        s[0] = 'b';
    else if (S_ISFIFO(m))
        s[0] = 'p';
    else if (S_ISSOCK(m))
        s[0] = 's';

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

    printf("%s", s);
}

int main(int argc, char **argv)
{
    // Если аргумент не передан, используем текущий каталог "."
    const char *dir = (argc > 1) ? argv[1] : ".";

    DIR *d = opendir(dir);
    if (!d)
    {
        perror("opendir");
        return 1;
    }

    struct dirent *e;
    char path[PATH_MAX];

    while ((e = readdir(d)) != NULL)
    {
        // Формируем полный путь для lstat
        snprintf(path, sizeof(path), "%s/%s", dir, e->d_name);

        struct stat st;
        // lstat считывает атрибуты самой ссылки, а не файла за ней
        if (lstat(path, &st) < 0)
        {
            perror("lstat");
            continue;
        }

        // Печатаем тип и права
        print_mode(st.st_mode);

        // Получаем имена владельца и группы (работа со статической памятью)
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);

        // Форматирование времени (Имитация поведения ls)
        char tbuf[64];
        time_t now = time(NULL);

        // Если файл изменен более ~6 месяцев назад, ls показывает год, иначе время
        if (difftime(now, st.st_mtime) > 60 * 60 * 24 * 182)
        {
            strftime(tbuf, sizeof(tbuf), "%b %d  %Y", localtime(&st.st_mtime));
        }
        else
        {
            strftime(tbuf, sizeof(tbuf), "%b %d %H:%M", localtime(&st.st_mtime));
        }

        printf(" %ld %-8s %-8s %8ld %s %s\n",
               (long)st.st_nlink,
               pw ? pw->pw_name : "?",
               gr ? gr->gr_name : "?",
               (long)st.st_size,
               tbuf,
               e->d_name);
    }

    closedir(d);
    return 0;
}