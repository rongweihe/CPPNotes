# 条款 25：考虑写出一个不抛异常的 swap 函数

### 常规 `swap` 函数

所谓置换两个对象的值，就是将两个对象的值彼此交换，在默认情况下，`swap`  动作可由标准库提供的算法完成：

```c++
namespace std {
	template<typename T>
	void swap(T& a, T& b) {
		T temp(a);
		a = b;
		b = temp;
	}
}
```

只要 T 类型支持 `copying`（通过 `copy` 构造函数和 `copy assignment` 操作符完成），就可以使用标准库提供的 swap 算法。但是对某些类型而言，复制操作的代价很大，例如“以指针指向一个对象，指针指向真正的数据”类型。

### class template 偏特化

```c++
class WidgetImpl {
private:
    int a, b, c;
    std::vector<double> v;    // 可能要复制很长时间；
};

class Widget {
public:
    Widget(const Widget& rhs) {
        ...
        *pImpl = *(rhs.pImpl);
    }
    ...
private:
    WidgetImpl* pImpl;
};
```

对于这种类型的交换操作，只需要交换 `pImpl` 的值即可，无需将整个对象拷贝。而实践这一个思路的唯一做法是：将 `std::swap`  针对 `Widget` 类进行特化。

```c++
class Widget {
	public:
  	...
    void swap(Widget& rhs) {
      std::swap(pImpl, rhs.pImpl);// 这里使用标准库的 swap；
    }
};

namespace std {
  template<> void swap(Widget& a, Widget& b) {
    a.swap(b);
  }
}
```

### function template 偏特化

假设 Widget 和 WidgetImpl 都是 class template，这时以下的写法会企图偏特化一个 function template，但 C++ 只允许对 class template 偏特化：

```c++
namespace std {
    template<typename T>
    void swap< Widget<T> >(Widget<T>& a, Widget<T>& b);
}
```

所以想要偏特化一个 function template 的惯用做法是添加一个重载版：

```c++
namespace WidgetStuff {
    template<T>
    void swap(Widget<T>& a, Widget<T>& b) {
        a.swap(b);
    }
}
```

一般而言，重载 `function template`  时要注意命名空间，std 的内容完全由 C++ 标准委员会决定，它禁止我们膨胀哪些已经声明好的东西。

**最后来个总结**

- 一般情况默认使用 std::swap 函数即可
- 如果你的 class 或者 template 使用了 pimpl 手法，需要实现一个特化版的 swap：在 class 内提供一个 public swap 成员函数，高效地交换两个对象值，然后在同等命名空间下实现一个 non-member 函数 swap ，调用前面的 swap 函数。
- 即如果在实现一个 class，在 class 内特化 std::swap ，并令它调用成员函数 swap。