# SIG STL源码分析

## 前言

本专栏主要以 STL 源码剖析分析路线来分析 SIGSTL3.0 源码.

整个模块准备对学习 `STL源码剖析` 之后做一个系统的总结，这些都是我个人的理解，如果分析有什么问题欢迎各位大佬们指出。

感谢 《STL源码剖析》 作者侯捷大师，以及网络中各个大佬的总结。

在梳理知识的过程中，让我也能更容易更深刻的理解到 `STL` 的强大和方便，对泛型思维的理解深刻。

**总共分为六个版块 : 空间配置器，迭代器，容器(序列容器, 关联容器)，算法，仿函数，配接器。**

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/STL/STL-%E5%85%AD%E5%A4%A7%E7%BB%84%E4%BB%B6%E5%9B%BE.png)

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/STL/STL-container.png)

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/STL/stl-%E5%85%AD%E9%83%A8%E6%9B%B2-%E5%BA%8F%E5%88%97%E5%BC%8F%E5%AE%B9%E5%99%A8%E6%80%9D%E7%BB%B4%E5%AF%BC%E5%9B%BE.png)

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/STL/STL-%E8%BF%AD%E4%BB%A3%E5%99%A8.png)