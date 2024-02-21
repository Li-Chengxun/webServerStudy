http连接处理类
===============

根据状态转移,通过主从状态机封装了http连接类。其中,主状态机在内部调用从状态机,从状态机将处理状态和数据传给主状态机

> * 客户端发出http连接请求
> * 从状态机读取数据,更新自身状态和接收数据,传给主状态机
> * 主状态机根据从状态机状态,更新自身状态,决定响应请求还是继续读取

---



### 基础知识

#### 什么是套接字地址？

套接字地址一个IP地址和端口对 （ip，port）。它唯一地表示了使用TCP通信的一端。

#### 套接字一些基础API

​		socket的主要API都定义在`<sys/socket.h>`头文件 中，包括创建socket、命名socket、监听socket、接受连接、发起连接、 读写数据、获取地址信息、检测带外标记，以及读取和设置socket选项。

​		个人理解：socket用来创建一个类似于插座的东西，bind用来绑定插座和地址。这个插座其实就是文件描述符，服务器与客户端通过文件描述符通信。

​	下面来介绍这些API

```c++
//创建套接字
//domain表示使用什么协议族，对于TCP/IP协议族对应的参数的为PF_INET(IPv4)和PF_INET6(IPv6);

//type参数指定服务类型。服务类型主要有SOCK_STREAM服务(流服务)和SOCK_UGRAM(数据报)服务。对TCP/IP协议族而言，其值取SOCK_STREAM表示传输层使用TCP协议，取SOCK_DGRAM表示传输层使用UDP协议。

//protocol参数是在前两个参数构成的协议集合下，再选择一个具体的协议。不过这个值通常都是唯一的(前两个参数已经完全决定了它的值)。几乎在所有情况下，我们都应该把它设置为0，表示使用默认协议。

//当成功时候返回一个socket的文件描述符，失败返回-1并且设置errno
int socket(int domain,int type,int protocol);

```

```c++
//将一个socket与socket地址绑定称为给socket命名。
//socketfd就是创建stocket的时候返回的文件描述符
//my_addr所指的socket地址分配给未命名的sockfd文件描述符，addrlen参数指出该socket地址的长度
//sockaddr结构如下:
struct sockaddr
{
    sa_family_t sa_family;
    char sa_data[14];
}
//命名socket
//bind成功时返回0，失败则返回-1并设置errno。
int bind(int sockfd,const struct sockaddr*my_addr,socklen_t
addrlen);
```

```c++
//监听socket
//创建socket后，开始监听，等待客户端连接
//sockfd参数指定被监听的socket。backlog参数提示内核监听队列的最大长度。监听队列的长度如果超过backlog，服务器将不受理新的客户连接。

//listen成功时返回0，失败则返回-1并设置errno。
int listen(int sockfd,int backlog);
```

```c++
//当有客户端连接时，接收连接
//socketfd表示监听的socket文件描述符，addr用来存储连接到服务器的地址
//accept成功时返回一个新的连接socket，该socket唯一地标识了被接受的这个连接，服务器可通过读写该socket来与被接受连接对应的客户端通信。accept失败时返回-1并设置errno。
int accept(int sockfd,struct sockaddr*addr,socklen_t*addrlen);
```

```c++
//发起连接
//客户端可以主动发起连接
int connect(int sockfd,const struct sockaddr*serv_addr,socklen_t
addrlen);
```

```c++
//关闭连接
//shutdown成功时返回0，失败则返回-1并设置errno
int close(int fd);
```

#### 数据读写

```c++
//读写API
/*
recv读取sockfd上的数据，buf和len参数分别指定读缓冲区的位置和大小，flags参数的含义见后文，通常设置为0即可。recv成功时返回实际读取到的数据的长度，它可能小于我们期望的长度len。因此我们可能要多次调用recv，才能读取到完整的数据。recv可能返回0，这意味着通信对方已经关闭连接了。recv出错时返回-1并设置errno。send往sockfd上写入数据，buf和len参数分别指定写缓冲区的位置和大小。send成功时返回实际写入的数据的长度，失败则返回-1并设置
errno
*/
ssize_t recv(int sockfd,void*buf,size_t len,int flags);
ssize_t send(int sockfd,const void*buf,size_t len,int flags);
```



flag含义如下：

![](img/flag含义.png)































