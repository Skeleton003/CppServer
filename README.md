# 自制C++服务器

最新可执行版本：[day13_cmake](https://github.com/Skeleton003/CppServer/tree/main/day13_cmake)

build & run:

```shell
mkdir build && cd build
cmake .. # for debug, add -DCMAKE_BUILD_TYPE=DEBUG

make format      # optional
make cpplint      # optional
make clang-tidy  # optional
make

cd test
make server
make multiple_client
make single_client

cd ../bin
./server

# in another terminal
./multiple_client -t 10000 -m 10 -w 10 
```

## day01

使用`socket`、`bind`、`listen`、`connect`、`accept`等系统调用，实现了一个简单的server和client。

## day02

封装了一个错误处理函数：

```cpp
void errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
```

使用`read`和`write`实现了一个简单的 echo 服务器。

## day03

利用`epoll`实现IO多路复用，支持多个客户端同时连接。

## day04

封装了`Socket`、`epoll`类，精简了主程序，隐藏了底层语言实现细节、增加可读性。

## day05

将一个文件描述符封装成一个`Channel`类，一个`Channel`类自始至终只负责一个文件描述符，对不同的服务、不同的事件类型，都可以在类中进行不同的处理。

## day06

将服务器改造成Reactor模式，将整个服务器抽象成一个`Server`类，这个类中有一个`main-Reactor`（暂无`sub-Reactor`），里面的核心是一个`EventLoop`事件循环类。添加需要监听的事务到这个事件循环内，每次有事件发生时就会通知（在程序中返回`Channel`），然后根据不同的描述符、事件类型进行处理。

## day07

分离接受连接这一模块，添加一个`Acceptor`类，这个类有以下几个特点：

- 类存在于事件驱动`EventLoop`类中，也就是Reactor模式的`main-Reactor`
- 类中的socket fd就是服务器监听Socket的socket fd，每一个`Acceptor`对应一个socket fd
- 这个类也通过一个独有的`Channel`负责分发到`epoll`，该`Channel`的事件处理函数`handleEvent()`会调用`Acceptor`中的接受连接函数来新建一个TCP连接。

## day08

把TCP连接抽象成一个`Connection`类，这个类也有以下几个特点：

- 类存在于事件驱动`EventLoop`类中，也就是Reactor模式的`main-Reactor`
- 类中的socket fd就是客户端的socket fd，每一个`Connection`对应一个socket fd
- 每一个类的实例通过一个独有的`Channel`负责分发到`epoll`，该`Channel`的事件处理函数`handleEvent()`会调用`Connection`中的事件处理函数来响应客户端请求
  
`Connection`类和`Acceptor`类是平行关系、十分相似，他们都直接由`Server`管理，由一个`Channel`分发到`epoll`，通过回调函数处理相应事件。

## day 09

封装了`Buffer`缓冲区类。

## day 10

构建了简单的线程池。

## day 11

完善了线程池，加入一个简单的测试程序。

## day 12

将服务器改写为多Reactor多线程模式。此模式的特点为：

- 服务器一般只有一个`mainReactor`，有多个`subReactor`。
- 服务器管理一个线程池，每一个`subReactor`由一个线程来负责`Connection`上的事件循环，事件执行也在这个线程中完成。
- `mainReactor`只负责`Acceptor`建立新连接，然后将这个连接分配给一个`subReactor`。

## day 13

引入Cmake，将代码工程化。
