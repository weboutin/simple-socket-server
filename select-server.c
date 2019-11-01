#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

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
    fd_set *readfds;
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

    int max_fd = server_socket;
    while (1)
    {
        printf("Loop ~~~\n");
        FD_ZERO(readfds);
        FD_SET(server_socket, readfds);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] > 0)
            {
                FD_SET(client_sockets[i], readfds);
                max_fd = max_fd + client_sockets[i];
            }
        }
        int selectResult = select(max_fd + 1, readfds, NULL, NULL, NULL);
        struct sockaddr_in cliaddr;
        int addrlen = sizeof(cliaddr);
        if (FD_ISSET(server_socket, readfds))
        {
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = accept(server_socket, (struct sockaddr *)&cliaddr, &addrlen);
                    printf("socketId==>%d client_sockets [%d] accept success, IP => %s\n", client_sockets[i], i, inet_ntoa(cliaddr.sin_addr));
                    break;
                }
            }
        }
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (FD_ISSET(client_sockets[i], readfds))
            {
                char buf[SIZE];
                int bufSize = read(client_sockets[i], buf, SIZE - 1);
                if (bufSize == -1)
                {
                    client_sockets[i] = 0;
                    close(client_sockets[i]);
                }
                if (bufSize == 0)
                {
                    client_sockets[i] = 0;
                    close(client_sockets[i]);
                }
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

    printf("end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}