#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        close(fd[1]);
        char resstring[14];
        ssize_t size = read(fd[0], resstring, 13);
        if (size > 0)
        {
            resstring[size] = '\0';
            printf("Потомок прочитал: %s\n", resstring);
        }
        close(fd[0]);
    }
    else
    {
        close(fd[0]);
        char msg[] = "Hello, world!";
        write(fd[1], msg, 13);
        printf("Родитель записал: %s\n", msg);
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}