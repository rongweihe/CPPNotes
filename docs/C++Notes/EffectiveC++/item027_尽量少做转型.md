# 条款 27：尽量少做转型

C++ 规则的设计目标之一是保证类型错误绝不可能发生。但是程序中避免不了转型（casts）操作，而转型会破坏类型系统（type system）。

### C 风格的转型

C 风格的转型操作有两种，通常称为“旧式转型“（old-style casts）：

- (T)expression
- T(expression)

两种形式无差别，纯粹只是小括号的摆放位置不同。

### C++ 风格的转型

C++ 还提供了四种新式转型（new-style casts 或 C++-style casts）：

- `const_cast(expression)` ：通常用来将对象的常量性转除（cast away the constness）；
- `dynamic_cast(expression)` ：主要用于执行”安全向下转型“（safe downcasting），即用来决定某对象是否归属继承体系中的某个类型；
- `reinterpret_cast(expression)` ：意图执行低级转型，实际动作可能取决于编译器，所以它也就表示不可移植；
- `static_cast(expression)` ：用来强迫隐式转型；

新式转型相比旧式转型更受欢迎的原因有两个：

1. 它们很容易在代码中被辨识出来；
2. 各转型动作的目标愈窄化，编译器愈能诊断出错误的运用；

### 关于转型的错误认知

首先，转型操作不仅仅是告诉编译器把某种类型视为另一个类型，任何一个类型转换往往真的令编译器编译出运行期间执行的码。

其次，关于转型，我们很容易写出某些似是而非的代码，例如许多应用框架（application frameworks）都要求 derived classes 内的 virtual 函数代码的第一个动作就是先调用 base class 的对应函数：

```c++
class Window {
public:
    virtual void onResize() {}
    ...
};

class SpecialWindow: public Window {
public:
    virtual void onResize() {
        static_cast<Window>(*this).noResize();
        ...
    }
};
```

上面这段代码将 *this 转型为 Window。对函数 onResize 的调用也因此调用了 Window::onResize，但其实这个函数并没有作用在当前对象身上。它是在“当前对象的 base class 成分”副本上调用 Window::onResize。然后在当前对象身上执行 SpecialWindow 的专属动作。这使得当前对象进入一种”伤残“状态：其 base class 成分的更改没有落实，而 derived class 成分的更改倒是落实了。

所以如果只是想调用 base class 版本的 `onResize` 函数，令它作用于当前对象上，可以这样实现：

```c++
class SpecialWindow: public Window {
public:
    virtual void onResize() {
        Window::onResize();
        ...
    }
};
```

### dynamic_cast

dynamic_cast 通常使用在指向 derived class 的 “base” 指针或者引用，此时程序想要通过这个指针或引用调用 derived class 的操作函数，但这种转型方式的实现执行速度相当慢。所以通常情况下有两种方式避免这种转型：

- 第一种方式是使用容器并在其中储存直接指向 derived class 对象的指针，但是这种方法无法在同一个容器内存储指针”指向所有可能的派生类“；
- 第二种方式是通过 base class 接口处理”所有可能的各种派生类“，即提供 virtual 函数；

绝对必须避免的一件事情是”连串（cascading）dynamic_casts"：

```c++
class Window { ... };
class SpecialWindow1: public Window { ... };
class SpecialWindow2: public Window { ... };
class SpecialWindow3: public Window { ... };

...

Window* winPtrs = ...;
if (SpecialWindow1* pw1 = dynamic_cast<SpecialWindow1*>(winPtrs)) {...}
else if (SpecialWindow2* pw2 = dynamic_cast<SpecialWindow2*>(winPtrs)) {...}
else if (SpecialWindow3* pw3 = dynamic_cast<SpecialWindow3*>(winPtrs)) {...}
```

这样产生的代码又大又臭，而且基础不稳，因为每次 Window class 继承体系一有改变，所有这一类代码都必须再次检阅看是否需要修改。