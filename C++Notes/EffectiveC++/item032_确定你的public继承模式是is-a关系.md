# 条款 32：确定你的 public 继承塑模出 is-a 关系

Make sure public inheritance models "is-a".

书中写的比较详细，但个人觉得，最关键的其实也就是两句话：

- public 继承主张：能够施行于 base class 对象身上的每件事情，也应该可以实施于 derived class 对象身上。
- is-a 并非是唯一存在于 classes 之间的关系，另两个常见的关系是 has-a 和 is-implemented-in-terms-of。

**最后来个总结：**

- public 继承意味着 is-a ，适用于 base classes 身上的每一件事情也同样使用于 dervied classes身上，因为每一个派生类对象也都是基类对象。