#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

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

    int shmid = shmget(key, 65536, 0666);
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

    printf("%s\n", shm);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}