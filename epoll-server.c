#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/epoll.h>

#define PORT 9990
#define SIZE 1024
#define MAX_CLIENTS 10
#define MAX_EVENTS 10

int creat_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
    {
        perror("bindResult");
    }

    int listenResult = listen(server_socket, 5);
    if (listenResult == -1)
    {
        perror("listenResult");
    }
    printf("server start\n");
    return server_socket;
}

int main()
{
    int listen_sock = creat_socket();

    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd;
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
    }
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
    }

    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

    while (1)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
        }

        for (int n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == listen_sock)
            {
                struct sockaddr_in cliaddr;
                int addrlen = sizeof(cliaddr);
                conn_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
                if (conn_sock == -1)
                {
                    perror("accept");
                }

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (client_sockets[i] == 0)
                    {
                        client_sockets[i] = conn_sock;
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = client_sockets[i];
                        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockets[i], &ev) == -1)
                        {
                            perror("epoll_ctl: conn_sock");
                        }
                        break;
                    }
                }
            }
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] > 0 && events[n].data.fd == client_sockets[i])
                {
                    char buf[SIZE];
                    int bufSize = read(client_sockets[i], buf, SIZE - 1);
                    buf[bufSize] = '\0';
                    printf("From client: %s\n", buf);
                }
            }
        }
    }

    printf("server end\n");

    close(listen_sock);

    return 0;
}