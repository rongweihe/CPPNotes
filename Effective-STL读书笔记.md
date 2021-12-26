# Effective STL 阅读笔记

## 第一条：慎重选择容器类型

C++ 提供了几种不同的容器供你选择，你有没有发现他们的共同点在哪里？

- 标准 STL 序列容器：vector、string、deque、list。
- 标准 STL 关联容器：set、multiset、map、multimap（注：unordered_set、unordered_map 在 C++11 的时候也被引入标准库）。
- 非标准序列容器：slist：是一个单向链表，rope 本质上是一个”重型“string。
- 非标准的关联容器：hash_set、hash_multiset、hash_map、hash_multimap。

如果区分以上序列容器还是关联容器，《ESTL》提供了一种分类方法：基于连续内存容器和基于节点的容器分类。

**连续内存容器：**把它的元素存放在一块或多块（动态分配）内存中，每块内存中存有多个元素，当有新元素插入或已有的元素被删除时，统一内存块的元素需要向前或向后移动。以便给新元素让出空间，或者填充被删除元素所留下的空隙，这种移动影响效率和异常安全性。

**基于节点的容器**：每一个（动态分配）的内存块中只存放一个元素。容器中元素的插入或删除只影响到指向节点的指针，而不影响节点本身的内容，所以当有插入或删除操作时候，元素的值不需要移动。

有了这些基础，为了选择的时候有所顾虑，少踩一些坑，《ESTL》第一章给出了简单的建议：

- **是否需要在容器的任意位置插入新元素？** 如果需要，就选择序列容器否则选择关联容器。
- **是否关心容器中元素是排序的？** 如果不关心则哈希容器是一个可行选择方案；否则你要避免哈希容器。
- **需要哪种类型的迭代器？** 如果必须是随机访问迭代器，则对容器的选择就限定为vector、deque和string。如果要求使用双向迭代器，则避免使用slist和哈希容器。
- **当发生元素的插入和删除操作时候，避免移动容器中原来的元素是否重要？** 如果是就要避免选择序列容器。
- **容器中数据布局是否需要和C保持兼容？** 如果是是只能选择 vector。
- **元素的查找速度是否是关键的考虑因素？** 如果是考虑哈希容器。
- **如果容器内部使用引用计数技术是否介意？** 如果是则避免使用string因为许多string的实现都是用了引用计数。如果需要表示某种字符串的方法，可以使用vector<char>方法。
- **对插入和删除操作，需要事务语义么** 在插入和操作失败的时需要回滚的能力么？如果需要就有使用基于节点的容器，如果需要对多个元素插入操作需要事务，则可以选择list。因为在标准容器中，只有list提供了多个元素的事务语义。但注意：事务语义对编写异常安全代码很重要但同时付出性能上代价。
- **需要使用迭代器、指针和引用变为无效的次数最少么** 如果是就需要使用基于节点的容器，因为这类对容器的插入和删除从来不会使用迭代器和指针和引用无效；而对连续内存的容器的插入和删除一般会对指向该容器的迭代器和指针、引用变为无效。
- **如果在容器上使用swap使得迭代器失效了会在意吗** 如果在意那么避免使用 string，因为 string 是唯一在STL中 swap 操作过程中导致迭代器、指针和引用无效唯一的容器。
- **如果序列容器迭代器是随机访问，而且只要没有删除操作发生，且插入操作只发生容器末尾，则指向数据的指针和引用就不会变为无效，这样容器是否考虑** 这是一种很特殊的情况，如果是则 deque 满足你的需求，deque 是唯一的迭代器可能会变为无效和指针和引用不会变为无效的 STL 标准容器。

  
## 第二条：不要试图编写独立于容器类型的代码

STL 是以泛化原则为基础的：

- 数组被泛化为”以其包含的对象的类型为参数“的容器；
- 函数被泛化为”以其使用的迭代器的类型为参数“的算法；
- 指针被泛化为”以其指向的对象的类型为参数“的迭代器；
- 容器被泛化为”序列式和关联式“容器。

试图编写对序列式容器和关联式容器都适用的代码几乎是毫无意义的。面对实际情况，不同容器是不同的，它们有非常明显的优缺点，不同的适用场景适合选择不同的容器。



## 第三条：确保容器中的对象拷贝正确而高效

当(通过如 insert 或 push_back 之类的操作)向容器中加入对象时，存入容器的是你所指定的对象的拷贝。

