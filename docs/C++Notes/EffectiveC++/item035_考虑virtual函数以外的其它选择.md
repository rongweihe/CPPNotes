# 条款 35：考虑 virtual 函数以外的其他选择

Consider alternatives to virtual functions.

在为解决问题寻找某个设计方法的时候，不妨考虑 virtual 函数的替代方案。

- 使用 non-virtual interface （NV）；
- 将 virtual 函数替换为“函数指针的成员变量”。
- 以 tr1::function 成员变量替换 virtual 函数。
- 将继承体系内的 virtual 函数替换为另一个继承体系的 virtual 函数。

这一节比较难懂，先留个坑。。