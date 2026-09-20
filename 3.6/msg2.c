#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct msg
{
    long mtype;
    char mtext[256];
};

int main()
{
    int qid = msgget(1234, IPC_CREAT | 0666);
    if (qid < 0)
    {
        perror("msgget");
        exit(1);
    }

    struct msg m;
    m.mtype = 2;
    strcpy(m.mtext, "Hello from process 2");
    msgsnd(qid, &m, strlen(m.mtext) + 1, 0);
    printf("2 отправил тип 2\n");

    if (msgrcv(qid, &m, 256, 1, 0) < 0)
    {
        perror("msgrcv");
        exit(1);
    }
    printf("2 получил: %s\n", m.mtext);
    return 0;
}