# 条款 34：区分接口继承和实现继承

Differentiate between inheritance of interface and inheritance of implementation

Public 继承由两部分组成：函数接口继承（function interface）和函数实现继承（function implementations）继承，而不同类型的函数继承程度也是不一样的。

### 成员函数的接口总是会被继承

声明一个纯虚函数的目的是为了让派生类只继承其函数接口。（基类无法为此函数提供合理的缺省实现，也就是说继承基类的派生类必须提供同名函数但不干涉你怎么实现它）。

### 声明非纯虚函数的目的是为了让派生类继承该函数的接口和缺省实现

考虑下面这个例子：

```c++
class Shape {
	public:
  	virtual void error(const std::string& msg);
}
```

考虑 Shape::error 这个例子，error 接口表示，每个 class 都必须支持一个遇上错误时可调用的函数，但每个 class 可以自由处理错误。如果某个 class 不想针对错误做出特殊行为，可以退回到 Shape class 提供的缺省错误处理行为。也就是说Shape::error 的声明式告诉 derived class 设计者：你必须支持一个 error 函数，但如果你不想自己写，可以使用Shape class 提供的缺省版本。

### 声明 non-virtual 函数的目的是为了让 derived classes 继承函数的接口及一份强制性的实现

最后来看看 Shape 的 non-virtual 函数 objectID；`Shape::objectID`是个 non-virtual 函数，这意味着它不打算在 derived class 中有不同行为。

可以把 Shape::objectID 看做“每个 Shape 对象都有一个用来产生识别码的函数，这个识别码采用相同计算方法。non-virtual函数代表的意义是不变性（invariant）凌驾特异性（specialization），所以不应该在derived classes中被重新定义，这个**条款**36讨论的重点。

最后总结一下：

- 接口继承和实现继承不同。在 public 继承下，derived classes 总是继承 base classes 的接口。
- pure virtual 函数只具体指定接口继承。
- impure virtual 函数具体指定接口继承和缺省实现继承。
- non-virtual 函数具体指定接口继承和强制性实现继承。