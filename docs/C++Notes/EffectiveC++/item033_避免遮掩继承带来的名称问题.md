# 条款 33：避免遮掩继承而来的名称

Avoid hiding inherited names.

```
int x;
void someFunc() {
    double x;
    std::cin >> x;
}
```

当编译器处于 someFunc 的作用域并碰到名称 x 时，它会先在 local 作用域内进行查找，如果找不到就不再找其它作用域。所以 C++ 的名称遮掩规则所做的唯一事情是：遮掩名称。 

derived classes 继承了声明于 base classes 内的所有东西，实际运作方式是，derived class 作用域被嵌套在 base class 作用域内。不过名称遮掩会同时遮掩掉重载函数：看下面的例子：

```c++
class Base {
public:
    virtual void mf1() = 0;
    virtual void mf1(int);
    virtual void mf2();
    void mf3();
    void mf3(int);
};

class Derived: public Base {
public:
    virtual void mf1();
    void mf3();
    void mf4();
};

Derived d;

d.mf1();     // ok，调用 Derived::mf1()
d.mf1(x);    // fail，因为 Derived::mf1() 遮掩掉了 Base::mf1() 和 Base::mf1(int)
d.mf2();     // ok，调用 Base::mf2()
d.mf3();     // ok，调用 Derived::mf3()
d.mf3(x);    // fail，因为 Derived::mf3() 遮掩掉了 Base::mf3() 和 Base::mf3(int)
```

如果想要继承重载函数，可以使用 `using` ：

```c++
class Derived: public Base {
public:
    using Base::mf1;
    using Base::mf3;
    virtual void mf1();
    void mf3();
    void mf4();
};
```

同样地，有时候我们希望 Derived class 不继承全部的函数。这在 public 继承下是不允许的，因为它违反了 public 继承所暗示的 “is-a” 关系。但是在 private 继承下却可能有这样的需求，通常只要一个简单的转交函数（forwarding function）：

```c++
class Base {
public:
    virtual void mf1() = 0;
    virtual void mf1(int);
};

class Derived: private Base {
public:
    virtual void mf1() {
        Base::mf1();
    }
};
```

**最后总结一下：**

- 派生类的名称会遮掩基类的名称，在 public 继承下注意这个问题。
- 为了让被遮掩的名称重见天日，可以使用 using 声明式或转交函数。