# 条款 38：通过复合塑模出 has-a 或”根据某物实现出“

Model "has-a" or "is-implemented-in-terms-of" through composition.

当复合发生于应用域内的对象之间，表现出 has-a 的关系；当它发生于实现域内则表现为 is-implemented-in-terms-of 的关系。

### Has-a 关系

```c++
class Address { ... };
class Person {
private:
    Address address;
};
```

### is-implemented-in-terms-of 关系

```c++
template<class T>
class Set {
private:
    std::list<T> rep;
};
```

