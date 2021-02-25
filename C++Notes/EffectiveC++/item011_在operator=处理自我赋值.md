# 条款 11：operator = 中处理自我赋值

自我赋值发生在对象赋值给自己的时候：

```c++
class Widget {...}
Widget w;
...
w = w;
```

这看起来有点愚蠢，但确实是合法的的行为，而且并不总是一眼能辨别出来。

比如说：

```c++
a[i] = a[j];	//存在潜在的自我赋值，当 i 和 j 有相同的值的时候，这便是个自我赋值。
*px = *py;    //如果 px 和 py 恰巧指向同一个东西，这也是自我赋值。
```

下面是一段看起来比较合理的自我赋值的代码。

```c++
class Bitmap {...}
class Widget{
  //...
private:
	Bitmap* bp;  	
}
Widget& Widget::operator=(const Widget& rhs) {
	delete bp;
	bp = new Bitmap(*rhs.bp);
	return *this;
}
```

上述这段代码存在的问题是：`operator=`  函数内的 `*this` （赋值的目的端）和 `rhs`  有可能是同一个对象。果真如此 `delete`  就不只是销毁当前对象的 `bitmap`，它也销毁 `rhs 的 bitmap` 。在函数末尾—— `Widget`  发现自己持有一个指针指向一个已被删除的对象。

想要防止这个问题，传统做法是在赋值前加一个判断，达到“自我赋值”的校验目的。

```c++
Widget& Widget::operator=(const Widget& rhs) {
	if (this == &rhs) return *this;//如果是自我赋值就不做任何事。
  delete bp;
	bp = new Bitmap(*rhs.bp);
	return *this;
}
```

注意 `if (this == &rhs)`   不能写成 ` if (*this == rhs) ` ，这样写是不对的，因为 `==`  是用于对象内每一个成员变量是否相同的判断，而不是地址是否重叠的判断。所以用 `this == &rhs`  才能从地址上来捕捉到是否真的是自我赋值。

如果思考的在深入一点，你会发现，这一版仍然存在异常方面的麻烦。更明确地说，这样做确实能解决上面所说的第一问题：自我赋值。事实上还可能出现另一个问题导致代码崩溃，试想，如果 `bp = new Bitmap` 不能正常分配空间怎么办，突然抛出了异常怎么办，这将导致原有空间的内容被释放，但新的内容又不能正常填充。有没有一个好的方法，在出现异常时，还能保持原有的内容不变呢？（可以提升程序的健壮性）

这有两种思路，书上先给出了这样的方案：我们只需要注意在复制 `bp` 所指东西之前别删除 `bp` 。

```c++
Widget& Widget::operator=(const Widget& rhs) {
	Bitmap* bOld = bp;//记住原先的 bp
	bp = new Bitmap(*rhs.bp);
  delete bOld;
	return *this;
}
```

现在的话，如果 `new Bitmap`  抛出异常， `bp` （以及后面的 `Widget`  ）保持原状，及时没有自我校验，这段代码还是能正常处理自我赋值。

还有一种思路，就是先用临时的指针申请新的空间并填充内容，没有问题后，再释放到本地指针所指向的空间，最后用本地指针指向这个临时指针。

上述两种方法都是可行，但还要注意拷贝构造函数里面的代码与这段代码的重复性，试想一下，如果此时对类增加一个私有的指针变量，这里面的代码，还有拷贝构造函数里面类似的代码，都需要更新，有没有可以一劳永逸的办法？

本书给出了最终的解决方案：

```c++
//利用 copy and swap 技术
class Widget {
  //...
  void swap(Widget& rhs);//交换*this和rhs的数据
  //...
};
Widget& Widget::operator=(const Widget& rhs) {
  Widget tmp(rhs);
  swap(tmp);
  return *this;
}
```

这样把负担都交给了拷贝构造函数，使得代码的一致性能到保障。如果拷贝构造函数中出了问题，比如不能申请空间了，下面的 `swap` 函数就不会执行到，达到了保持本地变量不变的目的。

最后总结一下：

- **确保当对象自我赋值时 `operator=` 有良好的行为，其中技术包括比较“来源对象”和“目标对象”的地址、精心周到的语句顺序、以及 `copy-and-swap` ；**

- **确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。**