//
// Created by hrw on 2021/03/05.
//
#include <sys/epoll.h>
#include <sys/types.h>  /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <sys/time.h>   /* timeval{} for select() */
#include <time.h>       /* timespec{} for pselect() */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */
#include <errno.h>
#include <fcntl.h> /* for nonblocking */
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <sys/uio.h>  /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h> /* for Unix domain sockets */

#include <sys/select.h> /* for convenience */
#include <sys/sysctl.h>
#include <poll.h>    /* for convenience */
#include <strings.h> /* for convenience */
#include <sys/ioctl.h>
#include <pthread.h>
#include <assert.h>
#include <linux/types.h>

#ifndef MAX_EVENT_NUMBER
#define MAX_EVENT_NUMBER 1024

// 普通的 BSD 标准 socket 结构体
// socket_state: socket 状态， 连接？不连接？
// type: socket type (%SOCK_STREAM, etc)
// flags: socket flags (%SOCK_NOSPACE, etc)
// ops: 专用协议的socket的操作
// file: 与socket 有关的指针列表
// sk: 负责协议相关结构体，这样就让这个这个结构体和协议分开。
// wq: 等待队列
struct socket {
	socket_state state;
	kmemcheck_bitfield_begin(type);
	short		type;
	kmemcheck_bitfield_end(type);
	unsigned long		flags;
	struct socket_wa __rcu	*wq;
	struct file			*file;
	struct sock 		*sk;
	const struct proto_ops	*ops;
};

//file:net/socket.c
SYSCALL_DEFINE3(socket, int, family, int, type, int, protocol)
{
//......
 retval = sock_create(family, type, protocol, &sock);
}

//sock_create 是创建 socket 的主要位置。其中 sock_create 又调用了 __sock_create。

//file:net/socket.c
int __sock_create(struct net *net, int family, int type, int protocol,
    struct socket **res, int kern)
{
 struct socket *sock;
 const struct net_proto_family *pf;

 //......

 //分配 socket 对象
 sock = sock_alloc();

 //获得每个协议族的操作表
 pf = rcu_dereference(net_families[family]);

 //调用每个协议族的创建函数， 对于 AF_INET 对应的是
 err = pf->create(net, sock, protocol, kern);
}






























