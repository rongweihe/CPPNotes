# 条款 28：避免返回 handles 指向对象内部成分

### 返回 handles 的问题

让成员函数返回 handles 指向对象内部成分是比较糟糕的做法：

```c++
class Rectangle {
public:
    ...
    Point& upperLeft() const { return pData->ulhc; }
    Point& lowerRight() const { return pData->lrhc; }
    ...
};
```

这样做有两个不好的影响：

- 首先，成员变量的封装性最多只等于 “返回其 reference” 的函数的访问级别。
- 其次，如果 const 成员函数传出一个 reference 那么这个函数的调用者就可以通过这个 reference 修改那笔数据。

因为，在 C++ 函数里，前面和后面使用 const 的作用：

- 前面使用 const 表示返回值为 const。

- 后面加 const 表示函数不可以修改 class 的成员

References、指针和迭代器统统都是 handles，而返回一个“代表对象内部数据”的 handle 会带来“较低对象封装性“的风险。

### 解决返回 handles 函数的影响

如果一定要让函数返回 handles，那么一定要用 const 关键字修饰：

```c++
class Rectangle {
public:
    ...
    const Point& upperLeft() const { return pData->ulhc; }
    const Point& lowerRight() const { return pData->lrhc; }
    ...
};
```

**最后总结一下：**

- 避免返回 handles（包括 references、指针、迭代器）指向对象内部，准守这个条款可增加封装性。帮助 const 成员函数的行为像个 const。