当(通过如front或back之类的操作)从容器中取出一个对象时，你所得到的是容器中所保存的对象的拷贝。

**进去的是拷贝，出来的也是拷贝(copy in, copy out)。这就是 STL 的工作方式。**

一旦一个对象被保存到容器中，它经常会进一步被拷贝。当对 vector、string 或 deque 进行元素的插入或删除操作时，现有元素的位置通常会被移动(拷贝)。如果你使用下列任何操作----排序算法，next_permutation 或previous_permutation, remove、unique 或类似的操作，rotate 或 reverse,等等----那么对象将会被移动(拷贝)。没错，拷贝对象是 STL 的工作方式。

利用一个对象的拷贝成员函数就可以很方便地拷贝该对象，特别是对象的拷贝构造函数(copy constructor)和拷贝赋值操作符(copy assignment operator)。

在存在继承关系的基础下，拷贝动作会导致剥离，也就是说，如果你创建了一个存放基类对象的容器，却向其中插入派生类的对象，那么在派生类对象(通过基类的拷贝构造函数)被拷贝进容器时，它所特有的部分(即派生类中的信息)将会丢失。**剥离问题**意味着向基类对象的容器中插入派生类对象几乎总是错误的**。

```c++
class Widget {};
class SpecialWidget: public Widget {};
int test_item_3() {
  std::vector<Widget> vw;
  SpecialWidget sw;
  vw.push_back(sw);//sw作为基类对象被拷贝进vw，它的派生类特有的部分在拷贝的时候被丢弃了
}
```

使拷贝动作高效、正确，并防止剥离问题发生的一个简单办法是使容器包含指针而不是对象。比如使用 `std::vector<Widget*> vw`; 而非上述操作。
  

## 第四条：调用 empty 而不是检查 size() 是否为 0

```c++
//对任意 c 容器，下面的代码本质上等价的
if (c.size() == 0) {}
if (c.empty()) {}
```

那既然如此，为何建议使用 empty 优先级于 size()，原因在于：empty 对所有的标准容器都是常数时间操作，而对一些 list 实现，size 函数耗费线性时间。（比如 list 容器的 splice 函数）。
  
## 第五条：区间成员函数优先于与之对应的单元素成员函数

```c++
class Widget5 {};
 
int test_item_5()
{
	std::vector<Widget5> v1, v2;
	v1.assign(v2.begin() + v2.size() / 2, v2.end()); // 推荐
 
	v1.clear();
	for (std::vector<Widget5>::const_iterator ci = v2.begin() + v2.size() / 2; ci != v2.end(); ++ci) // 不推荐
		v1.push_back(*ci);
 
	v1.clear();
	std::copy(v2.begin() + v2.size() / 2, v2.end(), std::back_inserter(v1)); // 效率不如assign
 
	v1.clear();
	v1.insert(v1.end(), v2.begin() + v2.size() / 2, v2.end()); // 对copy的调用可以被替换为利用区间的insert版本
 
	const int numValues = 100;
	int data[numValues];
 
	std::vector<int> v;
	v.insert(v.begin(), data, data + numValues); // 推荐，使用区间成员函数insert
 
	std::vector<int>::iterator insertLoc(v.begin());
	for (int i = 0; i < numValues; ++i) {
		insertLoc = v.insert(insertLoc, data[i]); // 不推荐，使用单元素成员函数
		++insertLoc;
	}
 
	return 0;
}
```

区间成员函数是指这样的一类成员函数，它们像 STL 算法一样，使用两个迭代器参数来确定该成员操作所执行的区间。如果不使用区间成员函数就得写一个显示的循环。

优点在于：

- C++ 标准要求区间insert 函数把现有容器中元素直接移动到它们最终的位置上，即只需要付出每个元素移动一次的代价。
- 明智地使用区间插入而不是单元素重复插入会提高程序的性能问题，比如对于vector来说如果内存已满再插入新元素会触发两倍扩容，区间插入不必多次重新分配内存。
- 区间成员函数减少代码量，形成更易懂的代码，增强软件长期可维护性。

那么，都有哪些区间成员函数？

- 区间创建函数、insert、erase、assign等。

```c++
container::container(InputIterator begin, InputIterator end);//区间
void container::container(iterator position, InputIterator begin, InputIterator end);
iterator container::erase(iterator begin, iterator end);
void container::erase(iterator begin, iterator end);
void container::assign(iterator begin, iterator end);
```

