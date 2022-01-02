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
## 第六条：当心 C++ 编译器最烦人的分析机制

```c++
// 注意：围绕参数名的括号(比如对f2中d)与独立的括号的区别：围绕参数名的括号被忽略，而独立的括号则表明参数
// 列表的存在：它们说明存在一个函数指针参数
int f1(double d); // 声明了一个带double参数并返回int的函数
int f2(double(d)); // 同上，d两边的括号被忽略,可以给参数名加上圆括号
int f3(double); // 同上，参数名被忽略
 
int g1(double(*pf)()); // 参数是一个指向不带任何参数的函数的指针，该函数返回double值；g1以指向函数的指针为参数
int g2(double pf()); // 同上，pf为隐式指针
int g3(double()); // 同上，省去参数名
 
int test_item_6()
{
	// 把一个存有整数(int)的文件ints.dat拷贝到一个list中
	std::ifstream dataFile("ints.dat");
	std::list<int> data1(std::istream_iterator<int>(dataFile), std::istream_iterator<int>()); // 小心，结果不会是你所想象的那样
 
	std::list<int> data2((std::istream_iterator<int>(dataFile)), std::istream_iterator<int>()); // 正确，注意list构造函数的第一个参数两边的括号
 
	std::istream_iterator<int> dataBegin(dataFile);
	std::istream_iterator<int> dataEnd;
	std::list<int> data3(dataBegin, dataEnd); // 正确
 
	return 0;
}
```

使用命名的迭代器对象与通常的STL程序风格相违背，但你或许觉得为了使代码对所有编译器都没有二义性，并且使维护代码的人理解起来更容易，这一代价是值得的。
		
## 第七条：如果容器中包含了通过 new 操作创建的指针，切记在容器对象析构前将指针 delete 掉

```c++
class Widget7 {};
 
struct DeleteObject {
	template<typename T>
	void operator()(const T* ptr) const
	{
		delete ptr;
	}
};
 
int test_item_7()
{
	const int num = 5;
 
	std::vector<Widget7*> vwp1, vwp2;
	for (int i = 0; i < num; ++i) {
		vwp1.push_back(new Widget7); // 如果在后面自己不delete，使用vwp在这里发生了Widget7的泄露
		vwp2.push_back(new Widget7);
	}
 
	for (std::vector<Widget7*>::iterator i = vwp1.begin(); i != vwp1.end(); ++i) {
		delete *i; // 能行，但不是异常安全的
	}
 
	for_each(vwp2.begin(), vwp2.end(), DeleteObject()); // 正确，类型安全，但仍不是异常安全的
 
	typedef std::shared_ptr<Widget7> SPW; // SPW"指向Widget7的shared_ptr"
	std::vector<SPW> vwp3;
	for (int i = 0; i < num; ++i) {
		vwp3.push_back(SPW(new Widget7)); // 从Widget7创建SPW,然后对它进行一次push_back使用vwp3,这里不会有Widget7泄露，即使有异常被抛出
	}
 
	return 0;
}
```

STL 容器很智能，但没有智能到知道是否该删除自己所包含的指针的程度。当你使用指针的容器，而其中的指针应该被删除时，为了避免资源泄漏，你必须或者用引用计数形式的智能指针对象(比如std::shared_ptr)代替指针，或者当容器被析构时手工删除其中的每个指针。

## 第八条：切勿创建包含 auto_ptr 的容器对象

auto_ptr 的容器(简称COAP) 是被禁止的。当你拷贝一个 auto_ptr 时，它所指向的对象的所有权被移交到拷入的 auto_ptr 上，而它自身被置为 NULL。如果你的目标是包含智能指针的容器，这并不意味着你要倒霉，包含智能指针的容器是没有问题的。但 auto_ptr 非智能指针。
				
				
## 第九条：慎重选择删除元素的方法

