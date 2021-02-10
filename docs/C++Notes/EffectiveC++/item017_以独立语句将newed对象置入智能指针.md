# 条款 17：以独立语句将 newed 对象置入智能指针

假设有以下两个接口：

```c++
int priority();
void processWidget(std::tr1::shared_ptr<Widget> pw, int priority);
```

如果使用不当，会导致内存泄漏：

```c++
processWidget(std::tr1::shared_ptr<Widget> (new Widget), priority());
```

这行代码可以通过编译，但在编译器产出一个 `processWidget` 调用码之前，会先核实即将被传递的各个实参。第二个实参只是单纯的对 `priority` 函数的调用，但第一个实参由两部分组成：

- 执行 `new Widget` 表达式；
- 调用 `tr1::shared_ptr` 构造函数；

于是在调用 `processWidget` 之前，编译器必须创建代码，做以下三件事情：

- 调用 `priority` ；
- 执行 `new Widget` ；
- 调用 `tr1::shared_ptr` 构造函数；

但是这三件事的执行顺序没有被明确定义，如果最终获得这样的操作序列：

1. 执行 `new Widget` ；
2. 调用 `priority` ；
3. 调用 `tr1::shared_ptr` 构造函数；

这种情况下，如果在调用 `priority` 函数时导致异常，此时 `new Widget` 返回的指针将会遗失，这会导致资源泄漏。

所以为了避免这类问题，应该分离语句，分别写出下面两个代码：

（1）创建 `Widget` ；

（2）再将它置入一个智能指针，最后再传入 `processWidget` 函数。

```c++
std::tr1::shared_ptr<Widget> pw(new Widget);
//在单独语句内以智能指针存储 newed 所得对象。
processWidget(pw, priority());
```

以上做法之所以行得通，因为编译器对于”跨越语句的各项操作“没有重新排列的自由。

对 `std::tr1::shared_ptr 构造函数的调用`  和 `priority`的调用时分隔开来的，位于不同的语句内。所以编译器不得在它们之间任意执行次序。

最后的总结：

-  **以独立语句将 newed 对象存储于智能指针内。如果不这样做，一旦异常被抛出，有可能导致难以察觉的资源泄漏。**