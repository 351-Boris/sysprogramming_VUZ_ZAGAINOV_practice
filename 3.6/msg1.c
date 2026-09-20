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
    m.mtype = 1;
    strcpy(m.mtext, "Hello from process 1");
    msgsnd(qid, &m, strlen(m.mtext) + 1, 0);
    printf("1 отправил тип 1\n");

    if (msgrcv(qid, &m, 256, 2, 0) < 0)
    {
        perror("msgrcv");
        exit(1);
    }
    printf("1 получил: %s\n", m.mtext);
    return 0;
}