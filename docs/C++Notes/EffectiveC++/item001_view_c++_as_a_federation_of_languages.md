# 条款 01：视 C++ 为一个语言联邦

一开始，`C++` 只是 `C` 加上一些面向对象的特性。`C++` 最初的名称是 `C With Classes` ，也反映了这种血缘关系。

今天的 `C++` 已经是个多重泛型编程语言（`multiparadigm programming language`）。

一个同时支持过程形式（`procedural`）、面向对象形式（`object-oriented`）、函数形式（`functional`）、泛型形式（`generic`）、元编程形式（`metaprogramming`）的语言。这些能力和弹性使得 `C++` 成为一个无可匹敌的工具，但也可能引发某些迷惑：所有的适当用法似乎都有例外。我们该如何理解这样一个语言呢？

最简单的方法是将 `C++` 视为一个由相关语言组成的联邦而非单一语言。在其中某个次语言（`sublanguage`）中，各种守则与例子都倾向于简单、直观易懂。并且容易记住。 

总共有 4 个次语言：

（1）`C`：说到底 `C++` 仍是以 `C` 为基础。区块 （`blocks`）、语句 (`statements`)、预处理 (`preprocessor`)、内置数据类型 (`build-in data types`)、数组 (`arrays`)、指针 (`pointers`) 等统统来自 `C`。许多时候 `C++` 对问题的解法不过就是较高级的 `C` 解法。

（2） `Object-Oriented C++`：`classes` (包括构造函数和析构函数)，封装 (`encapsulation`)、继承(`inheritance`)、多态(`polymorphism`)、`virtual` 函数(动态绑定)…等等。

（3）`Template C++`：这是 `C++` 的泛型编程 (`generic programming`) 部分。实际上由于 `templates` 威力强大，它们带来崭新的编程范型 (`programming paradigm` ) ，也就是所谓的 `template metaprogramming` (TMP，模板元编程)。

（4）`STL `：是个 `template` 程序库。它对容器 (`containers`)、迭代器 (`iterators` )、算法 (`algorithms` ) 以及函数对象 (`function objects` ) 的规约有极佳的紧密配合与协调。

记住上面四个次语言，当你对某个次语言切换到另一个的时候，需要灵活的应对。

比如对内置（也就是 `C-like`）类型而言 `pass-by-value` 通常比 `pass-by-reference` 高效，但当你从 `C part of C++`  移往 `Object-Oriented C++` ，由于用户自定义（`user-defined`）构造函数和析构函数的存在，`pass-by-reference-to-const` 往往更好。运用 `Template C++` 时尤其如此，因此此时你甚至不知道所处理的对象的类型。然后一旦你跨入 STL 你就会了解，迭代器和函数对象都是在 C 指针之上塑造出来的，所以对 `STL` 的迭代器和函数对象而言，旧式的 C pass-by-value 守则再次适用。

因此，在 Scott Meyers 大神看来，C++ 并不是一个带有一组守则的一体语言：它是从四个次语言组成的联邦政府，每个次语言都有自己的规约，记住这四个次语言你就会发现 C++ 容易了解很多。

**请记住：**

> C++ 高效编程守则视状况而变化：取决于你使用 C++ 的哪一部分。



# 条款 02：尽量以 const，enum，inline替换 #define

