# 条款 10：operator =返回一个 reference to *this

关于赋值，有趣的是你可以写成这样的形式：

```c++
int x,y,z;
x = y = z = 666;//连锁赋值
```

同样有趣的是，赋值采用右结合律，所以上述连锁赋值被解析为：

```c++
x = (y = (z = 666));
```

这里的 15 先被赋值 z，然后其结果（更新之后的 z）再被赋值给 y，然后其结果（更新之后的 y）再被赋值给 x。

为了实现“连锁赋值”，赋值操作符必须返回一个 reference 指向操作符的左侧实参，这是为 class 实现赋值操作符时应该遵循的协议。

```c++
class Widget {
	public:
		Widget& operator=(const Widget& rhs) { //返回类型是个 reference 指向当前对象
			//....
			return *this
		}
};
```

这个协议不仅仅使用与以上的标准赋值操作化，也适用于所有赋值相关运算，例如：

```c++
class Widget {
	public:
  	Widget& operator=(const Widget& rhs) { //返回类型是个 reference 指向当前对象
			//....
			return *this
		}
  	Widget& operator=(int rhs)
    {
      return *this;
    }
};
```

###  请记住

- **最好令赋值操作符（operator）返回一个 reference to *this 。**

- **另外注意注意参数里是一个 const 引用类型**

