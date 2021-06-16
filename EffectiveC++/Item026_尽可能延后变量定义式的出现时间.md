# 条款 26：尽可能延后变量定义式的出现时间

只要定义了一个变量而其类型带有一个构造函数或析构函数，那么当程序的控制流（control flow）到达这个变量定义式时，就要承受构造成本；当离开这个变量的作用域时，就要承受析构成本。

### 过早定义变量

```c++
std::string encryptPassword(const std::string& password) {
    std::string encrypted;
    
    if (password.length() < minimumPasswordLength) {
        throw logic_error("Password is too short");
    }
    ...
    return encrypted;
}
```

对象 `encrypted` 在此函数中并非完全未使用，但如果有个异常被丢出，它的构造过程等于无用功。

### 延后变量定义，直到需要它

```c++
std::string encryptPassword(const std::string& password) {
    if (password.length() < minimumPasswordLength) {
        throw logic_error("Password is too short");
    }
    
    std::string encrypted;
    ...
    
    return encrypted;
}
```

进一步想到：`encrypted`虽然获得定义却无任何实参作为初值，这意味着调用 default 构造函数，但许多时候对对象做的第一件事情就是给它一个初值，这样的话 default 构造函数的过程也等于无用功。

### 跳过无意义的 default 构造

```c++
std::string encryptPassword(const std::string& password) {
    if (password.length() < minimumPasswordLength) {
        throw logic_error("Password is too short");
    }
    
    std::string encrypted(password);
    ...
    
    return encrypted;
}
```

上述做法比较完美：尽可能延后定义式的真正意义在于延后这份定义直到能够给它初值实参为止，这样不仅能够避免构造和析构非必要对象，还可以避免无意义的 default 构造行为。

### 关于循环

```c++
// 方法 A：定义在循环外
Widget w;
for (int i = 0; i < n; ++i) {
    w = ...;
    ...
}

// 方法 B：定义在循环内
for (int i = 0; i < n; ++i) {
    Widget w(...);
    ...
}
```

具体采用哪种方法要考虑其对应的代价：

- A：1 个构造函数 + 1 个析构函数 + n 个赋值操作；
- B：n 个构造函数 + n 个析构函数；