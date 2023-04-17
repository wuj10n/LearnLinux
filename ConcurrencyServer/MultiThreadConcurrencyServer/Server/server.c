#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

// 这段代码是一个简单的TCP服务器程序，使用多线程来处理多个客户端的连接和通信。

// 代码中定义了一个结构体SockInfo，用来存储每个客户端的套接字、线程ID和地址信息。
struct SockInfo
{
    int fd;                      // 通信
    pthread_t tid;               // 线程ID
    struct sockaddr_in addr;     // 地址信息
};

// 然后定义了一个全局数组infos，用来保存所有客户端的SockInfo结构体。
struct SockInfo infos[128];     

// 代码中还定义了一个工作函数working，用来在子线程中接收和发送数据。
// 工作函数的参数是一个SockInfo结构体的指针，通过它可以访问客户端的套接字和地址信息。
// 工作函数中使用一个循环来不断地从客户端读取数据，并原样回写给客户端。
// 如果读取失败或者客户端关闭连接，就将该客户端的套接字设置为-1，表示空闲，并退出循环和子线程。
void* working(void* arg)
{
    while(1)
    {
        struct SockInfo* info = (struct SockInfo*)arg;
        // 接收数据
        char buf[1024];
        // read函数用来从套接字中读取数据，第一个参数是套接字描述符，第二个参数是缓冲区，第三个参数是缓冲区大小，返回值是实际读取的字节数
        int ret = read(info->fd, buf, sizeof(buf)); 
        if(ret == 0)
        {
            printf("客户端已经关闭连接...\n");
            info->fd = -1;
            break;
        }
        else if(ret == -1)
        {
            printf("接收数据失败...\n");
            info->fd = -1;
            break;
        }
        else
        {
            // write函数用来向套接字中写入数据，第一个参数是套接字描述符，第二个参数是缓冲区，第三个参数是要写入的字节数，返回值是实际写入的字节数
            write(info->fd, buf, strlen(buf)+1); 
        }
    }
    return NULL;
}
int main()
{
    // 1. 创建用于监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0); // socket函数用来创建一个套接字，第一个参数是地址族，AF_INET表示IPv4，第二个参数是套接字类型，SOCK_STREAM表示TCP，第三个参数是协议类型，0表示自动选择，返回值是套接字描述符
    if(fd == -1)
    {
        perror("socket"); // perror函数用来打印错误信息，参数是错误提示字符串
        exit(0); // exit函数用来退出程序，参数是退出码
    }

    // 2. 绑定
    struct sockaddr_in addr; // sockaddr_in结构体用来存储IPv4地址信息，包括地址族、端口号和IP地址
    addr.sin_family = AF_INET;          // ipv4
    addr.sin_port = htons(8989);        // 字节序应该是网络字节序，htons函数用来将主机字节序转换为网络字节序
    addr.sin_addr.s_addr =  INADDR_ANY; // == 0, 获取IP的操作交给了内核，INADDR_ANY表示任意IP地址
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr)); // bind函数用来将套接字和地址信息绑定在一起，第一个参数是套接字描述符，第二个参数是地址信息的指针，需要强制转换为sockaddr类型，第三个参数是地址信息的大小，返回值是0表示成功，-1表示失败
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3.设置监听
    ret = listen(fd, 100); // listen函数用来设置监听队列的大小，第一个参数是套接字描述符，第二个参数是队列的最大长度，返回值是0表示成功，-1表示失败
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 4. 等待, 接受连接请求
    int len = sizeof(struct sockaddr); // len变量用来存储地址信息的大小
     int max = sizeof(infos) / sizeof(infos[0]); // max变量用来存储infos数组的长度
    for(int i=0; i<max; ++i)
    {  
        bzero(&infos[i], sizeof(infos[i])); // bzero函数用来将内存区域清零，第一个参数是内存区域的指针，第二个参数是内存区域的大小
        infos[i].fd = -1; // 将每个客户端的套接字描述符初始化为-1，表示空闲
        infos[i].tid = -1; // 将每个客户端的线程ID初始化为-1，表示空闲
    }

    // 父进程监听, 子线程通信
    while(1)
    {
        // 创建子线程
        struct SockInfo* pinfo; // pinfo变量用来指向一个空闲的SockInfo结构体
        for(int i=0; i<max; ++i)
        {
            if(infos[i].fd == -1) 
            {
                pinfo = &infos[i]; // 如果找到一个空闲的SockInfo结构体，就将pinfo指向它，并跳出循环
                break;
            }
            if(i == max-1)
            {
                sleep(1); // 如果没有找到空闲的SockInfo结构体，就让主线程休眠一秒，然后重新遍历数组
                i--;
            }
        }
        int connfd = accept(fd, (struct sockaddr*)&pinfo->addr, &len); // accept函数用来接受客户端的连接请求，第一个参数是监听套接字描述符，第二个参数是客户端地址信息的指针，需要强制转换为sockaddr类型，第三个参数是地址信息大小的指针，返回值是用于通信的套接字描述符
        printf("parent thread, connfd: %d\n", connfd);
        if(connfd == -1)
        {
            perror("accept");
            exit(0);
        }
        pinfo->fd = connfd; // 将通信套接字描述符保存到pinfo结构体中
        pthread_create(&pinfo->tid, NULL, working, pinfo); // pthread_create函数用来创建一个子线程，第一个参数是线程ID的指针，第二个参数是线程属性，NULL表示默认属性，第三个参数是线程函数的指针，第四个参数是线程函数的参数，返回值是0表示成功，非0表示失败
        pthread_detach(pinfo->tid); // pthread_detach函数用来让子线程分离，不需要主线程回收，参数是线程ID，返回值是0表示成功，非0表示失败
    }
    // 释放资源
    close(fd);  // 监听
    return 0;
}