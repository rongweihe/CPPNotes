# 条款 20：宁以 pass-by-reference-to-const 替换 pass-by-value

缺省情况下 C++ 以 by value 方式传递对象至函数，这些复制系由对象的 copy 构造函数产出，它可能使得 pass-by-value 成为费时的操作。更好的方式是用 pass-by-reference-to-const 来回避所有构造函数和析构函数的操作：

```c++
bool validateStudent(const Student& s);
```

这种传递方式效率高得多，**因为没有任何构造函数和析构函数被调用，所以没有额外创建对象的开销。**

by reference 方式传递参数也可以避免对象切割（slicing）问题。当一个 derived class 对象以 by value 方式传递并被视为一个 base class 对象时，bass class 的 copy 构造函数会被调用，而造成此 derived class 对应的特化性质完全被切割。而 references 底层往往以指针实现，因此 pass by reference 通常意味着真正传递的是指针。

当传递的对象是内置类型时，可能 pass by value 比 pass by reference 更高效，**因为它们的构造函数和析构函数代价可能比指针的要小。**

但不是说小型 types 都应该 pass-by-value，这里有三个主要原因：

- **对象小并不意味着其 copy 构造函数不费时；**
- **某些编译器对待内置类型和用户自定义类型的态度截然不同，例如某些编译器会拒绝把只由一个double 组成的对象放进缓存器，却可以将正规的 double 类型放入；**
- **作为一个用户定义类型，其大小容易发生变化，它可能目前虽然小，但是将来也许会变大，因为其内部实现可能发生变化；**
