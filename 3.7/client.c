#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCK_PATH "/tmp/chat.sock"

int main()
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    fd_set read_fds;
    char buf[1024];
    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(fd, &read_fds);
        int maxfd = fd > 0 ? fd : 0;

        if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(0, &read_fds))
        {
            int n = read(0, buf, sizeof(buf) - 1);
            if (n <= 0)
                break;
            send(fd, buf, n, 0);
        }

        if (FD_ISSET(fd, &read_fds))
        {
            int n = recv(fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0)
                break;
            buf[n] = '\0';
            printf("Сообщение: %s", buf);
        }
    }
    close(fd);
    return 0;
}