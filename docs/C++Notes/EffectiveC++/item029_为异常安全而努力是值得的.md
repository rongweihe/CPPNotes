# 条款 29：为“异常安全”而努力是值得的

Strive for exception-safe code.

```c++
void Obj::changeBackground(std::istream& imgSrc) {
  lock(&mutex);
  delete bgImage;
  ++imageChanges;
  bgImage = new Image(imgSrc);
  unlock(&mutex);
}
```

异常安全有两个条件：

- 不泄露任何资源：当 `new Image(imgSrc)` 导致异常时，`unlock`  则绝对不会执行，导致了锁资源的泄漏。
- 不允许数据败坏：当  `new Image(imgSrc)` 导致异常时，`bgImage` 就是指向一个已经被删除的对象，并且 `imageChanges` 也被累加。

而 C++ 提供了一些机制来保证异常的安全。

```c++

class Obj {
    ...
    std::tr2::shared_ptr<Image> bgImage;
    ...
};

void Obj::changeBackground(std::istream& imgSrc) {
    Lock m1(&mutex);                    // 使用资源管理类管理锁
    bgImage.reset(new Image(imgSrc);    // 智能指针
    ++imageChanges;
}
```

注意，这里不需要在手动 delete 对象，因为这个动作已经由智能指针处理掉了，此外，删除动作只发生在新图像对象被成功创建之后，更准确地说，std::tr2::shared_ptr.reset 函数只有在参数被成功生成之后才会被调用。

### copy and swap

有一种一般化的策略很典型地确保安全保证：这就是在修改一个对象时，可以先创建一个副本，然后在副本身上做一切必要的修改；若有任何修改动作抛出异常，原对象仍保持未改变的状态；待所有改变成功后，再将修改过的那个副本和原对象在一个不抛出异常的操作中置换。

“copy and swap” 策略是对对象状态做出“全有或全无”改变的一个很好的办法，但一般而言它并不保证整个函数有强烈的异常安全性。

“copy and swap” 策略有两个需要注意的方面：

- “连带影响”（side effect），如果函数值操作局部性状态（local state），便相对容易地提供强烈保证；但如果函数对“非局部性数据”（non-local data）有连带影响时，提供强烈保证就很困难；
- 效率，创建副本和交换操作将带来时间和空间的开销；

**最后总结一下：**

- 异常安全函数及时发生异常也不会泄露资源或允许任何数据结构的破坏，这样的函数区分分三种可能的保证：基本型、强烈型、不抛异常型。
- “强烈保证”往往能够以 “copy and swap” 策略实现出来。但“强烈保证”并非对所有函数都可实现或具备现实意义。
- 函数提供的“异常安全保证”通常最高值等于其所调用之各个函数的“异常安全保证”中的最弱者。