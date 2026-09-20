#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);

    int c = accept(s, NULL, NULL);
    char buf[1024];
    int n = recv(c, buf, sizeof(buf) - 1, 0);
    if (n > 0)
    {
        buf[n] = '\0';
        printf("Получено: %s\n", buf);
        send(c, "OK", 2, 0);
    }
    close(c);
    close(s);
    return 0;
}