#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int synchro = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int turn = 1;

void *numbers(void *arg)
{
    for (int i = 0; i <= 255; i++)
    {
        if (synchro)
        {
            pthread_mutex_lock(&mutex);
            while (turn != 1)
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%d\t", i);
        fflush(stdout);
        if (synchro)
        {
            turn = 2;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

void *chars(void *arg)
{
    for (int i = 0; i <= 255; i++)
    {
        if (synchro)
        {
            pthread_mutex_lock(&mutex);
            while (turn != 2)
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%c\n", (char)i);
        fflush(stdout);
        if (synchro)
        {
            turn = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--synchro") == 0)
        synchro = 1;

    pthread_t t1, t2;
    pthread_create(&t1, NULL, numbers, NULL);
    pthread_create(&t2, NULL, chars, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}