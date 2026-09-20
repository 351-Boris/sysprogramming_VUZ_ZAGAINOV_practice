#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    char buf[1024];
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);

    while (1)
    {
        int n = recvfrom(s, buf, sizeof(buf) - 1, 0,
                         (struct sockaddr *)&cli, &len);
        if (n < 0)
            break;
        buf[n] = '\0';
        printf("Получено: %s\n", buf);
        sendto(s, "OK", 2, 0, (struct sockaddr *)&cli, len);
    }
    close(s);
    return 0;
}