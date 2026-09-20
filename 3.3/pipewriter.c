#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

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
        char fdstr[16];
        sprintf(fdstr, "%d", fd[0]);
        execl("./reader", "reader", fdstr, NULL);
        perror("execl");
        exit(1);
    }
    else
    {
        close(fd[0]);
        char msg[] = "Hello, world!";
        write(fd[1], msg, sizeof(msg));
        printf("Родитель записал: %s\n", msg);
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}