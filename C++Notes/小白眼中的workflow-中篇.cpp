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










