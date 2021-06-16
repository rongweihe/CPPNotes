为了阻止某个对象被拷贝，有两种方案：

1、可将相应的成员函数声明为 private 并且不与实现。

2、使用像 Uncopyable这样的 base class 也是一种做法。

```c++
//方案一
class HomeSale {
  public:
  ...
  private:
  ...
    HomeSale(const HomeSale&);//只有声明
  	HomeSale& operator=(const HomeSale&);
}
```

```c++
//方案二
class Uncopyable {
  protected:
  	Uncopyable() {}
    ~Uncopyable() {}
  private:
  	Uncopyable(const Uncopyable&);//阻止copying
  	Uncopyable& operator=(cosnt Uncopyable&);
}
class HomeSale : private Uncopyable {
  ...
}
```

