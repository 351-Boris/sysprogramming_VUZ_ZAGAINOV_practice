#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    key_t key = ftok(argv[1], 'A');
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    char buf[65536];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0)
    {
        perror("read");
        return 1;
    }
    buf[n] = '\0';
    close(fd);

    int shmid = shmget(key, 65536, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        return 1;
    }

    char *shm = shmat(shmid, NULL, 0);
    if (shm == (char *)-1)
    {
        perror("shmat");
        return 1;
    }

    memcpy(shm, buf, n + 1);
    shmdt(shm);

    printf("Записано %ld байт в разделяемую память\n", (long)n);
    return 0;
}