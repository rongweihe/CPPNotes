#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#ifdef __linux__
# include <sys/epoll.h>
# include <sys/timerfd.h>
#else
# include <sys/event.h>
# undef LIST_HEAD
# undef SLIST_HEAD
#endif
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "list.h"
#include "rbtree.h"
#include "poller.h"

#define POLLER_BUFSIZE			(256 * 1024)
#define POLLER_NODES_MAX		65536
#define POLLER_EVENTS_MAX		256
#define POLLER_NODE_ERROR		((struct __poller_node *)-1)

/*
poll 节点
*/
struct __poller_node
{
	int state;
	int error;
	struct poller_data data;
#pragma pack(1)
	union
	{
		struct list_head list;
		struct rb_node rb;
	};
#pragma pack()
	char in_rbtree;
	char removed;
	int event;
	struct timespec timeout;
	struct __poller_node *res;
};

/*
poller 数据结构
*/
struct __poller
{
	size_t max_open_files;
	poller_message_t *(*create_message)(void *);
	int (*partial_written)(size_t, void*);
	void (*cb)(struct poller_result*, void *);
	void *ctx;

	pthread_t tid;
	int pfd;
	int timerfd;
	int pipe_rd;
	int pipe_wr;
	int stopped;
	struct rb_root timeo_tree;
	struct rb_node *tree_first;
	struct list_head timeo_list;
	struct list_head no_timeo_list;
	struct __poller_node **nodes;
	pthread_mutex_t mutex;
	char buf[POLLER_BUFSIZE];
};
#ifdef __linux__

//封装 epoll_create 创建文件描述符 指向内核创建的描述符集 之后的操作都通过描述符来操作
static inline int __poller_create_pfd() {
	return epoll_create(1);
}

//添加文件描述符
static inline int __poller_add_fd(int fd, int event, void *data,
								poller_t *poller) 
{
	struct epoll_event ev = {
		.events 	= event;
		.data   	= {
			.ptr	= data
		}
	};
	return epoll_ctl(poller->pfd, EPOLL_CTL_ADD, fd, &ev);
}

static inline int __poller_del_fd(int fd, int event, poller_t *poller)
{
	return epoll_ctl(poller->pfd, EPOLL_CTL_DEL, fd, NULL);
}

static inline int __poller_mod_fd(int fd, int old_event,
								  int new_event, void *data,
								  poller_t *poller)
{
	struct epoll_event ev = {
		.events		=	new_event,
		.data		=	{
			.ptr	=	data
		}
	};
	return epoll_ctl(poller->pfd, EPOLL_CTL_MOD, fd, &ev);
}







