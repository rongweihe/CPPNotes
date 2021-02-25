# 条款 09：不要再构造和析构函数里调用虚函数

为什么不要这样做？**因为这样调用会带来意想不到的结果。**

我们来看一个例子：

假设你要定义一个 class 来模拟股票的交易，并且需要再定义一个函数来记录每次交易的流水账。

下面是一个看起来比较合理的做法：

```c++
class Transaction { //交易基类
  public:
  	Transaction();
  	virtual void LogTransaction() const = 0;//记录日志
}
Transaction::Transaction() {
  LogTransaction();//基类的构造函数实现
   //...
}
class BuyTransaction: public Transaction {
  public:
  	virtual void LogTransaction() const = 0; //派生类的日志记录
  	//...
}
class ShellTransaction: public Transaction {
  public:
  	virtual void LogTransaction() const = 0; //派生类的日志记录
  	//...
}
```

那么，当执行下面这行代码的时候，会发生什么？

```c++
BuyTransaction b;
```

首先毫无疑问的派生类 BuyTransaction 的构造函数会调用，但最先调用的一定是派生类的构造函数么？在这里例子里， Transaction 基类的构造函数会第一个调用：**在 C++ 里，派生类对象内的基类成分会在派生类自身构造函数调用之前先调用。**

触发这种现象的正式上述代码的第四行。

这时候被调用的 LogTransaction 日志记录函数是基类里的版本而不是派生类的版本——即使目前即将建立的类型是派生类 BuyTransaction 。

那么，我们该如何确保每一次的基类继承体系上的对象被创建，就会有适当的版本的 LogTransaction 函数被调用呢？很显然，在 LogTransaction 构造函数内对对象调用虚函数是这一种错误的做法。

一种比较好的做法在于：在基类内将 LogTransaction 函数改为 non-virtual 函数，然后要求派生类的构造函数传递必要信息给基类构造函数，而后那个构造函数便可以安全地调用  non-virtual  的 LogTransaction 函数。

```c++
class Transaction {
  public:
  	explicit Transaction(const std::string& logInfo);
  	void LogTransaction(const std::string& logInfo) cosnt;//改为 non-virtual 函数
  	//...
}
Transaction::Transaction(const std::string& logInfo) {
  //LogTransaction(logInfo);
}
class BuyTransaction: public Transaction {
  public:
  	BuyTransaction(parameters)
      : Transaction(createLogString(parameters)) {...}//将Log信息传给基类构造函数
  //...
  private:
  	static std::string createLogString(parameters);
}
```

### 请记住

- **在 C++ 里，基类对象构造期间虚函数绝不会下降到派生类这一层。这样规定的背后的考虑是：由于基类的构造函数先于派生类执行，当基类构造函数执行时派生类的成员变量尚未初始化，如果此时调用的虚函数下降到派生类，那么派生类函数几乎会调用 local 的成员变量，而这些成员变量尚未初始化。也就是说基类对象内的派生类专属成员变量尚未初始化的时候，最安全的做法就是视而不见，因为在C++的哲学里：要求用到对象内尚未初始化的成员变量是危险的代名词。**
- **相同道理也适用于析构函数。一旦派生类析构函数开始执行，对象内的派生类成员变量便呈现出未定义值，所以 C++ 仿佛视他们不存在。**

