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
#define MAX_CLIENTS 5

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
    fd_set rdset;
    FD_SET(server_socket, &rdset);
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }
    int flag = 0;

    while (1)
    {
        int selectResult = select(server_socket + 1, &rdset, NULL, NULL, NULL);
        struct sockaddr_in cliaddr;
        int addrlen = sizeof(cliaddr);
        if (FD_ISSET(server_socket, &rdset))
        {
            client_sockets[flag] = accept(server_socket, (struct sockaddr *)&cliaddr, &addrlen);
            flag++;
            printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));
            FD_SET(client_sockets[flag], &rdset);
        }
        // for (int i = 0; i < MAX_CLIENTS; i++)
        // {
            // printf("check sockets\n");
            // printf("select Result ===>%d\n", selectResult);
            // if (FD_ISSET(client_sockets[i], &rdset))
            // {
            //     printf("flag=>> %d\n", flag);
            //     printf("helloworld\n");
            //     char buf[SIZE];
            //     int bufSize = read(client_sockets[i], buf, SIZE - 1);
            //     buf[bufSize] = '\0';
            //     printf("From client: %s\n", buf);
            // }
        // }

        // char buf[SIZE];
        // int bufSize = read(client_socket, buf, SIZE - 1);
        // buf[bufSize] = '\0';
        // printf("From client: %s\n", buf);
        // write(client_socket, buf, bufSize);

        // if (strncmp(buf, "end", 3) == 0)
        // {
        //     break;
        // }
    }
}

int main()
{
    int server_socket = creat_socket();

    int client_socket = wait_client(server_socket);

    close(client_socket);
    close(server_socket);

    return 0;
}