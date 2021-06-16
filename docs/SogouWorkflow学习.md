# 1、Sogou C++ Workflow 是什么？

开胃小菜 大菜 麻辣香锅 ，满汉全席。满汉全席 ？佛跳墙？八仙过海？

workflow 是搜狗公司近期开源发布的一款 C++ 服务器引擎，支撑搜狗几乎所有后端 C++ 在线服务，包括所有搜索服务，云输入法，在线广告等，每日处理超百亿请求。

是一个设计轻盈优雅的企业级程序引擎，可以满足大多数 C++ 后端开发需求。

#### 你可以用来：

- 快速搭建 http 服务器：

```c++
#include <stdio.h>
#include "workflow/WFHttpServer.h"

int main()
{
    WFHttpServer server([](WFHttpTask *task) {
        task->get_resp()->append_output_body("<html>Hello World!</html>");
    });

    if (server.start(8888) == 0) {  // start server on port 8888
        getchar(); // press "Enter" to end.
        server.stop();
    }

    return 0;
}
```

- 作为万能异步客户端。目前支持 http，redis，mysql

  和kafka协议。

  - 轻松构建效率极高的spider。

- 实现自定义协议client/server，构建自己的RPC系统。

  - [srpc](https://github.com/sogou/srpc)就是以它为基础，作为独立项目开源。支持`srpc`，`brpc`和`thrift`等协议。

- 构建异步任务流，支持常用的串并联，也支持更加复杂的 DAG 结构。

- 作为并行计算工具使用。除了网络任务，我们也包含计算任务的调度。所有类型的任务都可以放入同一个流中。

- 在`Linux`系统下作为文件异步 IO 工具使用，磁盘IO也是一种任务。

- 实现任何计算与通讯关系非常复杂的高性能高并发的后端服务。

- 构建微服务系统。

  - 项目内置服务治理与负载均衡等功能。

#### 编译和运行环境

- 项目支持 Linux，macOS，Windows

  等操作系统。

  - `Windows`版以[windows](https://github.com/sogou/workflow/tree/windows)分支发布，使用`iocp`实现异步网络。用户接口与`Linux`版一致。

- 支持所有CPU平台，包括32或64位`x86`处理器，大端或小端`arm`处理器。

- 需要依赖于OpenSSL，推荐OpenSSL 1.1及以上版本。

  - 不喜欢SSL的用户可以使用[nossl](https://github.com/sogou/workflow/tree/nossl)分支，代码更简洁。但仍需链接`crypto`。

- 项目使用了`C++11`标准，需要用支持`C++11`的编译器编译。但不依赖`boost`或`asio`。

- 项目无其它依赖。如需使用`kafka`协议，需自行安装`lz4`，`zstd`和`snappy`几个压缩库。

**项目官方地址：**https://github.com/sogou/workflow

# 2、简要介绍

一句话介绍：基于 C++11 std::function 的异步引擎。用于解决一切关于串行，并行和异步的问题。

以自己目前接触到的功能来说，可以当做一个 MySQL 客户端，部署和使用比较方便。

下面以个人实际使用过的场景为例，带大家来看一下这个框架在 Linux 环境下的一些用法。

### 2.1 部署安装

命令安装 workflow-dev  截止本文档提交的最新版本是 1.13.1 。具体使用时，可 yum list | grep workflow 获取最新版本：

```sh
yum install workflow-devel-1.13.1
```

### 2.2 查看 workflow 库文件安装目录

一般在 /usr/include/workflow/ 目录下 。

```sh
cd /usr/include/workflow/ 
```

### 2.3 include 库文件

在要运行源文件（即启动程序）的目录，为了使用 workflow 的库文件，可以拷贝 /usr/include/workflow/ 到本目录或者直接在源文件里 include /usr/include/目录。

### 2.4 检查环境是否已经安装好 make，cmake

其中 cmake 大于等于 3.5 版本，低版本编译会报错不支持。是否支持 OpenSSL 。

同时创建 CMakeLists.txt，CMake 主要是编写 CMakeLists.txt 文件， 然后用 cmake 命令将 CMakeLists.txt 文件转化为 make 所需要的 makefile 文件，最后用 make 命令编译源码生成可执行程序或共享库，由于编译中出现中间的文件，因此最好新建一个独立的目录 build，在该目录下进行编译生成的 GNUmakefile。

具体配置可以参考：[使用workflow框架开发服务的一些经验](https://git.sogou-inc.com/herongwei/share/blob/master/workflow/%E4%BD%BF%E7%94%A8workflow%E6%A1%86%E6%9E%B6%E5%BC%80%E5%8F%91%E6%9C%8D%E5%8A%A1%E7%9A%84%E4%B8%80%E4%BA%9B%E7%BB%8F%E9%AA%8C.md)

### 2.5 启动程序

以上步骤配置没问题之后，在当前目录执行 make 命令，编译无误即可通过执行 ./xxx 来运行相应程序 。

### 3、个人理解

建议：建议各位朋友去官网上看过一遍具体介绍和demo之后再回来看我这篇唠叨，下面的所述的个人理解都是基于官网的一些知识做的一些补充。

#### 3.1 首先，我们看到这个架构图：

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/%E6%9E%B6%E6%9E%84%E5%9B%BE.jpg)

1、不管是MySQL task，还是 Redis_task，看源码都是来自于模板类 WFNetworkTask。我们接着往下看：

一次最简单的 HTTP 任务抓取数据之后在服务端返回，在这个过程中，我们通过庖丁解牛般来看一看这个框架的底层源码是何方神迹。

```c++
/**********Client Factory**********/

WFHttpTask *WFTaskFactory::create_http_task(const std::string& url,
											int redirect_max,
											int retry_max,
											http_callback_t callback)
{
	auto *task = new ComplexHttpTask(redirect_max,
									 retry_max,
									 std::move(callback));
	ParsedURI uri;

	URIParser::parse(url, uri);
	task->init(std::move(uri));
	task->set_keep_alive(HTTP_KEEPALIVE_DEFAULT);
	return task;
}

WFHttpTask *WFTaskFactory::create_http_task(const ParsedURI& uri,
											int redirect_max,
											int retry_max,
											http_callback_t callback)
{
	auto *task = new ComplexHttpTask(redirect_max,
									 retry_max,
									 std::move(callback));

	task->init(uri);
	task->set_keep_alive(HTTP_KEEPALIVE_DEFAULT);
	return task;
}
```

create_http_task 继承自基类 ComplexHttpTask 来构造了一个 task。而后这个 task 又继承自 WFComplexClientTask。

```c++
class ComplexHttpTask : public WFComplexClientTask<HttpRequest, HttpResponse>
{
public:
	ComplexHttpTask(int redirect_max,
					int retry_max,
					http_callback_t&& callback):
		WFComplexClientTask(retry_max, std::move(callback)),
		redirect_max_(redirect_max),
		redirect_count_(0)
	{
		HttpRequest *client_req = this->get_req();

		client_req->set_method(HttpMethodGet);
		client_req->set_http_version("HTTP/1.1");
	}
```

并且根据不同的参数重载了不同的版本。

WFNetworkTask public  继承 CommRequest。

> WFTaskFactory::create_http_task()产生一个http任务，前几个参数不用过多解释，http_callback_t 是 http 任务的 callback（回调函数）。
>
> task->get_req() 函数得到任务的request，默认是GET方法，HTTP/1.1，长连接。框架会自动加上request_uri，Host等。  
>
> task->start()启动任务，非阻塞，并且不会失败。之后callback必然会在被调用。因为异步的原因，start()以后显然不能再用task指针了。 

启动 create_redis_task 启动一个Redis任务和 create_http_task 启动 http 任务是类似的。

与http task的get_req()类似，redis task的get_req()返回任务对应的redis request。 其中，set_request接口用于设置redis命令。 

> callback需要特别解释的，是series_of(task)->push_back(next)这个语句。因为这是我们第一次使用到Workflow的功能。  
>
> 在这里next是我们下一个要发起的redis task，执行GET操作。我们并不是执行next->start()来启动任务，而是把next任务push_back到当前任务序列的末尾。  
>
> 这两种方法的区别在于：
>
>   \* 用start来启动任务，任务是被立刻启动的，而push_back的方法，next任务是在callback结束之后被启动。
>
> ​    \* 最起码的好处是，push_back方法可以保证log打印不会乱。否则，用next->start()的话，示例中"Finished."这个log可能会被先打印。
>
>   \* 用start来启动下一个任务的话，当前任务序列（series）就结束了，next任务会新启动一个新的series。
>
> ​    \* series是可以设置callback的，虽然在示例中没有用到。
>
> ​    \* 在并行任务里，series是并行任务的一个分枝，series结束就会认为分枝结束。并行相关内容在后续教程中讲解。
>
> 
>
> 总之，如果你想在一个任务之后启动下一个任务，一般是使用push_back操作来完成（还有些情况可能要用到push_front）。  
>
> 而series_of()则是一个非常重要的调用，是一个不属于任何类的全局函数。其定义和实现在[Workflow.h](../src/factory/Workflow.h#L140)里：
>
> 总之，如果你想在一个任务之后启动下一个任务，一般是使用push_back操作来完成（还有些情况可能要用到push_front）。
> 而series_of()则是一个非常重要的调用，是一个不属于任何类的全局函数。其定义和实现在[Workflow.h](https://github.com/sogou/workflow/blob/master/src/factory/Workflow.h#L140)里：
>
> ```
> static inline SeriesWork *series_of(const SubTask *task)
> {
>     return (SeriesWork *)task->get_pointer();
> }
> ```
>
> 任何task都是SubTask类型的派生。而任何运行中的task，一定属于某个series。通过series_of调用，得到了任务所在的series。
> 而push_back是SeriesWork类的一个调用，其功能是将一个task放到series的末尾。类似调用还有push_front。本示例中，用哪个调用并没有区别。
>
> ```
> class SeriesWork
> {
>     ...
> public:
>     void push_back(SubTask *task);
>     void push_front(SubTask *task);
>     ...
> }
> ```
>
> SeriesWork类在我们整个体系中，扮演重要的角色。在下一个示例中，我们将展现SeriesWork更多的功能。

```c++
void SeriesWork::push_back(SubTask *task)
{
	this->mutex.lock();
	task->set_pointer(this);
	this->queue[this->back] = task;//为什么不需要判断队列是否满再插入而是直接插入
	if (++this->back == this->queue_size)
		this->back = 0;

	if (this->front == this->back)
		this->expand_queue();//为什么这句不是放在插入语句之前呢

	this->mutex.unlock();
}
```

```c++
static inline SeriesWork *series_of(const SubTask *task)
{
    return (SeriesWork *)task->get_pointer();//返回的是 SeriesWork 的指针
}
```

```c++
series_of(task)->push_back(next);
```

这句话的作用：任何 task 都是 SubTask 类型的派生。而任何运行中的 task，一定属于某个 series。通过series_of 调用，得到了任务所在的 series。

#### 3.2 Http 任务并设置参数

workflow 的 http_task 还提供多种参数调用，比如支持限制 http 抓取返回内容的大小，以及接收回复的最大时间。

```c++
WFHttpTask *http_task = WFTaskFactory::create_http_task(...);
...
http_task->get_resp()->set_size_limit(20 * 1024 * 1024);
http_task->set_receive_timeout(30 * 1000);
```

set_size_limit()是HttpMessage的调用，用于限制接收http消息时包的大小。事实上所有的协议消息都要求提供这个接口。
set_receive_timeout()是接收数据的超时，单位为ms。
上述代码限制http消息不超过20M，完整接收时间不超过30秒。更多更丰富的超时配置，参考官方文档。

#### 3.3 内存回收机制

\* 任何任务都会在callback之后被自动内存回收。如果创建的任务不想运行，则需要通过dismiss方法释放。

\* 任务中的数据，例如网络请求的resp，也会随着任务被回收。此时用户可通过``std::move()``把需要的数据移走。

\* SeriesWork和ParallelWork是两种框架对象，同样在callback之后被回收。

  \* 如果某个series是parallel的一个分支，则将在其所在parallel的callback之后再回收。

\* 项目中不使用``std::shared_ptr``来管理内存。

#### 3.5 异步 server 的原理

在官方的 echo server 的示例里，没有进行网络通信，而是直接填写返回页面，而在 http_proxy 这个例子里介绍了异步 server 的思想：HTTP proxy 需要等待 web server 的结果。

 也就是在异步得到请求结果之后，再去回复用户请求，在等待结果期间，不能占用任何的线程。当然你也可以占用这个 process 函数的线程，等待结果返回，但这种等待的方式就是同步等待，效率比较低。

因此，可以在 server 的构造函数里面，处理函数里面

```c++
WFHttpServer server(&params, process);
```

在 process 的头部，我们给当前 series 设置了一个context，context里包含了proxy_task本身，以便我们异步填写结果。

poller.c 源码是封装了 epoll 函数，从而实现了多路复用 IO 的优势。

#### 3.6 并行任务

之前的示例里，我们已经接触过了SeriesWork类。

- SeriesWork由任务构成，代表一系列任务的串行执行。所有任务结束，则这个series结束。
- 与SeriesWork对应的ParallelWork类，parallel由series构成，代表若干个series的并行执行。所有series结束，则这个parallel结束。
- ParallelWork是一种任务。

总之，这个框架的串+并行任务流，让我一下子就回想起来了初中学物理，电路设计的那些知识，感觉很有趣，初中学过的知识居然能用到这个网络框架当中，这让我一方面觉得熟悉，一方面又觉得很新奇。

workflow 给开发者用户接触到的是task（任务）和series（任务流）。这个高度的抽象使得开发可以聚集所有精力完成应用逻辑的开发，完全不需要考虑线程等底层概念。

根据上述的定义，我们就可以动态或静态的生成任意复杂的工作流了。
Workflow类里，有两个接口用于产生并行任务：

```c++
class Workflow
{
    ...
public:
    static ParallelWork *
    create_parallel_work(parallel_callback_t callback);

    static ParallelWork *
    create_parallel_work(SeriesWork *const all_series[], size_t n,
                         parallel_callback_t callback);

    ...
};
```

第一个接口创建一个空的并行任务，第二个接口用一个series数组创建并行任务。
无论用哪个接口产生的并行任务，在启动之前都可以用ParallelWork的add_series()接口添加series。

因为 http 任务在 callback 之后就会被回收，如果需要在 create_http_task 之后保留上下文，resp 需要通过std::move()操作移走。

#### 3.7 内部算法工厂类

系统的算法工厂提供了一些常用的计算任务，比如排序，归并等。用户也可以很方便定义自己的计算任务。

创建升序排序任务

```c++
int main(int argc, char *argv[])
{
    ...
    WFSortTask<int> *task;
    if (use_parallel_sort)
        task = WFAlgoTaskFactory::create_psort_task("sort", array, end, callback);
    else
        task = WFAlgoTaskFactory::create_sort_task("sort", array, end, callback);
    ...
    task->start();
    ...
}
```

和WFHttpTask或WFRedisTask不同，排序任务多了一个模板参数代表要排序的数组数据类型。
create_sort_task和create_psort_task分别产生一个普通排序任务和一个并行排序任务。
这两个调用的参数和返回值并没有区别。
唯一需要特别说明的是第一个参数"sort"，这个是计算队列名，用于影响内部的任务调度。
计算任务的启动方法与使用方法和网络通信任务并没有什么区别。

# 处理结果

和通信任务一样，我们在callback里处理结果。

```c++
using namespace algorithm;

void callback(void SortTask<int> *task)
{
    SortInput<T> *input = task->get_input();
    int *first = input->first;
    int *last = input->last;

    // print result
    ...
    
    if (task->user_data == NULL)
    {
        auto cmp = [](int a1, int a2){ return a2 < a1; };
        WFSortTask<int> *reverse;

        if (use_parallel_sort)
            reverse = WFAlgoTaskFactory::create_psort_task("sort", first, last, cmp, callback);
        else
            reverse = WFAlgoTaskFactory::create_psort_task("sort", first, last, cmp, callback);
            
        reverse->user_data = (void *)1; /* as a flag */
        series_of(task)->push_back(reverse);
    }
    else
    {
        // all done. Signal main thread to exit.
        ... 
    }
}
```

计算任务的get_input()接口得到输入数据，get_output()得到输出数据。对于排序任务，输入和输出是相同类型，内容也完全相同。
在[WFAlgoTaskFactory.h](https://github.com/sogou/workflow/blob/master/src/factory/WFAlgoTaskFactory.h)里，可以看到排序任务输入输出的定义：

```c++
namespace algorithm
{
template <typename T>
struct SortInput
{
    T *first;
    T *last;
};

template <typename T>
using SortOutput = SortInput<T>;

}

template <typename T>
using WFSortTask = WFThreadTask<algorithm::SortInput<T>,
                                algorithm::SortOutput<T>>;

template <typename T>
using sort_callback_t = std::function<void (WFSortTask<T> *)>;
```

显然，input或output里的first, last分别为排序数组的首尾指针。值得注意的是这里的 first 和 last 是指针，而不是用iterator。
同样，用create_psort_task()可以创建一个并行排序任务。而对series的使用，和通信任务没有区别。

#### 3.7 MySQL 客户端

当然，我目前使用 workflow 最多的功能还是它的MySQL客户端，自己负责一个的后端服务需要用到MySQL客户端来处理数据库的数据。

目前支持的命令为**COM_QUERY**，已经能涵盖用户基本的增删改查、建库删库、建表删表、预处理、使用存储过程和使用事务的需求。基本满足了我现在的全部需求。

并且一些高级的用法比如事务，我还并没有用到。

因为我们的交互命令中不支持选库（**USE**命令），所以，如果SQL语句中有涉及到**跨库**的操作，则可以通过**db_name.table_name**的方式指定具体哪个库的哪张表。

其他所有命令都可以**拼接**到一起通过 `set_query()` 传给WFMySQLTask（包括INSERT/UPDATE/SELECT/PREPARE/CALL）。

举个例子：

```c++
req->set_query("SELECT * FROM table1; CALL procedure1(); INSERT INTO table3 (id) VALUES (1);");
```

总之，对于我这个刚工作一年，第一次接触并使用这种开源级别框架的小白而言，两个字就能完全表达我的感受：好用，很棒。

那么，除了好用之外，我还有一些自己的看法。

在 结果解析 这个地方，是不是可以选择再封装一下更好，不然在业务里面，如果用到了 MySQL 的回调函数，不同的业务都要每次判断一下，写一大段解析异常的结果，比如冗余。

#### 3.8 还支持简单的用户自定义协议 client/server

那么这个基于 HTTP 服务器的性能咋样呢？我把官网里的对比实验结果搬出来。

- 测试环境：CPU 40 core @ 2.20GHz，内存192G，网卡25000Mb/s

- 线程数

- - workflow：将使用 16 个 poller 线程和 20 个 handler 线程
  - nginx：40 个 Worker 进程
  - brpc：40 个 线程

- 数据长度：分别设置为 64、512 和 4096

- 并发度：在[1, 2K]之间翻倍增长

我们看下实际的实验结果：

![图片](https://mmbiz.qpic.cn/mmbiz_png/BBjAFF4hcwpZHnVHgRkc1aMxy0mV6y1wtiaR87D1AqFPcH0vqj4spt7TmTtEEPLuCLLeMqARbQnFuVHevu0WVPw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

特别是数据长度为64和512的两条曲线，并发度足够的时候，可以保持500K的QPS。