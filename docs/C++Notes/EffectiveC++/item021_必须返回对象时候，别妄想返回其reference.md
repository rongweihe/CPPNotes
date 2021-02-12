# 条款 21：必须返回对象时候，别妄想返回其 reference

使用 pass-by-reference 会有一个致命的错误：传递一些 reference 指向其实并不存在的对象。例如企图实现一个返回 reference 的函数来替代 pass-by-value。

```c++
const Object& operator* (const Object& lhs, const Object& rhs) {
  Object ret(lhs.a * rhs.a);
  return ret;
}
```

这种写法的问题在于：这个函数返回了一个 reference 指向 `ret`，但 `ret` 是个 `local` 对象，而 `local` 对象在函数退出之前就被销毁了，所以这个 reference 指向了一个“从前的对象”。

函数创建对象的途径一般有两个：在 stack 空间或者 heap 空间，如果定义一个 local 对象，就是在 stack 空间创建对象。

有人可能会写出下面这种代码：

```c++
const Object& operator* (const Object& lhs, const Object& rhs) {
    Object* ret = new Object(lhs.n * rhs.n);
    return *ret;
}
```

这种实现的问题在于：谁改对这个 new 出来的对象进行释放操作？并且这种写法很有可能因为客户端的调用而直接导致内存泄漏。

比如：

```c++
Object w, x, y, z;
w = x * y * z;
```

即使客户端有意识的删除 `w` 对象，但是这行代码中调用了两次 `new`。其中一个 `new` 出来的空间已经泄漏。

还有一种实现方法：

```c++
const Object& operator* (const Object& lhs, const Object& rhs) {
    static Object ret;
    ret = ...;
    return ret;
}
```

 这种实现方式有两个缺陷：

- 线程不安全，所有线程共享 `static`  对象；
- 共享 `static`  导致的问题，比如 `(a * b) == (c * d)` 永远被判定为 `True`；

一个“必须返回对象的函数”的正确写法就是：让那个函数返回一个对象。

```c++
inline const Object operator*(const Obj& lhs, const object& rhs) {
	return Object(lsh.n + rhs.n);
}
```

最后来个总结：

- **一个必须返回对象的函数的正确写法就是让那个函数返回一个对象。**

