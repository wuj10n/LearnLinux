// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    // 首先，调用了socket函数，创建了一个套接字，也就是一个网络通信的端点。
    // 指定了AF_INET参数，表示使用IPv4协议，SOCK_STREAM参数，表示使用TCP协议，0参数，表示使用默认的协议。
    // 如果socket函数成功，它会返回一个非负的整数，表示套接字的文件描述符。如果失败，它会返回-1，并设置errno变量，表示错误的原因。
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 连接服务器
    // 然后，定义了一个sockaddr_in结构体变量addr，用于存储服务器的地址信息。
    // 设置了addr.sin_family为AF_INET，表示使用IPv4协议，addr.sin_port为htons(10000)，表示使用10000端口，htons函数是用于将主机字节序转换为网络字节序。
    // 你调用了inet_pton函数，将字符串形式的IP地址"192.168.237.131"转换为二进制形式，并存储在addr.sin_addr.s_addr中。
    // 接着，调用了connect函数，尝试与服务器建立连接。
    // 传入了套接字文件描述符fd，服务器地址信息的指针(struct sockaddr*)&addr，以及地址信息的长度sizeof(addr)。
    // 如果connect函数成功，它会返回0，并在内核中建立一个TCP连接。如果失败，它会返回-1，并设置errno变量，表示错误的原因。
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口

    // inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);

    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表任意一个IP地址
    // 这个宏一般用于本地的绑定操作
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0,即表示IP地址0.0.0.0 

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }

    // 3. 和服务器端通信

    int number = 0;
    while(1)
    {
        // 发送数据
        char buf[1024];
        sprintf(buf, "你好, 服务器...%d\n", number++);
        write(fd, buf, strlen(buf)+1);
        
        // 接收数据
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("服务器say: %s\n", buf);
        }
        else if(len  == 0)
        {
            printf("服务器断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        sleep(1);   // 每隔1s发送一条数据
    }

    close(fd);

    return 0;
}