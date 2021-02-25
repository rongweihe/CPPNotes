# 条款18：让接口容易被正确使用，不易被误用

### 引入新类型

如果想要开发一个不同意误用的接口，首先需要考虑客户可能会犯什么错误

例如一个用来表示时间的类：

```c++
class Date{
public:
  Date(int month, int dat, int year);
  //...
};
Date d(30, 3, 1995);//错误的参数顺序
Date d(2,30, 1995);//无效的月份或天数
```

许多客户端的错误可以通过引入新类型来进行预防：

```c++
struct Day {
    explicit Day(int d): val(d) {}
    int val;
};

struct Month {
    explicit Month(int m): val(m) {}
    int val;
};

struct Year {
    explicit Year(int y): val(y) {}
    int val;
};

class Date {
public:
    Date(const Month& m, const Day& d, const Year& y);
    ...
};
Date d(30, 3, 1995);                   // 错误，不正确的类型；
Date d(Day(30), Month(3), Year(1995)); // 错误，不正确的顺序；
Date d(Month(3), Day(30), Year(1995)); // 正确；
```

一旦引入了新的类型，就可以对其限制取值范围。比如一年只有12个有效月份，所以 `Month` 的取值不能小于 1 大于 12。一种方式是用过 enum 表现月份，但 enums 不具备类型安全性，因为它可以作为 ints 使用。更好的办法是预先定义有效的月份：

```c++
class Month {
public:
    static Month Jan() { return Month(1); }
    ...
    static Month Dec() { return Month(12); }
private:
    explicit Month(int m);
};
```

之所以使用函数替换对象，是为了防止 `non-local static` 对象的初始化次序问题（[条款4]()）。

### 替客户做那些必要的事情

任何接口如果要求客户必须记得做某些事情，就是有着“不正确使用”的倾向，因为客户可能忘记做那件事情。

例如一个 `factory`  函数：

```c++
Investment* createInvestment();
```

客户在使用该接口时可能会犯两种错误，一是没有删除指针，二是删除了超过一次。所以这个接口依赖客户将得到的指针放入智能指针中，而更好的做法是令 `factory` 函数直接返回一个智能指针：

```c++
std::tr1::shared_ptr<Investment> createInvestment();

// 定义智能指针的释放方式，getRidOfInvestment()
std::tr1::shared_ptr<Investment> createInvestment() {
    std::tr1::shared_ptr<Investment> retVal(static_cast<Investment*>(0),
                                            getRidOfInvestment);
    retVal = ... // 令智能指正指向正确的对象；
    return retVal;
}
```

智能指针有一个特别好的特性：它会自动使用它的“每个指针专属的删除器“，因此消除了另一个潜在客户错误”cross-DLL problem“。

> cross-DDL problem
>
> 这个问题发生在对象在动态链接程序库（DLL）中被 new 创建，却在另一个 DLL 内被 delete 销毁。在许多平台上，跨 DLL 的 new/delete 调用会导致运行期错误。

智能指针也存在性能缺陷，因为它需要额外的辅助空间作为簿记用途和删除器的专属数据。

最后做一个总结：

- **好的接口很容易被正确使用，不容易被误用，应该在所有的接口中努力达成这样的性质。**
- **”促进正确使用“的办法包括接口的一致性，以及与内置类型的行为兼容。**
- **”阻止误用“的办法包括建立新类型，限制类型上的操作，束缚对象值，以及消除客户的资源管理责任。**
- **`std::tr1::shared_ptr` 支持定型删除器，可以防范 DDL 问题。**

