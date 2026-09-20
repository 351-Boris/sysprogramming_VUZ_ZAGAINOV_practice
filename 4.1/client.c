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
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char msg[] = "Hello UDP";
    sendto(s, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof(addr));

    char buf[1024];
    int n = recvfrom(s, buf, sizeof(buf) - 1, 0, NULL, NULL);
    if (n > 0)
    {
        buf[n] = '\0';
        printf("Ответ: %s\n", buf);
    }
    close(s);
    return 0;
}