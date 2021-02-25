# 条款 15：资源管理类中提供对原始资源的访问

在第 14 条中，我们学习到了资源管理类，在资源管理类中，有许多 APIs 需要直接获取原始资源，例如：

```c++
std::tr1::shared_ptr<Investment> pInv(new Investment());
int deysHeld(const Investment* pi);
```

所以资源管理类需要提供对原始资源的访问接口。

引用计数型智能指针和智能指针都提供了一个  `get()` 成员函数，用来执行显示转换，它会返回智能指针内部的原始指针；并且也重载了指针取值（pointer dereferencing）和操作符（operator-> 和 operator*），它们允许转换至底部原始指针。

对于自定义的资源管理类，可以选择显示转换函数和隐式转换函数两种方式。

```c++
class Font {
public:
  //...
  FontHandle get() const { return f; }//显示转换;
  operator FontHandle() const {	return f; }//隐式转换;
private:
  FontHandle f;
}
//隐式转换会增加错误发生的概率
Font f1(new FontHandle());
FontHandle f2 = f1;// 原意是要拷贝一个 Font 对象；却反而将 f1 隐式转换其底部的 FontHandle 对象才复制它。
```

最后总结一下：

- **一般而言显式转换比较安全，但隐式对客户比较方便。**
- **一些 APIs 往往要求访问原始资源（raw resources ），所以每一个 RAII class 都应该提供一个”取得其所管理之资源“的办法。**

