// server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建监听的套接字
    // 第一步，调用socket函数，创建一个监听套接字lfd，这个套接字用于监听客户端的连接请求。
    // socket函数的三个参数分别指定了地址族（AF_INET表示IPv4协议）、套接字类型（SOCK_STREAM表示流式套接字，对应TCP协议）和协议类型（0表示自动选择）。
    // 如果socket函数返回-1，表示创建失败，需要打印错误信息并退出程序。

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 将socket()返回值和本地的IP端口绑定到一起
    // 第二步，调用bind函数，将监听套接字lfd和本地的IP地址和端口号绑定到一起。
    // bind函数的第二个参数是一个sockaddr_in结构体的指针，这个结构体用于存储IP地址和端口号的信息。
    // 结构体中的sin_family字段指定了地址族（AF_INET表示IPv4协议），sin_port字段指定了端口号（htons函数用于将主机字节序转换为网络字节序），sin_addr字段指定了IP地址（INADDR_ANY宏表示任意一个本机IP地址）。
    // 如果bind函数返回-1，表示绑定失败，需要打印错误信息并退出程序。


    //     struct sockaddr_in {
    //     short int sin_family;        // 2 字节 ，地址族，e.g. AF_INET, AF_INET6
    //     unsigned short int sin_port; // 2 字节 ，16位TCP/UDP 端口号 e.g. htons (3490)，
    //     struct in_addr sin_addr;     // 4 字节 ，32位IP地址
    //     struct in_addr {
    //         unsigned long s_addr;    // 32位IPV4地址打印的时候可以调用inet_ntoa ()函数将其转换为char *类型.
    //     };
    //     unsigned char sin_zero [8]; // 8 字节 ，填充的字节，不使用
    //  };
    // 注意：sin_port和sin_addr都必须是网络字节序（NBO）即大端字节序

    struct sockaddr_in addr; 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   //htons(Host to Network Short) 将16位主机字节序转换为网络字节序     
    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表任意一个IP地址
    // 这个宏一般用于本地的绑定操作
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0,即表示IP地址0.0.0.0

    // 这个函数的作用是将字符串形式的IP地址转换为二进制形式，并存储在指定的内存地址中。
    // inet_pton函数的参数:
    //          第一个参数是地址族，AF_INET表示IPv4协议
    //          第二个参数是字符串形式的IP地址，
    //          第三个参数是要存储二进制IP地址的内存地址。
    //    inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr); 

    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr)); //将监听套接字(socket):lfd和本地的IP地址和端口号绑定到一起
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3. 设置监听
    // 第三步，调用listen函数，设置监听套接字lfd为被动模式，等待客户端的连接请求。
    // listen函数的第二个参数指定了请求队列的长度，也就是能够同时处理的客户端连接数。
    // 如果listen函数返回-1，表示设置失败，需要打印错误信息并退出程序。
    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 4. 阻塞等待并接受客户端连接
    // 第四步，调用accept函数，阻塞等待并接受客户端的连接请求。accept函数会返回一个新的套接字cfd，这个套接字用于和客户端进行数据交换。
    // accept函数的第二个参数是一个sockaddr_in结构体的指针，这个结构体用于存储客户端的IP地址和端口号的信息。
    // accept函数的第三个参数是一个整型变量的指针，这个变量用于存储结构体的长度。
    // 如果accept函数返回-1，表示接受失败，需要打印错误信息并退出程序。如果成功接受客户端的连接请求，则打印客户端的IP地址和端口号。
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
    if(cfd == -1)
    {
        perror("accept");
        exit(0);
    }
    // 打印客户端的地址信息
    char ip[24] = {0};
    printf("客户端的IP地址: %s, 端口: %d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),    //将ip地址二进制转成字符串
           ntohs(cliaddr.sin_port));

    // 5. 和客户端通信
    // 首先，定义一个字符数组buf，用于存储接收或发送的数据，然后将其初始化为全0。 然后，调用read函数，从客户端套接字cfd中读取数据，存储在buf中，返回值len表示读取的字节数。
    // 如果len大于0，表示成功读取到数据，打印出客户端发送的内容，并调用write函数，将buf中的数据回写给客户端。
    // 如果len等于0，表示客户端断开了连接，打印出提示信息，并跳出循环。
    // 如果len小于0，表示读取失败，打印出错误信息，并跳出循环。

    // 套接字cfd是一个用于和客户端通信的文件描述符，它是通过accept函数返回的，表示已经建立了一个TCP连接。
    // 套接字cfd可以通过read和write函数来读写数据，也可以通过recv和send函数来读写数据。
    // 套接字cfd获得数据的方式是通过TCP协议的可靠传输机制，TCP协议会将数据分割成多个段（segment），每个段都有一个序号和确认号，以及一些控制位，用于保证数据的正确传输。
    // 套接字cfd能存多少数据取决于它的缓冲区的大小，缓冲区分为发送缓冲区和接收缓冲区，分别用于存储待发送的数据和已接收的数据。
    // 缓冲区的大小可以通过getsockopt和setsockopt函数来获取和设置。
    // 一般来说，发送缓冲区的大小是64KB，接收缓冲区的大小是256KB。
    // 当发送缓冲区满了或者接收缓冲区空了时，read或write函数会阻塞等待，直到有足够的空间或数据可用。
    
    //***********************************
    // int getsockopt(int socket, int level, int option_name, void *restrict option_value, socklen_t *restrict option_len);  获取套接字选项
    // 其中，socket是套接字的描述符，level是选项所在的协议层次，option_name是要获取的选项的名称，
    // option_value是指向缓冲区的指针，在该缓冲区中返回所请求的选项的值，option_len是指向optval缓冲区大小的指针

    int snd_buf, rcv_buf;
    socklen_t len;

    len = sizeof(snd_buf);
    //SOL_SOCKET表示套接字层次，SO_SNDBUF表示发送缓冲区选项。
    if (getsockopt(cfd, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len) == -1) {
        perror("getsockopt");
        exit(1);
    }

    len = sizeof(rcv_buf);
    if (getsockopt(cfd, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len) == -1) {
        perror("getsockopt");
        exit(1);
    }
    // 打印cfd socket缓存大小
    printf("Send buffer size: %d\n", snd_buf);      // 524288 64KB
    printf("Receive buffer size: %d\n", rcv_buf);   // 1048576 256KB
    //***********************************

    while(1)
    {
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("客户端say: %s\n", buf);
            write(cfd, buf, len);
        }
        else if(len  == 0)
        {
            printf("客户端断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    close(cfd);
    close(lfd);

    return 0;
}