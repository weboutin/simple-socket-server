#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>

#define PORT 9990
#define SIZE 1024
#define MAX_CLIENTS 10

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

int wait_client(int server_socket)
{
    struct pollfd pollfds[10];
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN | POLLPRI;

    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

    while (1)
    {
        // printf("Loop ~ \n");
        int pollResult = poll(pollfds, 2, 5000);
        if (pollResult > 0)
        {
            if (pollfds[0].revents & POLLIN)
            {
                struct sockaddr_in cliaddr;
                int addrlen = sizeof(cliaddr);
                int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, &addrlen);
                pollfds[1].fd = client_socket;
                pollfds[1].events = POLLIN | POLLPRI;
            }
            if (pollfds[1].revents & POLLIN)
            {
                char buf[SIZE];
                int bufSize = read(pollfds[1].fd, buf, SIZE - 1);
                buf[bufSize] = '\0';
                printf("From client: %s\n", buf);
            }
        }
    }
}

int main()
{
    int server_socket = creat_socket();

    int client_socket = wait_client(server_socket);

    printf("server end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}