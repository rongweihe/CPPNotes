# 条款 07：为多态基类声明 virtual 析构函数（declare_destructos_virtual_in_poly_base_class）

看下面的一段代码：

```c++
class TimeKeeper {
public:
  TimeKeeper();
  ~TimeKeeper();
  ...
};

class AtomicClock: public TimeKeeper { ... };	//原子钟
class WaterClock: public TimeKeeper { ... };	//水钟
class WristWatch: public TimeKeeper { ... };	//腕表

//@ 使用时
TimeKeeper *ptk = getTimeKeeper();  
...                         
delete ptk;            
```

getTimeKeeper() 是一个 factory 函数，返回指针指向一个 TimeKeeper 派生类的动态分配对象，返回的对象位于 heap，因此需要对其返回的对象适当地 delete 掉。

问题在于：

C++ 明确指出：**当派生类对象经由一个基类指针删除，而该基类指针带有 non-virtual 析构函数，则结果未定义——实际执行时通常发生的是对象的 derived 部分没被销毁，而 base 类部分通常会被销毁，造成一个诡异的“局部销毁”对象，可能造成资源泄漏、败坏数据结构、徒增调试时间。**

消除这个问题的做法是：给 base 类一个 virtual 析构函数。这样通过基类指针销毁派生类将会符合期望：

```c++
class TimeKeeper {
public:
  TimeKeeper();
  virtual ~TimeKeeper();
  ...
};

TimeKeeper *ptk = getTimeKeeper();
...
delete ptk;  
```

类似 TimeKeeper 这样的基类一般都包含除了析构函数以外的其它 virtual 函数，因为 virtual 函数的目的就是允许派生类的实现得以定制化。例如，TimeKeeper 可以有一个虚函数 getCurrentTime，它在各种不同的派生类中有不同的实现。几乎所有拥有虚函数的类差不多都应该有一个虚析构函数。

**如果一个类不包含虚函数，通常表示它并不意图被用作一个基类。当一个类不打算作为基类时，令其析构函数为 virtual 通常是个馊主意。**

因为虚函数的实现要求对象携带额外的信息，这些信息用于在运行时确定该对象应该调用哪一个虚函数。这份信息通常是由 vptr(virtual table pointer) 指针指出。 vptr 指向一个由函数指针构成的数组，称为 vtbl(virtual table)。每个带有 virtual 函数的类，都有一个相应的 vtbl，当对象调用某一个 virtual 函数的时候，实际调用的函数取决于该对象的 vptr 所指向的那个 vtbl-编译器在其中寻找适当的函数指针。

**析构函数的运作方式是：最深层派生的那个 class 其析构函数最先被调用，然后是其每一个 base class 的析构函数被调用。**

