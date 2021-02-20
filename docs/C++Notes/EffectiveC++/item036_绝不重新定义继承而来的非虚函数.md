# 条款 36：绝不重新定义继承而来的 non-virtual 函数

Never redefine an inherited non-virtual function.

考虑下面一段代码：

```c++
class B {
  public:
  	void mf();
};
class D : public B {};

D x;
B* pB = &x;
pB->mf();//调用B::mf
D* pD = &x;
pD->mf();//调用D::mf
```

造成这个现象的原因在于：虽然两者都是通过对象 x 调用成员函数 mf，但由于 non-virtual 函数是静态绑定，pB被声明一个 pointer-to-a 通过 pB 调用的 non-virtual 函数永远是 B 所定义的版本，即使 pB 指向一个类型为  “B派生类之 class”的对象。

而 virtual 函数确实动态绑定：如果 mf 是个虚函数，不论是通过 pB 或 pD 调用 mf，都会导致调用 D::mf，因为 pB 和 pD 真正指向的是类型为 D 的对象。

**最后总结一下**：

- 绝不重新定义继承而来的 non-virtual 函数。