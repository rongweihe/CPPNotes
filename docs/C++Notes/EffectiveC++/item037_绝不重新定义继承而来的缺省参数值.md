# 条款 36：绝不重新定义继承而来的 non-virtual 函数

在了解本条款前，先复习一下对象的静态类型和动态类型。

**对象的静态类型：**就是它在程序中被声明时所采用的类型，考虑以下的 class 继承体系。

```c++
class Shape {
public:
  enum ShapeColor {Red, Green, Blue};
  virtual void draw(ShapeColor color = Red) const = 0;
};
class Rectangle : public Shape {
public:
  //赋予不同的缺省参数。这比较糟糕
  virtual void draw(ShapeColor color = Green) const;
};

class Circle: public Shape {
public:
  virtual void draw(ShapeColor color) const;
  //以上这么写当客户以对象调用此函数，一定要指定参数值
  //因为静态绑定下这个函数并不从其base继承缺省参数值
  //但若以指针（或reference）调用此函数，可以不指定参数值
  //因为动态绑定下这个函数会从其base继承缺省参数值
};
```

```c++
Shape* ps;                    	//静态类型是Shape*
Shape* pc = new Circle;         //静态类型是Shape*
Shape* pr = new Rectangle;      //静态类型是Shape*
```

ps，pc，pr 不论这些指针指向什么，它们的静态类型都是 Shape*。

**对象的所谓动态类型：**则是指“目前所指对象的类型”，可以在程序执行过程中改变。

ps = pr;

ps = pc;

virtual 函数系动态绑定而来，意思是调用一个virtual 函数时， 究竟调用哪一份函数实现代码，取决于发出调用的那个对象的动态类型：

```c++
pc->draw(Shape::Red) //调用Circle::draw
pr->draw(Shape::Red) //调用Rectangle::draw
```

考虑带有缺省参数的 virtual 函数时，因为 virtual 函数是动态绑定的，而缺省参数是静态绑定的。意思是你可能在“调用一个定义于 derived class 内的 virtual 函数”的同时，却使用 base class  为他所指定的缺省参数值：

```c++
pr->draw(); //调用Rectangle::draw(Shape::Red)!
```

pr 的动态类型是 Rectangle，所以调用的是 Rectangle 的 virtual 函数。Rectangle::draw 函数的缺省参数应该是Green，但由于 pr 的静态类型是 Shape*，所以调用的缺省参数值来自 Shape class 而非 Rectangle class。

即使把指针换成reference，问题仍然存在。

那么为什么 C++ 坚持以这种乖张的方式来运作呢？在书中提到：原因在于运行期效率。如果缺省参数是动态绑定的，编译器就必须有某种办法在**运行期**为 virtual 函数决定适当的参数缺省值。这比目前实行的**在编译期决定**的机制更慢并且复杂。为了程序的执行速度和编译器实现上的简易度，c++ 做了这样的取舍，其结果就是你如今享受的执行效率。

我们知道一个程序需要经过编译，链接，执行这基本的三个步骤。

如果你遵守这条规则，并且同时提供缺省参数值给 base 和 derived classes 的用户，又会发生什么呢？

代码重复。

```c++
class Shape {
public:
    enum {Red, Green, Blue};
    virtual void draw(ShapeColor color = Red) const = 0;
};

class Rectangle : public Shape {
public:
    virtual void draw(ShapeColor color = Red) const;
};
```

怎么办？

当你想令 virtual 函数表现你所想要的行为却遭遇麻烦，聪明的做法是考虑替代设计：条款35。其中之一就是NVI手法：

令base class内的一个public non-virtual函数调用一个private virtual函数，后者可被derived classes重新定义。这里我们可以让non-virtual函数指定缺省参数，而private virtual函数负责真正的工作：

```c++
class Shape {
public:
    enum {Red, Green, Blue};
    void draw(ShapeColor color = Red) const
    {
        doDraw(color);
    }
private:
    virtual void doDraw(ShapeColor color) const = 0;
};

class Rectangle : public Shape {
public:
private:
    virtual void doDraw(ShapeColor color) const;//不需指定缺省参数
};
```

> 由于non-virtual函数应该绝对不被derived classes覆写（条款36），这个设计很清楚的使draw函数的color缺省参数值总为Red。（其实我个人还是不太明白这样写到底有什么好处）