#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCK_PATH "/tmp/chat.sock"
#define MAX_CLIENTS 10

int main()
{
    int listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    unlink(SOCK_PATH);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if (listen(listen_fd, 5) < 0)
    {
        perror("listen");
        exit(1);
    }

    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    fd_set master, read_fds;
    FD_ZERO(&master);
    FD_SET(listen_fd, &master);
    int maxfd = listen_fd;

    while (1)
    {
        read_fds = master;
        if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(listen_fd, &read_fds))
        {
            int client = accept(listen_fd, NULL, NULL);
            if (client >= 0)
            {
                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clients[i] == -1)
                    {
                        clients[i] = client;
                        FD_SET(client, &master);
                        if (client > maxfd)
                            maxfd = client;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if (fd == -1)
                continue;
            if (FD_ISSET(fd, &read_fds))
            {
                char buf[1024];
                int n = recv(fd, buf, sizeof(buf) - 1, 0);
                if (n <= 0)
                {
                    close(fd);
                    FD_CLR(fd, &master);
                    clients[i] = -1;
                }
                else
                {
                    buf[n] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clients[j] != -1 && clients[j] != fd)
                        {
                            send(clients[j], buf, n, 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}