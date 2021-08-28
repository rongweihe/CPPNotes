# 如何设计一个基于事件的非阻塞 TCP 网络库？

首先，我们需要知道，一个基于事件的非阻塞的网络库最核心的作用是什么：

### **TCP 网络编程本质论**

使用这种方式编程通常需要转化思维模式，把原来：

- 主动调用 recv 接收数据；
- 主动调用 accept 来接收连接；
- 主动调用 send 来发送数据；

的思路换成：

- 注册一个收数据的回调，网络库收到数据会调用我，直接把数据发送给我；
- 注册一个接收连接的回调，网络库接收到新连接会用我；直接把新连接给我；
- 需要发送数据的时候，直接往连接中写，网络库会负责无阻塞地发送。

常识普及：

##### 文件描述符fd

文件描述符（File descriptor）是计算机科学中的一个术语，是一个用于表述指向文件的引用的抽象化概念。**I/O的socket操作也是一种文件描述符fd**。 文件描述符在形式上是一个非负整数。实际上，它是一个索引值，指向内核为每一个进程所维护的该进程打开文件的记录表。当程序打开一个现有文件或者创建一个新文件时，内核向进程返回一个文件描述符。在程序设计中，一些涉及底层的程序编写往往会围绕着文件描述符展开。但是**文件描述符这一概念往往只适用于 UNIX、Linux 这样的操作系统**。

#### 调用bind的一般形式：

```C++
auto newCallable = bind(callable, arg_list);
```
该形式表达的意思是：当调用 newCallable 时，会调用callable，并传给它 arg_list 中的参数。

# 介绍

#### 网络核心库

muduo 是基于 Reactor 模式的网络库，其核心是个事件循环 EventLoop，用于响应计时器和 lO 事件。

muduo 采用基于对象(object-based ) 而非面向对象 ( object-oriented ) 的设计风格，其事件回调接口多以 boost::**function** + boost::**bind **表达，用户在使用 muduo 的时候不需要继承其中的 class。

网络库核心位于muduo/net和muduo/net/poller。

# 第一部分：公开接口

**EventLoop 事件循环（反应器 Reactor）：**每个线程只能有一个 EventLoop 实体，它负责 IO 和定时器事件的分派。它用 eventfd(2) 来异步唤醒，这有别于传统的的用一对的 pipe(2)，用 TimerQueue 作为计时器管理，用 Poller 作为 IO 多路复用。

扩展：

> eventfd 是一个计数器型的文件描述符
>
> 读写也就是 read/write，读写这个 fd 很容易理解，但是请注意了，只能 8 个字节。这个读写的内容其实是计数。
>
> 举个栗子：如下，我们连续写 3 次
>
> ```c
> // 写 3 次
> write(efd, &u /* u = 1 */ , 8)
> write(efd, &u /* u = 2 */ , 8)
> write(efd, &u /* u = 3 */ , 8)
> ```
>
> 你猜猜读的时候，是多少？
>
> ```c
> read(ebd, &x, 8)
> ```
>
> 读到的值是 6（因为 1+2+3），理解了吧。

**EventLoopThread：**启动一个线程，在其中运行 EventLoop::loop()。

**TcpConnection:** 整个网络库的核心，封装一次 TCP 连接，注意它不能发起连接。

**TCPClient：** 用于编写网络客户端，能发起连接，并且有重试功能。使用了 ConnectorPtr 成员来发起连接。

**TcpServer：**用于编写网络服务器，接受客户的连接。

在上面这些类中，TcpConnection 的生命周期靠 shared_ptr 管理（用户和库共同控制）。

# 第二部分：内部实现

