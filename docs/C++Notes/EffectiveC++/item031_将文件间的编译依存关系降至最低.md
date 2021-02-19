# 条款 31：将文件间的编译依存关系降至最低

Minimize compilation dependencies between files.

### 编译依存

编译依存（compilation dependency）指一个类的编译依赖于其他的头文件。例如：

```c++
#include <string>
#include "date.h"
#include "address.h"    // 需要引入其他头文件

class Person {
public:
    Person(const std::string& name, const Date& birthday, const Address& addr);
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;
    ...
private:
    std::string theName;
    Date theBirthDate;
    Address theAddress;    // 实现细目
};
```

### 前置声明的问题

C++ 之所以坚持将 class 的实现细目置于 class 的定义式中，主要考虑到两个问题：

- 对于标准头文件而言，它们不会成为编译瓶颈；
- 对于自定义类型，编译器需要在编译期间知道对象的大小；

### Handle classes

为了解决编译依存，可以使用 handle classes 的实现方式，”将对象实现细目隐藏于一个指针背后“：

```c++
class Person {
public:
    Person(const std::string& name, const Date& birthday, const Address& addr);
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;
    ...
private:
    std::tr1::shared_ptr<PersonImpl> pImpl;
};
```

这个分离的关键在于以”声明的依存性“替换”定义的依存性“。

设计策略：

- 如果使用 object references 或 object pointers 可以完成任务，就不要使用 objects；
- 如果能够，尽量以 class 声明式替换 class 定义式；

```c++
class Date; // class 声明式
void clearAppointments(Date d); // Date 定义式
```

pass-by-value 会引入”非必要的编译依存关系“，因为编译器需要知道类型的大小。

- 为声明式和定义式提供不同的头文件，只含声明式的头文件命名为”xxfwd.h"，命名方式取自 C++ 标准库头文件；

### Interface classes

实现一个特殊的抽象基类（abstract base class），它通常不带成员变量，也没有构造函数，只有一个 virtual 析构函数和一组 pure virtual 函数，用来叙述整个接口。

这个 class 的客户必须以 pointers 和 references 来撰写应用程序，而 Interface class 应该提供一种办法为这种 class 创建对象，通常是通过工厂函数或 virtual 构造函数来具现化对应的 derived class。

```c++
class Person {    // virtual base class
public:
    virtual ~Person();
    virtual std::string name() const = 0;
    virtual std::string birthDate() const = 0;
    virtual std::string address() const = 0;
    ...
};

class RealPerson: public Person {    // derived class
public:
    RealPerson(const std::string& name, const Date& birthday, const Address& addr);
    virtual ~Person();
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;
private:
    std::string theName;
    Date theBirthDay;
    Address theAddress;
};

// factory function
std::tr1::shared_ptr<Person> Person::create(const std::string& name,
                                            const Date& birthDate,
                                            const Address& addr) {
    return std::tr1::shared_ptr<Person>(new RealPerson(name, birthDate, addr));
}
```

### Handle classes & Interface classes

- handle class：成员函数必须通过 implementation pointer 取得对象数据；
  - 空间：需要消耗一个 implementation pointer 的大小，一个动态分配的 implementation object 大小；
  - 时间：动态申请和释放内存的时间开销，每次访问的指针取址跳转；
  - 安全：需要面对内存分配的 bad_alloc 异常；
- interface class：每个函数都是 virtual；
  - 空间：派生对象必须内含一个 vptr；
  - 时间：每次调用需要付出一次间接跳跃（indirect jump）

**最后总结一下：**

- 支持“编译依存性最小化”的一般构想是：相依于申明式，不要相依于定义式。基于此构想的两个手段是 Handle classes 和 Interface classes。

- 程序库头文件应该以“完全且仅有申明式”的形式存在。这种做法不论是否设计 template 都适用。