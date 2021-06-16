# 条款 23：宁以 non-member、non-friend 替换 member 函数

考虑这样一个类，它有一系列的 clear 函数：

```c++
class WebBrowser {
public:
    ...
    void clearCache();
    void clearHistory();
    void removeCookies();

    void clearEverything(); // 调用 clearCache，clearHistory，removeCookies；
    ...
};

void clearBrowser（WebBrowser& wb) {
    wb.clearCache();
    wb.clearHistory();
    wb.removeCookies();
}
```

相比于 `clearEverything` 函数，应该更倾向于使用 `clearBrowser` 的形式，因为越少的代码可以看到数据，则越多的数据可被封装；相反，愈多函数可以访问它，数据的封装性就愈低。所以当要在一个 member 函数和一个 non-member non-friend 函数之间做抉择，而且两者提供相同机能时，拥有较大封装性的是 non-member non-friend 函数。

在 C++ 中，比较自然的做法是让 `clearBrowser` 成为一个 non-member 函数并且位于 `WebBrowser` 所在的同一个 namespace 下：

```c++
namespace WebBrowser {
    class WebBrowser { ... }
    void clearBrowser(WebBrowser& wb);
}
```

最后来个总结：

- **这个论述只适用于 non-member non-friend 函数，friend 函数对 class private 成员的访问权力和 member 函数相同；**
- **只因在意封装性而让函数成员成为类的 non-member non-friend 并不意味它不可以是另一个类的成员函数；**