- Channel 是  selectable IO channel，负责注册和响应 IO 事件，注意它不拥有 file descriptor 。它是 Acceptor、Connector、EventLoop、TimerQueue、TcpConnection 的成员，生命期由后者控制。
- Socket 是一个 RAII Handle，封装一个 file description ，并在析构的时候关闭 fd，它是 Acceptor、TcpConnection的成员，生命期由后者控制。EventLoop、TimerQueue 也拥有 fd，但不封装为 Socket class。
- SocketOps 封装各种 Sockets 系统调用。
- Poller 是 PollPoller 和 EPollPoller 的基类，采用“电平触发”语意，是 EventLoop 成员，生命期由后者控制。
-  PollPoller 和 EPollPoller  封装成两种 Poll 和 EPOLL 两种 IO 多路复用的实现，Poll 的价值在于可以调试。
- Connector 用于发起 TCP 连接，是 TcPClient 的成员，生命期由后者控制。
- Acceptor 用于接收连接，是  TcPServer 的成员，生命期由后者控制。
- TimerQueue 用 timerfd 定时，这有别于传统的 poll/epoll_wait 的等待时长的方法。TimerQueue 用 std::map 来管理 Timer，常用操作时间复杂度是 O(logN)，N 是定时器数目，是 EventLoop 成员，生命期由后者控制。
- EventLoopThreadPool 用于创建 IO 线程池，用于把 TcpConnetion 分派到某个 EventLoop 线程上。它是 TCPServer 的成员，生命期由后者控制。

# 第三部分：muduo 多线程模型

### **TCP 网络编程本质论**

使用这种方式编程通常需要转化思维模式，把原来：

- 主动调用 recv 接收数据；
- 主动调用 accept 来接收连接；
- 主动调用 send 来发送数据；

的思路换成：

- 注册一个收数据的回调，网络库收到数据会调用我，直接把数据发送给我；
- 注册一个接收连接的回调，网络库接收到新连接会用我；直接把新连接给我；
- 需要发送数据的时候，直接往连接中写，网络库会负责无阻塞地发送。

### TCP网络编程最本质的是处理三个半事件

- 连接的建立，包括服务端接受 (accept) 新连接和客户端成功发起 (connect) 连接。TCP 连接一旦建立，客户端和服务端是平等的，可以各自收发数据。

- 连接的断开，包括主动断开 ( close、shutdown ) 和被动断开 ( read(2)返回0)。

- 消息到达，文件描述符可读。这是最为重要的一个事件，对它的处理方式决定了网络编程的风格( 阻塞还是非阻塞，如何处理分包，应用层的缓冲如何设计等等)。

**在非阻塞网络编程中**，为什么要使用应用层发送缓冲区? 假设应用程序需要发送 40kB 数据，但是操作系统的 TCP 发送缓冲区只有 25kB 剩余空间，那么剩下的 15kB 数据怎么办? 如果等待 OS 缓冲区可用，会阻塞当前线程，因为不知道对方什么时候收到并读取数据。

因此网络库应该把这 15kB 数据缓存起来，放到这个 TCP 链接的应用层发送缓冲区中，等 socket 变得可写的时候立刻发送数据，这样“发送"操作不会阻塞。如果应用程序随后又要发送 50kB 数据，而此时发送缓冲区中尚有未发送的数据(若干kB), 那么网络库应该将这50kB 数据追加到发送缓冲区的末尾，而不能立刻尝试 write()，因为这样有可能打乱数据的顺序。

### muduo 内置的多线程方案：

也是 Netty 内置的多线程方案。这种方案的特点是 one loop per thread, 有一个main Reactor 负责 accept(2) 连接, 然后把连接挂在某个 sub Reactor 中( muduo采用round-robin的方式来选择 sub Reactor ), 这样该连接的所有操作都在那个sub Reactor 所处的线程中完成。多个连接可能被分派到多个线程中，以充分利用 CPU。

muduo 采用的是固定大小的 Reactor pool, 池子的大小通常根据 CPU 数目确定, 也就是说线程数是固定的，这样程序的总体处理能力不会随连接数增加而下降。另外，由于一个连接完全由一个线程管理，那么请求的顺序性有保证，突发请求也不会占满全部8个核(如果需要优化突发请求，可以考虑方案11)。这种方案把 IO 分派给多个线程，防止出现一个 Reactor 的处理能力饱和。

这种方案减少了进出 thread pool 的两次上下文切换，在把多个连接分散到多个Reactor 线程之后，小规模计算可以在当前 IO 线程完成并发回结果，从而降低响应的延迟。我认为这是一个适应性很强的多线程 IO 模型，因此把它作为 muduo 的默认线程模型。

