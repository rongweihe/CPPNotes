# 条款 24：若所有参数皆需类型转换，请为此采用 non-member 函数

令 class 支持隐私类型转换通常是一个糟糕的行为，但也有例外，最常见的例外就是在建立数值类型的时候。

考虑一个有理数类，允许整数隐私转换为有理数似乎很有道理。

```c++
class Rational {
public:
    Rational(int numerator = 0, int denominator = 1);
    ...
};
```

现在研究一下这个类的乘法实现：

```c++
class Rational {
public:
    const Rational operator* (const Rational& rhs) const;
};
Rational ret, one;
ret = one * 2;//成功
ret = 2 * one;//失败
```

第一行的运算之所以成功是因为发生了隐式类型转换`（implicit type conversion）` ，编译器会自动调用 `Rational`  的构造函数将 ` int`  变成 `Rational` 。当然，只有在构造函数为 `non-explicit ` 时，编译器才会这样做。所以为了实现混合运算，应该让 `operator*` 成为一个 non-member 函数：

```c++
const Rational operator*(const Rational& lhs, const Rational& rhs) {
    return Rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
}
```

