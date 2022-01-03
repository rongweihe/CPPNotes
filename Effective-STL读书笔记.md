# Effective STL 阅读笔记

目录 
- [第 1 条：慎重选择容器类型](#第-1-条慎重选择容器类型)
- [第 2 条：不要试图编写独立于容器类型的代码](#第-2-条不要试图编写独立于容器类型的代码)
- [第 3 条：确保容器中的对象拷贝正确而高效](#第-3-条确保容器中的对象拷贝正确而高效)
- [第 4 条：调用 empty 而不是检查 size() 是否为 0](#第-4-条调用-empty-而不是检查-size-是否为-0)

## 第 1 条：慎重选择容器类型

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

  
## 第 2 条：不要试图编写独立于容器类型的代码

STL 是以泛化原则为基础的：

- 数组被泛化为”以其包含的对象的类型为参数“的容器；
- 函数被泛化为”以其使用的迭代器的类型为参数“的算法；
- 指针被泛化为”以其指向的对象的类型为参数“的迭代器；
- 容器被泛化为”序列式和关联式“容器。

试图编写对序列式容器和关联式容器都适用的代码几乎是毫无意义的。面对实际情况，不同容器是不同的，它们有非常明显的优缺点，不同的适用场景适合选择不同的容器。


## 第 3 条：确保容器中的对象拷贝正确而高效

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


## 第 4 条：调用 empty 而不是检查 size() 是否为 0

```c++
//对任意 c 容器，下面的代码本质上等价的
if (c.size() == 0) {}
if (c.empty()) {}
```

那既然如此，为何建议使用 empty 优先级于 size()，原因在于：empty 对所有的标准容器都是常数时间操作，而对一些 list 实现，size 函数耗费线性时间。（比如 list 容器的 splice 函数）。

## 第 5 条：区间成员函数优先于与之对应的单元素成员函数

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
## 第 6 条：当心 C++ 编译器最烦人的分析机制

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
		
## 第 7 条：如果容器中包含了通过 new 操作创建的指针，切记在容器对象析构前将指针 delete 掉

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

## 第 8 条：切勿创建包含 auto_ptr 的容器对象

auto_ptr 的容器(简称COAP) 是被禁止的。当你拷贝一个 auto_ptr 时，它所指向的对象的所有权被移交到拷入的 auto_ptr 上，而它自身被置为 NULL。如果你的目标是包含智能指针的容器，这并不意味着你要倒霉，包含智能指针的容器是没有问题的。但 auto_ptr 非智能指针。
				
				
## 第 9 条：慎重选择删除元素的方法

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
	
## 第 10 条：了解分配子(allocator)的约定和限制

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
		
	
## 第 12 条：切勿对 STL 容器的线程安全性有不切实际的依赖

对一个 STL 实现，你最多只能期望：
- 多个线程读取是安全的。
- 多个线程对不同的容器做写入操作是安全的

考虑当一个库视图实现完全的容器线程安全性时可能采取的方式：
- 对容器成员函数的每次调用，都锁住容器直到调用结束
- 在容器所返回的每个迭代器的生存期结束前，都锁住容器
- 对作用于容器的每个算法，都锁住该该容器，直到容器结束

当涉及到STL容器和线程安全性时，你可以指望一个STL库允许多个线程同时读一个容器，以及多个线程对不同的容器做写入操作。你不能指望STL库会把你从手工同步控制中解脱出来，而且你不能依赖于任何线程支持。

例如
```c++
std::vector<int> g_v;
void* fun(void *p) {
    for(int i = 0; i < 100000; i++) {
    	pthread_mutex_lock(&mutex);
        g_v.push_back(i);
		pthread_mutex_unlock(&mutex);
    }
 
    return NULL;
}
```
			      
## 第 13 条：vector和string优先于动态分配的数组

如果使用动态的分配数组，那么可能需要做更多的工作，为了减轻负担，使用 vector 和 string 。
			      
##  第 14 条：使用 reserve 来避免不必要的重新分配
```c++
int test_item_14() {
	std::vector<int> v;
	v.reserve(1000); // 如果不使用reserve,下面的循环在进行过程中将导致2到10次重新分配;加上reserve，则在循环过程中,将不会再发生重新分配
	for (int i = 1; i <= 1000; ++i) v.push_back(i);
	return 0;
}
```
对于 vector 和 string，增长过程是这样来实现的：每当需要更多空间时，就调用与 realloc类似的操作。这一类似于 realloc 的操作分为四部分：
- 分配一块大小为当前容量的某个倍数的新内存。在大多数实现中，vector 和 string 的容量每次以 2 的倍数增长，即，每当容器需要扩张时，它们的容量即加倍。
- 把容器的所有元素从旧的内存拷贝到新的内存中。
- 析构掉就内存中的对象。
- 释放旧内存。

reserve 函数能使你把重新分配的次数减少到最低程度，从而避免了重新分配和指针/迭代器/引用失效带来的开销。避免重新分配的关键在于，尽早地使用 reserve ，把容器的容量设为足够大的值，最好是在容器刚被构造出来之后就使用reserve。

通常有两种方式来使用reserve以避免不必要的重新分配。
第一种方式是，若能确切知道或大致预计容器中最终会有多少元素，则此时可以使用reserve。第二种方式是，先预留足够大的空间(根据你的需要而定)，然后，当把所有数据都加入以后，再去除多余的容量。

## 第 15 条：注意 string 实现的多样性

```c++
int test_item_15() {
	fprintf(stdout, "string size: %d, char* size: %d\n", sizeof(std::string), sizeof(char*));
 
	return 0;
}
```
- string 的值可能会被引用计数，也可能不会。很多实现在默认情况下会使用引用计数，但它们通常提供了关闭默认选择的方法，往往是通过预处理宏来做到这一点。
- string 对象大小的范围可以是一个 char* 指针大小的 1 倍到 7 倍。
- 创建一个新的字符串值可能需要零次、一次或两次动态分配内存。
- string 对象可能共享，也可能不共享其大小和容量信息。
- string 可能支持，也可能不支持针对单个对象的分配子。
- 不同的实现对字符内存的最小分配单位有不同的策略。

## 第 16 条：了解如何把 vector 和 string 数据传给旧的 API

C++ 标准要求 vector 中的元素存储在连续的内存中，就像数组一样。string 中的数据不一定存储在连续的内存中，而且 string 的内部表示不一定是以空字符结尾的。

## 第 17 条：使用”swap技巧”除去多余的容量

```c++
class Contestant {};
int test_item_17() {
	// 从contestants矢量中除去多余的容量
	std::vector<Contestant> contestants;
	// ... // 让contestants变大，然后删除它的大部分元素
	// vector<Contestant>(contestants)创建一个临时矢量，vector的拷贝构造函数只为所拷贝的元素分配所需要的内存
	std::vector<Contestant>(contestants).swap(contestants);
 
	contestants.shrink_to_fit(); // C++11
 
	std::string s;
	// ... // 让s变大，然后删除它的大部分字符
	std::string(s).swap(s);
 
	s.shrink_to_fit(); // C++11
 
	std::vector<Contestant>().swap(contestants); // 清除contestants并把它的容量变为最小
 
	std::string().swap(s); // 清除s并把它的容量变为最小
 
	return 0;
}
```

对 vector 或 string 进行 shrink-to-fit 操作时，考虑”swap”技巧。C++11 中增加了shrink_to_fit 成员函数。

swap 技巧的一种变化形式可以用来清除一个容器，并使其容量变为该实现下的最下值。

在做 swap 的时候，不仅两个容器的内容被交换，同时它们的迭代器、指针和引用也将被交换(string除外)。在 swap 发生后，原先指向某容器中元素的迭代器、指针和引用依然有效，并指向同样的元素----但是，这些元素已经在另一个容器中了。

## 第 18 条： 避免使用vector<bool>
```c++
int test_item_18() {
	std::vector<bool> v;
	// error: cannot convert 'std::vector<bool>::reference* {aka std::_Bit_reference}' to 'bool*' in initialization
	//bool* pb = &v[0]; // 不能被编译，原因：vector<bool>是一个假的容器，它并不真的储存bool，相反，为了节省空间，它储存的是bool的紧凑表示
	return 0;
}
```
作为一个 STL 容器，vector<bool> 只有两点不对。首先，它不是一个 STL 容器；其次，它并不存储bool。除此以外，一切正常。

在一个典型的实现中，储存在 ”vector” 中的每个 ”bool” 仅占一个二进制位，一个 8 位的字节可容纳 8 个 ”bool”。在内部，vector<bool> 使用了与位域 (bit field)一样的思想，来表示它所存储的那些bool，实际上它只是假装存储了这些 bool。

位域与 bool 相似，它只能表示两个可能的值，但是在 bool 和看似 bool 的位域之间有一个很重要的区别：你可以创建一个指向 bool 的指针，而指向单个位的指针则是不允许的。指向单个位的引用也是被禁止的。

当你需要 vector<bool>时，标准库提供了两种选择，可以满足绝大多数情况下的需求。

- 第一种是 deque<bool>。deque 几乎提供了 vector 所提供的一切(没有reserve和capacity)，但deque<bool> 是一个 STL 容器，而且它确实存储 bool。当然 deque 中元素的内存不是连续的，所以你不能把 deque<bool> 中的数据传递给一个期望 bool 数组的 C API。

- 第二种可以替代 vector<bool> 的选择是 bitset。bitset 不是 STL 容器，但它是标准 C++ 库的一部分。与 STL 容器不同的是，它的大小(即元素的个数)在编译时就确定了，所以它不支持插入和删除元素。

## 第 19 条：理解相等(equality)和等价(equivalence)的区别

相等的概念是基于 operator== 的。等价关系是以”在已排序的区间中对象值的相对顺序”为基础的。标准关联容器是基于等价而不是相等。

标准关联容器总是保持排列顺序的，所以每个容器必须有一个比较函数(默认为 less )来决定保持怎样的顺序。等价的定义正是通过该比较函数而确定的，因此，标准关联容器的使用者要为所使用的每个容器指定一个比较函数(用来决定如何排序)。

如果该关联容器使用相等来决定两个对象是否有相同的值，那么每个关联容器除了用于排序的比较函数外，还需要另一个比较函数来决定两个值是否相等。(默认情况下，该比较函数应该是 equal_to，但 equal_to 从来没有被用作 STL 的默认比较函数。当 STL 中需要相等判断时，一般的惯例是直接调用 operator ==)。
	
## 第 20 条：为包含指针的关联容器指定比较类型

每当你要创建包含指针的关联容器时，一定要记住，容器将会按照指针的值进行排序。绝大多数情况下，这不会是你所希望的，所以你几乎肯定要创建自己的函数子类作为该容器的比较类型(comparison type)。

如果你有一个包含智能指针或迭代器的容器，那么你也要考虑为它指定一个比较类型。对指针的解决方案同样也适用于那些类似指针的对象。就像DereferenceLess适合作为包含T*的关联容器的比较类型一样，对于容器中包含了指向T对象的迭代器或智能指针的情形，DereferenceLess也同样可用作比较类型。
	
## 第 21 条：总是让比较函数在等值情况下返回 false 

实际踩坑简化版

背景：现在我们有一个排序的结构体，假设数据经过召回、过滤等一系列操作后，得到最终的候选集，需要根据相应的策略，进行排序，最终返回 top k 结果
```c++
struct DataItem {
  std::string data_id;
  int priority;
  int score;
};
```

先根据 priority 值判断，最终根据 score 值是否相等返回。

线上代码如下：

```c++
void DataSort(std::vector<DataItem> &data_items) {
 std::sort(data_items.begin(), data_items.end(), [](const DataItem &item1, const DataItem &item2) {
   if (item1.priority < item2.priority) {
      return true;
    } else if (item1.priority > item2.priority) {
      return false;
    }
    return item1.score >= item2.score;
 });
}
```

测试环境构造测试 case，符合预期，上线。恐怖的事情来了，上线不久后，程序直接 coredump，然后自动重启，接着有 coredump，当时心情是很难形容。。。

把 core 文件拉到本地调试或者编译成 debug 版，线上问题复现，原因就是因为 DataSort 导致，但是在 DataSort 中，就一个简单的排序，sort 不可能出现崩溃，唯一的原因，就是 lambda 函数实现有问题。

重新修改 lambda 函数，把最后一句 return item1.score >= item2.score 改成 return false 执行，运行正常

打开 Google，输入 std::sort coredump，看到了一句话

❝
Having a non-circular relationship is called non-transitivity for the < operator. It’s not too hard to realise that if your relationships are circular then you won’t be getting reasonable results. In fact there is a very strict set of rules that a data type and its comparators must abide by in order to get correct results from C++ STL algorithms, that is 「strict weak ordering」.
❞

从上面的意思看，在 STL 中，对于 sort 函数中的排序算法，需要遵循严格弱序(strict weak ordering)的原则。

上面概念，总结下就是，存在两个变量x和y：

- x > y 等同于  y < x
- x == y 等同于 !(x < y) && !(x > y)

对于 std::sort()，当容器里面元素的个数大于 _S_threshold 的枚举常量值时，会使用快速排序，在 STL 中这个值的默认值是16

调试跟踪定位发现 sort 的函数调用链最终会调用 __unguarded_partition， 我们看下 __unguarded_partition 函数的定义：
```c++
template<typename _RandomAccessIterator, typename _Tp, typename _Compare>
     _RandomAccessIterator
     __unguarded_partition(_RandomAccessIterator __first,
               _RandomAccessIterator __last,
               _Tp __pivot, _Compare __comp) {
       while (true) {
       while (__comp(*__first, __pivot))
         ++__first;
       --__last;
       while (__comp(__pivot, *__last))
         --__last;
       if (!(__first < __last))
         return __first;
       std::iter_swap(__first, __last);
       ++__first;
     }
    }
```

在上面代码中，有下面一段：

```c++
while (__comp(*__first, __pivot))
         ++__first;
```

其中，__first 为迭代器，__pivot 为中间值，__comp 为传入的比较函数。
如果传入的 vector 中，按照之前的写法 >= 元素完全相等的情况下那么 __comp 比较函数一直是 true，那么后面 ++__first，最终就会使得迭代器失效，从而导致 coredump。
至此，分析完毕，请记住，STL sort 自定义比较函数，总是对相同值的比较返回 false。
	
	
## 第 22 条：切勿直接修改 set 或 multiset 中的键
	
```c++
int test_item_22() {
	std::map<int, std::string> m{ { 0, "xxx" } };
	//m.begin()->first = 10; // build error, map的键不能修改
 
	std::multimap<int, std::string> mm{ { 1, "yyy" } };
	//mm.begin()->first = 10; // build error, multimap的键同样不能修改
 
	std::set<int> s{ 1, 2, 3 };
	//*(s.begin()) = 10; // build error, set的键不能修改
	const_cast<int&>(*s.begin()) = 10; // 强制类型转换
 
	std::vector<int> v{ 1, 2, 3 };
	*v.begin() = 10;
 
	return 0;
}
```
像所有的标准关联容器一样，set 和 multiset 按照一定的顺序来存放自己的元素，而这些容器的正确行为也是建立在其元素保持有序的基础之上的。如果你把关联容器中的一个元素的值改变了(比如把10改为1000)，那么，新的值可能不在正确的位置上，这将会打破容器的有序性。

对于 map 和 multimap 尤其简单，因为如果有程序试图改变这些容器中的键，它将不能通过编译。这是因为，对于一个 map<K, V>或 multimap<K, V> 类型的对象，其中的元素类型是 pair<const K, V>。因为键的类型是 const K，所以它不能被修改。(如果利用 const_cast，你或许可以修改它。)

对于 set<T> 或 multiset<T> 类型的对象，容器中元素的类型是 T，而不是 const T。注：不通过强制类型转换并不能改变 set 或 multiset 中的元素。
	
## 第 23 条：考虑用排序的vector替代关联容器

这个建议的前提是：
- 创建一个新的数据结构，并插入大量元素，在这个阶段，几乎所有的操作都是插入和删除操作。很少或几乎没有查找操作。
- 查找阶段：查询该数据结构找到特点的信息，在这个阶段，几乎所有的操作都是查找很少或几乎没有删除。
- 重组阶段：改变数据结构的内容。

这种方式使用其数据结构的应用程序来说，排序的 vector 可能比管理容器提供了更好的性能。
