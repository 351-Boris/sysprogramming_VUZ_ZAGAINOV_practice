#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    // По умолчанию запускаем pwd, но можно передать свою команду в аргументах
    const char *cmd = (argc > 1) ? argv[1] : "pwd";

    printf("Родитель (PID %d): готовлюсь запустить утилиту '%s'\n", getpid(), cmd);

    pid_t pid = fork();

    // 1. Обработка ошибки создания дочернего процесса (требование методички)
    if (pid < 0)
    {
        perror("Ошибка fork()");
        exit(1);
    }

    if (pid == 0)
    {
        // Мы в дочернем процессе
        printf("Потомок (PID %d): запускаю '%s'\n", getpid(), cmd);

        // execlp ищет утилиту в PATH.
        // Первый аргумент - имя файла, второй - argv[0] (обычно совпадает), далее NULL
        execlp(cmd, cmd, NULL);

        // ВАЖНО: Если execlp успешен, код ниже НИКОГДА не выполнится.
        // Если мы здесь, значит произошла ошибка (например, утилита не найдена).
        perror("Ошибка execlp()");
        exit(127); // Стандартный код ошибки "команда не найдена"
    }

    // Мы в родительском процессе
    int status;

    // 2. Ждем завершения КОНКРЕТНОГО дочернего процесса
    if (waitpid(pid, &status, 0) < 0)
    {
        perror("Ошибка waitpid()");
        return 1;
    }

    // 3. Анализ того, как завершился потомок
    if (WIFEXITED(status))
    {
        printf("Родитель: потомок (PID %d) успешно завершен с кодом %d\n",
               pid, WEXITSTATUS(status));
    }
    else
    {
        printf("Родитель: потомок (PID %d) завершен аварийно\n", pid);
    }

    return 0;
}