// Copyright 2021. rongweihe All rights reserved.
// https://github.com/rongweihe/CPPNotes/tree/master/NetPrograming
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// This is a public header file, it must only include public header files.


#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <functional>

#include "muduo/net/Channel.h"
#include "muduo/net/Socket.h"

namespace muduo
{
namespace net
{

class EventLoop;
class InetAddress;
///
/// Acceptor of incoming TCP connections.
/// acceptor 负责 accept 新连接，并将新连接分发到 subReactor。
class Acceptor : noncopyable
{

public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    void listen();

    bool listening() const { return listening_; }

    // Deprecated, use the correct spelling one above.
    // Leave the wrong spelling here in case one needs to grep it for error messages.
    // bool listenning() const { return listening(); }

private:
    void handleRead(); 
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
};

}   //namespace net
}   //namespace muduo

#endif  // MUDUO_NET_ACCEPTOR_H