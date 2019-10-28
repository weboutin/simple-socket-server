#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 9990 //端口号
#define SIZE 1024 //定义的数组大小

int creat_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); //创建一个负责监听的套接字
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;                /* Internet地址族 */
    addr.sin_port = htons(PORT);              /* 端口号 */
    addr.sin_addr.s_addr = htonl(INADDR_ANY); /* IP地址 */

    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)); //连接

    int listenResult = listen(server_socket, 5); //监听
    return server_socket;
}

int wait_client(int server_socket)
{
    struct sockaddr_in cliaddr;
    int addrlen = sizeof(cliaddr);
    printf("waiting connect .. \n");
    int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, &addrlen); //创建一个和客户端交流的套接字
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
        int ret = read(client_socket, buf, SIZE - 1);
        if (ret == -1)
        {
            perror("read");
            break;
        }
        if (ret == 0)
        {
            break;
        }
        buf[ret] = '\0';
        int i;
        for (i = 0; i < ret; i++)
        {
            buf[i] = buf[i] + 'A' - 'a';
        }

        printf("%s\n", buf);
        write(client_socket, buf, ret);

        if (strncmp(buf, "end", 3) == 0)
        {
            break;
        }
    }

    return 0;
}