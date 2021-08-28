// Copyright 2021 rongweihe.  All rights reserved.
// https://github.com/rongweihe/CPPNotes/tree/master/NetPrograming
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.


#include "muduo/net/EventLoopThreadPool.h"

#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
	: baseLoop_(baseLoop),
	name_(nameArg),
	started_(fasle),
	numThreads_(0),
	next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

// round-robin 算法选择下一个 loop
//轮询调度（Round Robin Scheduling）算法就是以轮询的方式依次将请求调度不同的服务器，
//即每次调度执行i = (i + 1) mod n，并选出第i台服务器。 算法的优点是其简洁性，它无需记录当前所有连接的状态，所以它是一种无状态调度。
EventLoop* EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop* loop = baseLoop_;

	if (!loops_.empty()) 
	{
		//rounf-robin
		loop = loops_[next_];
		++next_;
		if (implicit_cast<size_t>(next_) >= loops_.size())
		{
			next_ = 0;
		}
	}
	return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
  baseLoop_->assertInLoopThread();
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    loop = loops_[hashCode % loops_.size()];
  }
  return loop;
}

//返回所有的loops
std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  if (loops_.empty())
  {
    return std::vector<EventLoop*>(1, baseLoop_);
  }
  else
  {
    return loops_;
  }
}