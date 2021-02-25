# 条款 16：成对使用 new 和 delete 时要采用相同形式

在使用 `new` 时，程序会执行两件事：

（1）为对象实例分配内存（调用 operator new）（2）针对被分配的内存调用对象的构造函数；

而在使用 `delete` 时，同样有两件事：

（1）针对此内存调用析构函数（2）释放内存（operator delete）。

`delete` 在释放对象时，需要知道即将被删除的内存中有多少个对象，即指针指向一个单一的对象，还是一个数组？对于数组而言，它通常还包括数组大小的记录。

所以在使用 `new` 和 `delete` 时应该配对使用：

```c++
std::string* stringPtr1 = new std::string;
std::string* stringPtr2 = new std::string[100];
...
delete stringPtr1;
delete[] stringPtr2;
```

- 如果使用 `delete[]` 去删除 `stringPtr1` ，`delete` 会读取若干内存将它解释为数组大小，然后开始多次调用析构函数，这有可能会越界去释放别的内存空间；
- 如果使用 `delete` 去删除 `stringPtr2` ，可能导致太少的析构函数被调用；

除此之外，还需要注意 `typedef` ：

```c++
typedef std::string AddressLines[4];
std::string* pal = new AddressLines;
delete []pal;
```

最后总结一下：

- **在我们使用 new 创建对象时，就应该使用 delete 删除对象。**
- **使用 new[] 创建对象时，应该对于地使用 delete[] 进行删除。**