![img](https://pic3.zhimg.com/80/v2-60809dcd0ed19efcc4004eefa87b1056_1440w.jpg)

书中用了一个银行柜台办理业务为比喻，简述各种模型的特点。

银行有旋转门，办理业务的客户人员从旋转门进出（IO）；银行也有柜台，客户在柜台办理业务（计算）。

要想办理业务，客户要先通过旋转门进入银行；办理完之后，客户要再次通过旋转门离开银行。一个客户可以办理多次业务，每次都必须从旋转门进出（TCP 长连接）。另外，旋转门一次只允许一个客户通过（无论进出），因为 read()/write() 只能同时调用其中一个。

**方案**9：这间大银行相当于包含方案5 中的多家小银行，每个客户进大门的时候就被固定分配到某一间小银行中，他的业务只能由这间小银行办理，他每次都要进出小银行的旋转门。但总体来看，大银行可以同时服务多个客户。这时同样要求办理业务时不能空等（阻塞），否则会影响分到同一间小银行的其他客户。而且必要的时候可以为VIP 客户单独开一间或几间小银行，优先办理VIP 业务。这跟方案5 不同，当普通客户在办理业务的时候，VIP 客户也只能在门外等着（见图6-11 的右图）。这是一种适应性很强的方案，也是muduo 原生的多线程IO 模型。

**方案**11：这间大银行有多个旋转门，多个柜台。旋转门和柜台之间没有一一对应关系，客户进大门的时候就被固定分配到某一旋转门中（奇怪的安排，易于实现线程安全的IO，见§4.6），进入旋转门之后，有一个队列，客户在此排队到柜台办理业务。这种方案的资源利用率可能比方案9 更高，一个客户不会被同一小银行的其他客户阻塞，但延迟也比方案9 略大。

# 第四部分：muduo 源码解读

### 为什么 TcpConnection::shutdown() 没有直接关闭 TCP 连接？

> muduo TcpConnection 没有提供 Close()，而只是提供了 shutdown()，这么做是为了收发数据的完整性。
>
> TCP 是一个全双工协议，同一个文件描述符既可读又可以写，shutdown() 关闭了“写”方向的连接，保留了“读”方向，这称为 TCP half-close。如果 直接 close(socket_fd) 那么 socket_fd 就不能读写了。
>
> 用 shutdown 半关闭的效果是，如果对方已经发送了数据，这些数据还在路上，那么，muduo 不会漏收这些数据。换句话说，muduo 在 TCP 这一层面解决了“当你打算关闭网络连接的时候，如何得知对方是否发了一些数据而你还没有收到”的问题。
>
> 也就是说：muduo 把“主动关闭连接”这件事情分成两部分来做：如果要主动关闭连接，先关闭写端，等对方关闭之后，再关闭本地“读”端。

### muduo 什么时候真正 close socket 呢？

> 在 TcpConnectin 对象析构的时候，TcpConnectin 持有一个 Socket 对象，Socket 是一个RAII handler，它的析构函数会 close(sockfd_)。
>
> muduo 在 read() 返回 0 的时候回调 connection callback，TcpServer 或 TcpClient 把 TcpConnection 的引用计数减一。如果引用计数减到 0 ，则表示再也不持有 TcpConnection，它就析构了。

### muduo TCP 打包分包的解决方案

> 分包指的是发送一个消息或一帧数据时，通过一定的处理，让接收方能从字节流中识别并截取（还原）出一个个消息。
>
> 对于短链接的 TCP 服务，分包并不是一个问题。只要发送方主动关闭连接，则表示一条消息发送完毕，接收方 read() 接收到 0 ，从而知道消息的结尾。
>
> 对于长连接的 TCP 服务，分包有四种方法：
>
> - 消息长度固定；
> - 使用特殊的字符或字符串作为消息的边界，例如 HTTP 的协议的 headers 的 “\r\n” 作为字段的分隔符。
> - 在每条消息的头部加一个字段（这种做法比较常见）；
> - 利用消息本身的格式来分包：比如 XML 格式消息中的 <root>..</root>  或者 JSON 格式的 { } 格式数据。这种往往要利用到状态机。
>
> **muduo的打包分包基本原理：**
>
> - 当发送端发送一个结构体时，首先使用 protobuf 进行序列化，然后计算序列化数据的长度，然后使用 “长度+序列化数据” 的打包方式发送给接收端，接收端接收到数据后对数据进行分包，首先读取数据长度，然后根据长度读取序列化数据，并对数据进行反序列化得到完整的消息结构。
> - 当发送端只发送一个简单的字符串时，可以使用“字符串长度+字符串数据”这种简单的打包方式，打包后的数据存放到muduo::net::Buffer，然后发送出去。接收的数据同样存放在 muduo::net。
>
> **实现长度分包的代码：**
> muduo 实现长度分包的代码由类 LengthHeaderCodec 实现。代码位于https://github.com/chenshuo/muduo/blob/master/examples/asio/chat/codec.h
> LengthHeaderCodec 实现发送功能的代码
>
> ```c++
>   // FIXME: TcpConnectionPtr
>   void send(muduo::net::TcpConnection* conn,
>             const muduo::StringPiece& message)
>   {
>     muduo::net::Buffer buf;
>     buf.append(message.data(), message.size());
>     int32_t len = static_cast<int32_t>(message.size());
>     int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
>     buf.prepend(&be32, sizeof be32);
>     conn->send(&buf);
>   }
> ```
>
> 将 std::string 类型的 message 存放到 muduo::net::Buffer，获取 message 的长度，然后把长度由本地字节序转换成网络字节序，并放到 message 的前面。
>
> 实现接收部分的代码为：
>
> ```c++
> void onMessage(const muduo::net::TcpConnectionPtr& conn,
>                  muduo::net::Buffer* buf,
>                  muduo::Timestamp receiveTime)
>   {
>     while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
>     {
>       // FIXME: use Buffer::peekInt32()
>       const void* data = buf->peek();
>       int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS
>       const int32_t len = muduo::net::sockets::networkToHost32(be32);
>       if (len > 65536 || len < 0)
>       {
>         LOG_ERROR << "Invalid length " << len;
>         conn->shutdown();  // FIXME: disable reading
>         break;
>       }
>       else if (buf->readableBytes() >= len + kHeaderLen)
>       {
>         buf->retrieve(kHeaderLen);
>         muduo::string message(buf->peek(), len);
>         messageCallback_(conn, message, receiveTime);
>         buf->retrieve(len);
>       }
>       else
>       {
>         break;
>       }
>     }
>   }
> ```
>
> onMessage() 当收到的数据不足 4 个字节（用于存储 message 的长度)或者不足 4+ 消息长度个字节时，onMessage 直接返回。当收到的数据多于一条消息时，while 循环来反复读取数据，直到 Buffer 中的数据不够一条完整的消息。
>
> 当收到的数据恰好为一整条消息时，通过构造完整的 message，并通过 messageCallback_  回调用户代码。所以用户需要事先通过 LengthHeaderCodec 的构造函数把 messageCallback_ 传给它。
>
> ```c++
> class LengthHeaderCodec : boost::noncopyable
> {
>  public:
>   typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
>                                 const muduo::string& message,
>                                 muduo::Timestamp)> StringMessageCallback;
> 
>   explicit LengthHeaderCodec(const StringMessageCallback& cb)
>     : messageCallback_(cb)
>   {
>   }
>     //send()和onMessage()的代码同前    
>     private:
>     StringMessageCallback messageCallback_;
>     const static size_t kHeaderLen = sizeof(int32_t);
> }
> ```
>
> 把 StringMessageCallback 传给 LengthHeaderCodec 之后，当 LengthHeaderCodec 的 onMessage 收到数据后，就会把muduo::net::Buffer* 的数据转换成包含一条完整消息的 std::string&，这样让用户代码不必关心分包操作。

> Examples/asio/chat/codec.h 代码还没看懂

### muduo 如何实现多线程？

用  one loop per thread 的思想实现多线程 TcpServer 的关键步骤是在新建 TcpConnection 时候从 event loop pool 中挑选一个 loop 给 TcpConnection 使用，也就是说多线程 TcpServer 自己的 EventLoop 只用来接受新连接，而新连接会用其他的 EventLoop 来执行 IO 。

TcpServer 每次接受一个 TcpConnection 就会调用 getNetLoop() 来获取 EventLoop，如果是单线程服务，每次返回的都是 baseLoop_ （即 TcpServer 自己使用的 EventLoop）。

