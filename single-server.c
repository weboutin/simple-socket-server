#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 9990
#define SIZE 1024

int creat_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    printf("bindResult ==> %d\n", bindResult);
    if (bindResult == -1)
    {

        perror("bindResult");
    }

    int listenResult = listen(server_socket, 5);
    printf("listenResult ==> %d\n", listenResult);
    if (bindResult == -1)
    {

        perror("listenResult");
    }
    return server_socket;
}

int wait_client(int server_socket)
{
    struct sockaddr_in cliaddr;
    int addrlen = sizeof(cliaddr);
    printf("waiting connect .. \n");
    int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, &addrlen);
    printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));

    return client_socket;
}

int main()
{
    int server_socket = creat_socket();

    int client_socket = wait_client(server_socket);

    char buf[SIZE];
    while (1)
    {
        int bufSize = read(client_socket, buf, SIZE - 1);
        if (bufSize == -1)
        {
            break;
        }
        if (bufSize == 0)
        {
            break;
        }
        printf("bufSize: ==> %d\n", bufSize);
        buf[bufSize] = '\0';
        printf("From client: %s\n", buf);
        write(client_socket, buf, bufSize);

        if (strncmp(buf, "end", 3) == 0)
        {
            break;
        }
    }

    close(client_socket);
    close(server_socket);

    return 0;
}