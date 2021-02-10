# 条款 12：复制对象时勿忘其每一个成分

这句话包含两部分的意思：第一部分是要考虑到所有成员变量，特别是后加入的，相应的拷贝构造函数和赋值运算符要及时更新；第二部分是在存在继承时，不要遗忘基类部分的复制。

先看第一部分的意思，举个例子：

```c++
class baseClass {
public:
	baseClass(const baseClass& s): a(s.a) {}
private:
	int a;
};
```

这里只举了一个拷贝构造函数的例子，赋值运算符与之类似，如果这个时候又加了一个成员变量比如 double b，拷贝构造函数和赋值运算符就要相应地更新（构造函数当然也要更新，只是构造函数一般不会被忘记，而拷贝构造函数和赋值运算符却常常被遗忘）。

像这样：

```c++
class baseClass {
public:
	baseClass(const baseClass& s): a(s.a), d(s.d) {}
private:
	int a;
  double b;
};
```

再看第二部分的意思，当存在继承关系时：

```c++
class Derived: public baseClass {
public:
  Derived(const Derived& d):c(d.c){}
private:
  int c;
};
```

像这样，很容易就会漏掉基类的部分，导致基类部分没有得到正常的拷贝，应该修改为如下：

```c++
class Derived: public baseClass {
public:
  Derived(const Derived& d): baseClass(d), c(d.c){}
private:
  int c;
};
```

最后总结一下：

- **` Copying`  函数应该确保复制“对象内的所有成员变量”及“所有基类成分”。**