```c++
bool badValue(int) { return true; } // 返回x是否为"坏值"
 
int test_item_9()
{
	// 删除c中所有值为1963的元素
	std::vector<int> c1;
	c1.erase(std::remove(c1.begin(), c1.end(), 1963), c1.end()); // 当c1是vector, string或deque时，erase-remove习惯用法是删除特定值的元素的最好办法
 
	std::list<int> c2;
	c2.remove(1963); // 当c2是list时，remove成员函数是删除特定值的元素的最好办法
 
	std::set<int> c3;
	c3.erase(1963); // 当c3是标准关联容器时，erase成员函数是删除特定值元素的最好办法
 
	// 删除判别式(predicate)返回true的每一个对象
	c1.erase(std::remove_if(c1.begin(), c1.end(), badValue), c1.end()); // 当c1是vector, string或deque时，这是删除使badValue返回true的对象的最好办法
 
	c2.remove_if(badValue); // 当c2是list时，这是删除使badValue返回true的对象的最好办法
 
	for (std::set<int>::iterator i = c3.begin(); i != c3.end();) {
		if (badValue(*i)) c3.erase(i++); // 对坏值，把当前的i传给erase，递增i是副作用
		else ++i;                        // 对好值，则简单的递增i
	}
 
	// 每次元素被删除时，都向一个日志(log)文件中写一条信息
	std::ofstream logFile;
	for (std::set<int>::iterator i = c3.begin(); i != c3.end();) {
		if (badValue(*i)) {
			logFile << "Erasing " << *i << '\n'; // 写日志文件
			c3.erase(i++); // 对坏值，把当前的i传给erase，递增i是副作用
		}
		else ++i;              // 对好值，则简单第递增i
	}
 
	for (std::vector<int>::iterator i = c1.begin(); i != c1.end();) {
		if (badValue(*i)) {
			logFile << "Erasing " << *i << '\n';
			i = c1.erase(i); // 把erase的返回值赋给i，使i的值保持有效
		}
		else ++i;
	}
 
	return 0;
}
```

总结一下：

- 要删除容器中有特定值的所有对象：如果容器是 vector，string 或deque，则使用 erase-remove 习惯用法；如果容器是list，则使用 list::remove；如果容器是一个标准关联容器，则使用它的 erase 成员函数。

- 要删除容器中满足特定判别式(条件)的所有对象：如果容器是vector， string或deque，则使用erase-remove_if习惯用法；如果容器是list，则使用list::remove_if；如果容器是一个标准关联容器，则使用remove_copy_if和swap，或者写一个循环来遍历容器中的元素，记住当把迭代器传给erase时，要对它进行后缀递增。

- 要在循环内做某些(除了删除对象之外的)操作：如果容器是一个标准序列容器，则写一个循环来遍历容器中的元素，记住每次调用erase时，要用它的返回值更新迭代器；如果容器是一个标准关联容器，则写一个循环来遍历容器中的元素，记住当把迭代器传给erase时，要对迭代器做后缀递增。
	
## 第十条：了解分配子(allocator)的约定和限制

编写自定义的分配子，需要注意：

- 你的分配子是一个模板，模板参数T代表你为它分配内存的对象的类型。
- 提供类型定义 pointer 和 reference，但是始终让 pointer 为T指针，reference为T&。
- 千万别让你的分配子拥有随对象而不同的状态(per-object state)。通常，分配子不应该有非静态的数据成员。*
- 记住，传给分配子的 allocate 成员函数的是那些要求内存的对象的个数，而不是所需的字节数。同时要记住，这些函数返回T*指针(通过pointer类型定义)，即使尚未有T对象被构造出来。
- 一定要提供嵌套的 rebind 模板，因为标准容器依赖该模板。
	
## 第 11 条：理解并自定义分配子的合理用法

看一段代码

```c++

void* mallocShared(size_t bytesNeed) {
    return malloc(bytesNeed);
}
void freeShared(void* ptr) {
    free(ptr);
}

template<typename T>
class SharedMemoryAllocator {
public:
    typedef T* pointer;//pointer是个类型定义，它实际上总是T*
    typedef size_t size_type; //通常情况下，size_type 是 size_t 的一个类型定义
    typedef T value_type;

    pointer allocate(size_type numObj, const void* localHint = 0) {
        return static_cast<pointer>(mallocShared(numObj * sizeof(T)));
    }
    void deallocate(pointer ptrToMemory, size_type numObj) {
		freeShared(ptrToMemory);
	}
    template<typename U>
	struct rebind {
		typedef std::allocator<U> other;
	};
};

int test_item_11() {
	typedef std::vector<double, SharedMemoryAllocator<double>> SharedDoubleVec;
	// v所分配的用来容纳其元素的内存将来自共享内存
	// 而v自己----包括它所有的数据成员----几乎肯定不会位于共享内存中，v只是普通的基于栈(stack)的对象，所以，像所有基于栈的对象一样，它将会被运行时系统放在任意可能的位置上。这个位置几乎肯定不是共享内存
	SharedDoubleVec v; // 创建一个vector,其元素位于共享内存中
 
	// 为了把v的内容和v自身都放到共享内存中，需要这样做
	void* pVectorMemory = mallocShared(sizeof(SharedDoubleVec)); // 为SharedDoubleVec对象分配足够的内存
	SharedDoubleVec* pv = new (pVectorMemory)SharedDoubleVec; // 使用"placement new"在内存中创建一个SharedDoubleVec对象
	// ... // 使用对象(通过pv)
	pv->~SharedDoubleVec(); // 析构共享内存中的对象
	freeShared(pVectorMemory); // 释放最初分配的那一块共享内存
 
	return 0;
}

```
遵守同一类型的分配子必须是等价的这一限制要求。
