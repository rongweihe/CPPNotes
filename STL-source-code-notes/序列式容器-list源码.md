## 前言

在 SGI STL中，list 容器是一个循环的双向链表，它的内存空间效率较前文介绍的 vector 容器高。

因为 vector 容器的内存空间是连续存储的，且在分配内存空间时，会分配额外的可用空间；

而 list 容器的内存空间不一定是连续存储，内存之间是采用迭代器或节点指针进行连接，并且在插入或删除数据节点时，就配置或释放一个数据节点，并不会分配额外的内存空间，这两个操作过程都是常数时间。

与 vector 容器不同的是，list 容器在进行插入操作或拼接操作时，迭代器并不会失效；

且不能以普通指针作为迭代器，因为普通指针的+或-操作只能指向连续空间的后移地址或前移个地址，不能保证指向list的下一个节点，迭代器必须是双向迭代器，因为 list 容器具备有前移和后移的能力。

注：本文所列的源码出自SGI STL中的文件<stl_list.h>，对于 list 容器类的详细信息也可以查看[《list容器库》](http://zh.cppreference.com/w/cpp/container/list)



### transfer函数

**`transfer`函数功能是将一段链表插入到我们指定的位置之前**. 该函数一定要理解, 后面分析的所有函数都是该基础上进行修改的.

`transfer`函数接受3个迭代器. 第一个迭代器表示链表要插入的位置, `first`到`last`最闭右开区间插入到`position`之前.

从`if`下面开始分析(*这里我将源码的执行的先后顺序进行的部分调整, 下面我分析的都是调整顺序过后的代码. 当然我也会把源码顺序写下来, 以便参考*)

- **为了避免待会解释起来太绕口, 这里先统一一下部分名字**

1. `last`的前一个节点叫`last_but_one`
2. `first`的前一个节点叫`zero`

- 好, 现在我们开始分析`transfer`的每一步(*最好在分析的时候在纸上画出两个链表一步步来画*)

1. 第一行. `last_but_one`的`next`指向插入的`position`节点
2. 第二行. `position`的`next`指向`last_but_one`
3. 第三行. 临时变量`tmp`保存`position`的前一个节点
4. 第四行. `first`的`prev`指向`tmp`
5. 第五行. `position`的前一个节点的`next`指向`first`节点
6. 第六行. `zero`的`next`指向`last`节点
7. 第七行. `last`的`prev`指向`zero`

```c++
template <class T, class Alloc = alloc>
class list {
    ...
protected:
    void transfer(iterator position, iterator first, iterator last)  {
      if (position != last) {
        (*(link_type((*last.node).prev))).next = position.node;
        (*(link_type((*first.node).prev))).next = last.node;
        (*(link_type((*position.node).prev))).next = first.node;  
        link_type tmp = link_type((*position.node).prev);
        (*position.node).prev = (*last.node).prev;
        (*last.node).prev = (*first.node).prev; 
        (*first.node).prev = tmp;
      }
    }
    ...
};
```

**splice** 将两个链表进行合并.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
public:
    void splice(iterator position, list& x) {
      if (!x.empty()) 
        transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&, iterator i) {
      iterator j = i;
      ++j;
      if (position == i || position == j) return;
      transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first, iterator last) {
      if (first != last) 
        transfer(position, first, last);
    }
    ...
};
```

## list 源码剖析

```c++
//以下是链表List_iterator_base的迭代器
struct _List_iterator_base {
	//数据类型
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  //list迭代器的类型是双向迭代器bidirectional_iterator
  typedef bidirectional_iterator_tag iterator_category;

  //定义指向链表节点的指针
  _List_node_base* _M_node;

  //构造函数
  _List_iterator_base(_List_node_base* __x) : _M_node(__x) {}
  _List_iterator_base() {}

  //更新节点指针，指向直接前驱或直接后继节点
  void _M_incr() { _M_node = _M_node->_M_next; }
  void _M_decr() { _M_node = _M_node->_M_prev; }

  //操作符重载
  bool operator==(const _List_iterator_base& __x) const {
    return _M_node == __x._M_node;
  }
  bool operator!=(const _List_iterator_base& __x) const {
    return _M_node != __x._M_node;
  }
};  

//以下是链表List_iterator的迭代器
template<class _Tp, class _Ref, class _Ptr>
struct _List_iterator : public _List_iterator_base {
  typedef _List_iterator<_Tp,_Tp&,_Tp*>             iterator;
  typedef _List_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;
  typedef _List_iterator<_Tp,_Ref,_Ptr>             _Self;

  typedef _Tp value_type;
  typedef _Ptr pointer;
  typedef _Ref reference;
  typedef _List_node<_Tp> _Node;

  //构造函数
  _List_iterator(_Node* __x) : _List_iterator_base(__x) {}
  _List_iterator() {}
  _List_iterator(const iterator& __x) : _List_iterator_base(__x._M_node) {}

  //以下都是基本操作符的重载,取出节点数据
  reference operator*() const { return ((_Node*) _M_node)->_M_data; }

#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  _Self& operator++() { 
    this->_M_incr();
    return *this;
  }
  _Self operator++(int) { 
    _Self __tmp = *this;
    this->_M_incr();
    return __tmp;
  }
  _Self& operator--() { 
    this->_M_decr();
    return *this;
  }
  _Self operator--(int) { 
    _Self __tmp = *this;
    this->_M_decr();
    return __tmp;
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

//返回迭代器的类型
inline bidirectional_iterator_tag
iterator_category(const _List_iterator_base&)
{
  return bidirectional_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr>
inline _Tp*
value_type(const _List_iterator<_Tp, _Ref, _Ptr>&)
{
  return 0;
}

inline ptrdiff_t*
distance_type(const _List_iterator_base&)
{
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
```

### list容器的构造函数和析构函数

这里把list容器的构造函数列出来讲解，使我们对list容器的构造函数进行全面的了解，以便我们对其使用。在以下源码的前面我会总结出list容器的构造函数及其使用方法。

```c++
//以下是双向链表list类的定义，分配器_Alloc默认为第二级配置器
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class list : protected _List_base<_Tp, _Alloc> { 
	 ...
public:
 //************************************
  /*************以下是构造函数******************
  //***********默认构造函数*********************
	explicit list( const Allocator& alloc = Allocator() );
  //************具有初值和大小的构造函数************
	explicit list( size_type count,
               const T& value = T(),
               const Allocator& alloc = Allocator());
         list( size_type count,
               const T& value,
               const Allocator& alloc = Allocator());
  //********只有大小的构造函数********************
	explicit list( size_type count );
  //******某个范围的值为初始值的构造函数**************
	template< class InputIt >
	list( InputIt first, InputIt last,
      const Allocator& alloc = Allocator() );
 //******拷贝构造函数*************************
	list( const list& other );
  */
  //************************************
  //构造函数
  //链表的默认构造函数
  explicit list(const allocator_type& __a = allocator_type()) : _Base(__a) {}
  list(size_type __n, const _Tp& __value,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { insert(begin(), __n, __value); }
  explicit list(size_type __n)
    : _Base(allocator_type())
    { insert(begin(), __n, _Tp()); }

#ifdef __STL_MEMBER_TEMPLATES

  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.  
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { insert(begin(), __first, __last); }

#else /* __STL_MEMBER_TEMPLATES */

  list(const _Tp* __first, const _Tp* __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { this->insert(begin(), __first, __last); }
  list(const_iterator __first, const_iterator __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { this->insert(begin(), __first, __last); }

#endif /* __STL_MEMBER_TEMPLATES */
  list(const list<_Tp, _Alloc>& __x) : _Base(__x.get_allocator())
    { insert(begin(), __x.begin(), __x.end()); }//拷贝构造函数

  ~list() { }//析构函数

  //赋值操作
  list<_Tp, _Alloc>& operator=(const list<_Tp, _Alloc>& __x);
  //构造函数，析构函数，赋值操作 定义到此结束
  //***********************************
	...
};
```

### list容器的成员函数

list容器的成员函数为我们使用该容器提供了很大的帮助，所以这里对其进行讲解，首先先给出源码的剖析，然后在对其中一些重点的成员函数进行图文讲解；具体源码剖析如下所示：

```c++
//以下是双向链表list类的定义，分配器_Alloc默认为第二级配置器
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class list : protected _List_base<_Tp, _Alloc> {
  // requirements:

	...

protected:
	//创建值为x的节点，并返回该节点的地址
  _Node* _M_create_node(const _Tp& __x)
  {
    _Node* __p = _M_get_node();//分配一个节点空间
    __STL_TRY {//把x值赋予指定的地址，即是data值
      _Construct(&__p->_M_data, __x);
    }
    __STL_UNWIND(_M_put_node(__p));
    return __p;//返回节点地址
  }

  //创建默认值的节点
  _Node* _M_create_node()
  {
    _Node* __p = _M_get_node();
    __STL_TRY {
      _Construct(&__p->_M_data);
    }
    __STL_UNWIND(_M_put_node(__p));
    return __p;
  }

public:
	

  //以下是迭代器的定义
  iterator begin()             { return (_Node*)(_M_node->_M_next); }
  const_iterator begin() const { return (_Node*)(_M_node->_M_next); }

  iterator end()             { return _M_node; }
  const_iterator end() const { return _M_node; }

  reverse_iterator rbegin() 
    { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(end()); }

  reverse_iterator rend()
    { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }

  //判断链表是否为空链表
  bool empty() const { return _M_node->_M_next == _M_node; }
 
  //返回链表的大小
  size_type size() const {
    size_type __result = 0;
	//返回两个迭代器之间的距离
    distance(begin(), end(), __result);
	//返回链表的元素个数
    return __result;
  }
  size_type max_size() const { return size_type(-1); }

  //返回第一个节点数据的引用，reference相当于value_type&
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  //返回最后一个节点数据的引用
  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  //交换链表容器的内容
  void swap(list<_Tp, _Alloc>& __x) { __STD::swap(_M_node, __x._M_node); }

 //************************************
 //***********插入节点*********************
  /**********以下是插入节点函数的原型，也是公共接口********
	//在指定的位置pos之前插入值为value的数据节点
	iterator insert( iterator pos, const T& value );
	iterator insert( const_iterator pos, const T& value );

	//在指定的位置pos之前插入n个值为value的数据节点
	void insert( iterator pos, size_type count, const T& value );
	iterator insert( const_iterator pos, size_type count, const T& value );

	//在指定的位置pos之前插入[first,last)之间的数据节点
	template< class InputIt >
	void insert( iterator pos, InputIt first, InputIt last);
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last );
  *************************************/
  /**在整个链表的操作中，插入操作是非常重要的，很多成员函数会调用该函数**/
//*************************************
  //在指定的位置插入初始值为x的节点
  iterator insert(iterator __position, const _Tp& __x) {
	  //首先创建一个初始值为x的节点，并返回该节点的地址
    _Node* __tmp = _M_create_node(__x);
	//调整节点指针，把新节点插入到指定位置
    __tmp->_M_next = __position._M_node;
    __tmp->_M_prev = __position._M_node->_M_prev;
    __position._M_node->_M_prev->_M_next = __tmp;
    __position._M_node->_M_prev = __tmp;
	//返回新节点地址
    return __tmp;
  }
  //在指定的位置插入为默认值的节点
  iterator insert(iterator __position) { return insert(__position, _Tp()); }

  //在指定位置插入n个初始值为x的节点
  void insert(iterator __pos, size_type __n, const _Tp& __x)
    { _M_fill_insert(__pos, __n, __x); }
  void _M_fill_insert(iterator __pos, size_type __n, const _Tp& __x); 

#ifdef __STL_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.
  //这里采用__type_traits技术
 
   //在指定位置插入指定范围内的数据
  //首先判断输入迭代器类型_InputIterator是否为整数类型
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__pos, __first, __last, _Integral());
  }

  
  //若输入迭代器类型_InputIterator是为整数类型，调用此函数
  template<class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                          __true_type) {
    _M_fill_insert(__pos, (size_type) __n, (_Tp) __x);
  }

  //若输入迭代器类型_InputIterator是不为整数类型，调用此函数
  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,
                          _InputIterator __first, _InputIterator __last,
                          __false_type);

 #else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator __position, const _Tp* __first, const _Tp* __last);
  void insert(iterator __position,
              const_iterator __first, const_iterator __last);
#endif /* __STL_MEMBER_TEMPLATES */
  
  //在链表头插入节点
  void push_front(const _Tp& __x) { insert(begin(), __x); }
  void push_front() {insert(begin());}
  //在链表尾插入节点
  void push_back(const _Tp& __x) { insert(end(), __x); }
  void push_back() {insert(end());}

  //*******************************
  //**********在指定位置删除节点***********
  //**********以下是删除节点的公共接口*********
  /******************************
	//删除指定位置pos的节点
	iterator erase( iterator pos );
	iterator erase( const_iterator pos );

	//删除指定范围[first,last)的数据节点
	iterator erase( iterator first, iterator last );
	iterator erase( const_iterator first, const_iterator last );
  ******************************/
  //*******************************
  //在指定位置position删除节点，并返回直接后继节点的地址
  iterator erase(iterator __position) {
	  //调整前驱和后继节点的位置
    _List_node_base* __next_node = __position._M_node->_M_next;
    _List_node_base* __prev_node = __position._M_node->_M_prev;
    _Node* __n = (_Node*) __position._M_node;
    __prev_node->_M_next = __next_node;
    __next_node->_M_prev = __prev_node;
    _Destroy(&__n->_M_data);
    _M_put_node(__n);
    return iterator((_Node*) __next_node);
  }
  //删除两个迭代器之间的节点
  iterator erase(iterator __first, iterator __last);
  //清空链表，这里是调用父类的clear()函数
  void clear() { _Base::clear(); }

  //调整链表的大小
  void resize(size_type __new_size, const _Tp& __x);
  void resize(size_type __new_size) { this->resize(__new_size, _Tp()); }

  //取出第一个数据节点
  void pop_front() { erase(begin()); }
  //取出最后一个数据节点
  void pop_back() { 
    iterator __tmp = end();
    erase(--__tmp);
  }

public:
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
  /***********************************
  //assign()函数的两个版本原型，功能是在已定义的list容器填充值
	void assign( size_type count, const T& value );

	template< class InputIt >
	void assign( InputIt first, InputIt last );
  //***********************************
	例子：
	#include <list>
	#include <iostream>
 
	int main()
	{
		std::list<char> characters;
		//若定义characters时并初始化为字符b，下面的填充操作一样有效
		//std::list<char>characters(5,'b')
 
		characters.assign(5, 'a');
 
		for (char c : characters) {
			std::cout << c << ' ';
		}
 
		return 0;
	}
	输出结果：a a a a a
  ***********************************/
	//这里是第一个版本void assign( size_type count, const T& value );
  void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }

  //这里为什么要把_M_fill_assign这个函数放在public呢？？保护起来不是更好吗？？
  void _M_fill_assign(size_type __n, const _Tp& __val);

#ifdef __STL_MEMBER_TEMPLATES

  //以下是针对assign()函数的第二个版本
  /*
	template< class InputIt >
	void assign( InputIt first, InputIt last );
	这里有偏特化的现象，判断输入数据类型是否为整数型别
  */
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  //若输入数据类型为整数型别，则派送到此函数
  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Tp) __val); }

  //若输入数据类型不是整数型别，则派送到此函数
  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                          __false_type);

#endif /* __STL_MEMBER_TEMPLATES */
  //assign()函数定义结束
  //*********************************

protected:
	//把区间[first,last）的节点数据插入到指定节点position之前,position不能在区间内部
	//这个函数是list类的protected属性，不是公共接口，只为list类成员服务
	//为下面拼接函数void splice()服务
  void transfer(iterator __position, iterator __first, iterator __last) {
    if (__position != __last) {
      // Remove [first, last) from its old position.
      __last._M_node->_M_prev->_M_next     = __position._M_node;
      __first._M_node->_M_prev->_M_next    = __last._M_node;
      __position._M_node->_M_prev->_M_next = __first._M_node; 

      // Splice [first, last) into its new position.
      _List_node_base* __tmp      = __position._M_node->_M_prev;
      __position._M_node->_M_prev = __last._M_node->_M_prev;
      __last._M_node->_M_prev     = __first._M_node->_M_prev; 
      __first._M_node->_M_prev    = __tmp;
    }
  }

public:
	//******************************
	//***********拼接操作对外接口*************
	//把链表拼接到当前链表指定位置position之前
	/*void splice(const_iterator pos, list& other);
	
	//把it在链表other所指的位置拼接到当前链表pos之前，it和pos可指向同一链表
	void splice(const_iterator pos, list& other, const_iterator it);

	//把链表other的节点范围[first,last)拼接在当前链表所指定的位置pos之前
	//[first,last)和pos可指向同一链表
	void splice(const_iterator pos, list& other,
            const_iterator first, const_iterator last);
	*******************************/
	//******************************
	//将链表x拼接到当前链表的指定位置position之前
	//这里x和*this必须不同，即是两个不同的链表
  void splice(iterator __position, list& __x) {
    if (!__x.empty()) 
      this->transfer(__position, __x.begin(), __x.end());
  }
  //将i所指向的节点拼接到position所指位置之前
  //注意：i和position可以指向同一个链表
  void splice(iterator __position, list&, iterator __i) {
    iterator __j = __i;
    ++__j;
	//若i和position指向同一个链表，且指向同一位置
	//或者i和position指向同一个链表，且就在position的直接前驱位置
	//针对以上这两种情况，不做任何操作
    if (__position == __i || __position == __j) return;
	//否则，进行拼接操作
    this->transfer(__position, __i, __j);
  }
  //将范围[first,last)内所有节点拼接到position所指位置之前
  //注意：[first,last)和position可指向同一个链表，
  //但是position不能在[first,last)范围之内
  void splice(iterator __position, list&, iterator __first, iterator __last) {
    if (__first != __last) 
      this->transfer(__position, __first, __last);
  }
  //以下是成员函数声明，定义在list类外实现
  //******************************
  //删除链表中值等于value的所有节点
  void remove(const _Tp& __value);
  //删除连续重复的元素节点，使之唯一
  //注意：是连续的重复元素
  void unique();
  //合并两个已排序的链表
  void merge(list& __x);
  //反转链表容器的内容
  void reverse();
  //按升序排序链表内容
  void sort();

#ifdef __STL_MEMBER_TEMPLATES
  template <class _Predicate> void remove_if(_Predicate);
  template <class _BinaryPredicate> void unique(_BinaryPredicate);
  template <class _StrictWeakOrdering> void merge(list&, _StrictWeakOrdering);
  template <class _StrictWeakOrdering> void sort(_StrictWeakOrdering);
#endif /* __STL_MEMBER_TEMPLATES */
};
```

在list容器的成员函数中最重要的几个成员函数是插入insert()、擦除erase()、拼接splice()和排序sort()函数；以下利用图文的形式对其进行讲解；首先对插入节点函数进行insert()分析：

下面的插入函数是在指定的位置插入初始值为value的节点，具体实现见下面源码剖析：

```c++
 //***********插入节点*********************
  /**********以下是插入节点函数的原型，也是公共接口********
	//在指定的位置pos之前插入值为value的数据节点
	iterator insert( iterator pos, const T& value );
	iterator insert( const_iterator pos, const T& value );

	//在指定的位置pos之前插入n个值为value的数据节点
	void insert( iterator pos, size_type count, const T& value );
	iterator insert( const_iterator pos, size_type count, const T& value );

	//在指定的位置pos之前插入[first,last)之间的数据节点
	template< class InputIt >
	void insert( iterator pos, InputIt first, InputIt last);
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last );
  *************************************/
  /**在整个链表的操作中，插入操作是非常重要的，很多成员函数会调用该函数**/
//*************************************
  //在指定的位置插入初始值为x的节点
  iterator insert(iterator __position, const _Tp& __x) {
	  //首先创建一个初始值为x的节点，并返回该节点的地址
    _Node* __tmp = _M_create_node(__x);
	//调整节点指针，把新节点插入到指定位置
    __tmp->_M_next = __position._M_node;
    __tmp->_M_prev = __position._M_node->_M_prev;
    __position._M_node->_M_prev->_M_next = __tmp;
    __position._M_node->_M_prev = __tmp;
	//返回新节点地址
    return __tmp;
  }
```

下面举一个例子对插入函数insert()进行图文分析：假设在以下list链表中节点5之前插入节点9，具体实现见下图步骤：注：图中的箭头旁边的数字表示语句的执行步骤

第一步：首先初始化节点9，并为其分配节点空间；

第二步：调整节点5指针方向，使其与节点9连接；

第三步：调整节点5的前驱结点7指针的方向，使其与节点9连接；

![img](https://box.kancloud.cn/2016-07-12_5784b878e8de1.jpg)

以下分析的是擦除指定位置的节点，详细见源码剖析：

```c++
 //*******************************
  //**********在指定位置删除节点***********
  //**********以下是删除节点的公共接口*********
  /******************************
	//删除指定位置pos的节点
	iterator erase( iterator pos );
	iterator erase( const_iterator pos );

	//删除指定范围[first,last)的数据节点
	iterator erase( iterator first, iterator last );
	iterator erase( const_iterator first, const_iterator last );
  ******************************/
  //*******************************
  //在指定位置position删除节点，并返回直接后继节点的地址
  iterator erase(iterator __position) {
	  //调整前驱和后继节点的位置
    _List_node_base* __next_node = __position._M_node->_M_next;
    _List_node_base* __prev_node = __position._M_node->_M_prev;
    _Node* __n = (_Node*) __position._M_node;
    __prev_node->_M_next = __next_node;
    __next_node->_M_prev = __prev_node;
    _Destroy(&__n->_M_data);
    _M_put_node(__n);
    return iterator((_Node*) __next_node);
  }
```

下面举一个例子对擦除函数erase()进行图文分析：假设在以下list链表中删除节点5，具体实现见下图步骤：图中的箭头旁边的数字表示语句的执行步骤

第一步：首先调整待删除节点直接前驱指针，使其指向待删除节点的直接后继节点；

第二步：调整待删除节点直接后继指针方向，使其指向待删除节点的直接前驱节点；

第三步：释放待删除节点对象，回收待删除节点内存空；

![img](https://box.kancloud.cn/2016-07-12_5784b87912ac4.jpg)

以下对迁移操作transfer()进行分析，该函数不是公共接口，属于list容器的保护成员函数，但是它为拼接函数服务，拼接函数的核心就是迁移函数;transfer()和splice()函数源代码剖析如下：

```c++
protected:
	//把区间[first,last）的节点数据插入到指定节点position之前,position不能在区间内部
	//这个函数是list类的protected属性，不是公共接口，只为list类成员服务
	//为下面拼接函数void splice()服务
  void transfer(iterator __position, iterator __first, iterator __last) {
    if (__position != __last) {
      // Remove [first, last) from its old position.
      __last._M_node->_M_prev->_M_next     = __position._M_node;
      __first._M_node->_M_prev->_M_next    = __last._M_node;
      __position._M_node->_M_prev->_M_next = __first._M_node; 

      // Splice [first, last) into its new position.
      _List_node_base* __tmp      = __position._M_node->_M_prev;
      __position._M_node->_M_prev = __last._M_node->_M_prev;
      __last._M_node->_M_prev     = __first._M_node->_M_prev; 
      __first._M_node->_M_prev    = __tmp;
    }
  }

public:
	//******************************
	//***********拼接操作对外接口*************
	//把链表拼接到当前链表指定位置position之前
	/*void splice(const_iterator pos, list& other);
	
	//把it在链表other所指的位置拼接到当前链表pos之前，it和pos可指向同一链表
	void splice(const_iterator pos, list& other, const_iterator it);

	//把链表other的节点范围[first,last)拼接在当前链表所指定的位置pos之前
	//[first,last)和pos可指向同一链表
	void splice(const_iterator pos, list& other,
            const_iterator first, const_iterator last);
	*******************************/
	//******************************
	//将链表x拼接到当前链表的指定位置position之前
	//这里x和*this必须不同，即是两个不同的链表
  void splice(iterator __position, list& __x) {
    if (!__x.empty()) 
      this->transfer(__position, __x.begin(), __x.end());
  }
  //将i所指向的节点拼接到position所指位置之前
  //注意：i和position可以指向同一个链表
  void splice(iterator __position, list&, iterator __i) {
    iterator __j = __i;
    ++__j;
	//若i和position指向同一个链表，且指向同一位置
	//或者i和position指向同一个链表，且就在position的直接前驱位置
	//针对以上这两种情况，不做任何操作
    if (__position == __i || __position == __j) return;
	//否则，进行拼接操作
    this->transfer(__position, __i, __j);
  }
  //将范围[first,last)内所有节点拼接到position所指位置之前
  //注意：[first,last)和position可指向同一个链表，
  //但是position不能在[first,last)范围之内
  void splice(iterator __position, list&, iterator __first, iterator __last) {
    if (__first != __last) 
      this->transfer(__position, __first, __last);
  }
```

下面用图文对该函数进行分析：注：transfer函数中的每一条语句按顺序对应图中执行步骤；

下图是执行第一过程Remove[first, last) from its old position流图：

![img](https://box.kancloud.cn/2016-07-12_5784b87931223.jpg)

下图是执行第二过程Splice [first, last) into its new position流图：

![img](https://box.kancloud.cn/2016-07-12_5784b8795207f.jpg)

关于list容器的排序算法sort前面博文已经单独对其进行讲解，需要了解的请往前面博文[《STL源码剖析——list容器的排序算法sort()》](http://blog.csdn.net/chenhanzhun/article/details/39337331)了解；

### list容器的操作符重载

关于操作符重载具体看源码剖析：

```
//********************************
//*********以下是比较运算符操作符重载***********
//********************************
template <class _Tp, class _Alloc>
inline bool 
operator==(const list<_Tp,_Alloc>& __x, const list<_Tp,_Alloc>& __y)
{
  typedef typename list<_Tp,_Alloc>::const_iterator const_iterator;
  const_iterator __end1 = __x.end();
  const_iterator __end2 = __y.end();

  const_iterator __i1 = __x.begin();
  const_iterator __i2 = __y.begin();
  while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2) {
    ++__i1;
    ++__i2;
  }
  return __i1 == __end1 && __i2 == __end2;
}

template <class _Tp, class _Alloc>
inline bool operator<(const list<_Tp,_Alloc>& __x,
                      const list<_Tp,_Alloc>& __y)
{
  return lexicographical_compare(__x.begin(), __x.end(),
                                 __y.begin(), __y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline bool operator!=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool operator>(const list<_Tp,_Alloc>& __x,
                      const list<_Tp,_Alloc>& __y) {
  return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool operator<=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Tp, class _Alloc>
inline bool operator>=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x < __y);
}

//交换两个链表内容
template <class _Tp, class _Alloc>
inline void 
swap(list<_Tp, _Alloc>& __x, list<_Tp, _Alloc>& __y)
{
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */
//操作符重载结束
//********************************
```

### list容器完整源码剖析

list容器完成源码剖析：

```c++
//以下是list链表节点的数据结构
struct _List_node_base {
  _List_node_base* _M_next;//指向直接后继节点
  _List_node_base* _M_prev;//指向直接前驱节点
};

template <class _Tp>
struct _List_node : public _List_node_base {
  _Tp _M_data;//节点存储的数据
};

//以下是链表List_iterator_base的迭代器
struct _List_iterator_base {
	//数据类型
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  //list迭代器的类型是双向迭代器bidirectional_iterator
  typedef bidirectional_iterator_tag iterator_category;

  //定义指向链表节点的指针
  _List_node_base* _M_node;

  //构造函数
  _List_iterator_base(_List_node_base* __x) : _M_node(__x) {}
  _List_iterator_base() {}

  //更新节点指针，指向直接前驱或直接后继节点
  void _M_incr() { _M_node = _M_node->_M_next; }
  void _M_decr() { _M_node = _M_node->_M_prev; }

  //操作符重载
  bool operator==(const _List_iterator_base& __x) const {
    return _M_node == __x._M_node;
  }
  bool operator!=(const _List_iterator_base& __x) const {
    return _M_node != __x._M_node;
  }
};  

//以下是链表List_iterator的迭代器
template<class _Tp, class _Ref, class _Ptr>
struct _List_iterator : public _List_iterator_base {
  typedef _List_iterator<_Tp,_Tp&,_Tp*>             iterator;
  typedef _List_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;
  typedef _List_iterator<_Tp,_Ref,_Ptr>             _Self;

  typedef _Tp value_type;
  typedef _Ptr pointer;
  typedef _Ref reference;
  typedef _List_node<_Tp> _Node;

  //构造函数
  _List_iterator(_Node* __x) : _List_iterator_base(__x) {}
  _List_iterator() {}
  _List_iterator(const iterator& __x) : _List_iterator_base(__x._M_node) {}

  //以下都是基本操作符的重载,取出节点数据
  reference operator*() const { return ((_Node*) _M_node)->_M_data; }

#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  _Self& operator++() { 
    this->_M_incr();
    return *this;
  }
  _Self operator++(int) { 
    _Self __tmp = *this;
    this->_M_incr();
    return __tmp;
  }
  _Self& operator--() { 
    this->_M_decr();
    return *this;
  }
  _Self operator--(int) { 
    _Self __tmp = *this;
    this->_M_decr();
    return __tmp;
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

//返回迭代器的类型
inline bidirectional_iterator_tag
iterator_category(const _List_iterator_base&)
{
  return bidirectional_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr>
inline _Tp*
value_type(const _List_iterator<_Tp, _Ref, _Ptr>&)
{
  return 0;
}

inline ptrdiff_t*
distance_type(const _List_iterator_base&)
{
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Base class that encapsulates details of allocators.  Three cases:
// an ordinary standard-conforming allocator, a standard-conforming
// allocator with no non-static data, and an SGI-style allocator.
// This complexity is necessary only because we're worrying about backward
// compatibility and because we want to avoid wasting storage on an 
// allocator instance if it isn't necessary.

#ifdef __STL_USE_STD_ALLOCATORS

// Base for general standard-conforming allocators.
template <class _Tp, class _Allocator, bool _IsStatic>
class _List_alloc_base {
public:
  typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type
          allocator_type;//返回节点配置器
  allocator_type get_allocator() const { return _Node_allocator; }

  _List_alloc_base(const allocator_type& __a) : _Node_allocator(__a) {}

protected:
  _List_node<_Tp>* _M_get_node()
   { return _Node_allocator.allocate(1); }
  void _M_put_node(_List_node<_Tp>* __p)
    { _Node_allocator.deallocate(__p, 1); }

protected:
  typename _Alloc_traits<_List_node<_Tp>, _Allocator>::allocator_type
           _Node_allocator;
  _List_node<_Tp>* _M_node;
};

// Specialization for instanceless allocators.
//instanceless分配器偏特化版
template <class _Tp, class _Allocator>
class _List_alloc_base<_Tp, _Allocator, true> {
public:
	//定义分配器类型
  typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type
          allocator_type;
  //返回节点配置器
  allocator_type get_allocator() const { return allocator_type(); }

  //构造函数
  _List_alloc_base(const allocator_type&) {}

protected:
  typedef typename _Alloc_traits<_List_node<_Tp>, _Allocator>::_Alloc_type
          _Alloc_type;
  //分配一个节点空间
  _List_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
  //回收一个节点空间
  void _M_put_node(_List_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); }

protected:
	//定义节点指针
  _List_node<_Tp>* _M_node;
};

template <class _Tp, class _Alloc>
class _List_base 
  : public _List_alloc_base<_Tp, _Alloc,
                            _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
public:
  typedef _List_alloc_base<_Tp, _Alloc,
                           _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
          _Base; 
  //allocator_type迭代器类型
  typedef typename _Base::allocator_type allocator_type;

 //构造函数
  _List_base(const allocator_type& __a) : _Base(__a) {
    _M_node = _M_get_node();//分配一个节点空间
    _M_node->_M_next = _M_node;//
    _M_node->_M_prev = _M_node;
  }
  //析构函数
  ~_List_base() {
    clear();//清空链表
    _M_put_node(_M_node);//回收一个节点内存空间
  }

  void clear();//清空链表
};

#else /* __STL_USE_STD_ALLOCATORS */

template <class _Tp, class _Alloc>
class _List_base 
{
public:
  typedef _Alloc allocator_type;//获得分配器类型
  allocator_type get_allocator() const { return allocator_type(); }

  //构造函数
  _List_base(const allocator_type&) {
    _M_node = _M_get_node();//分配一个节点空间
	//节点前驱和后继指针指向自己，表示是一个空链表
    _M_node->_M_next = _M_node;
    _M_node->_M_prev = _M_node;
  }
  //析构函数
  ~_List_base() {
    clear();//清空链表
    _M_put_node(_M_node);//回收一个节点内存空间
  }

  void clear();//清空链表

protected:
	//迭代器类型
  typedef simple_alloc<_List_node<_Tp>, _Alloc> _Alloc_type;
  //分配一个节点内存空间
  _List_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
  //回收一个节点内存空间
  void _M_put_node(_List_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); } 

protected:
  _List_node<_Tp>* _M_node;//链表的节点指针
};

#endif /* __STL_USE_STD_ALLOCATORS */

//clear()函数的实现，即清空链表
template <class _Tp, class _Alloc>
void 
_List_base<_Tp,_Alloc>::clear() 
{
 //选取_M_node->_M_next作为当前节点
 _List_node<_Tp>* __cur = (_List_node<_Tp>*) _M_node->_M_next;
  while (__cur != _M_node) {//遍历每一个节点
    _List_node<_Tp>* __tmp = __cur;//设置一个节点临时别名
    __cur = (_List_node<_Tp>*) __cur->_M_next;//指向下一个节点
    _Destroy(&__tmp->_M_data);//析构数据对象
    _M_put_node(__tmp);//回收节点tmp指向的内存空间
  }
  //空链表,即前驱和后继指针都指向自己
  _M_node->_M_next = _M_node;
  _M_node->_M_prev = _M_node;
}

//以下是双向链表list类的定义，分配器_Alloc默认为第二级配置器
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class list : protected _List_base<_Tp, _Alloc> {
  // requirements:

  __STL_CLASS_REQUIRES(_Tp, _Assignable);

  typedef _List_base<_Tp, _Alloc> _Base;
protected:
  typedef void* _Void_pointer;//定义指针类型

public: //以下是内嵌型别
  typedef _Tp value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef _List_node<_Tp> _Node;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef typename _Base::allocator_type allocator_type;//分配器类型
  allocator_type get_allocator() const { return _Base::get_allocator(); }

public:
	//迭代器的类型
  typedef _List_iterator<_Tp,_Tp&,_Tp*>             iterator;
  typedef _List_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator>       reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_bidirectional_iterator<const_iterator,value_type,
                                         const_reference,difference_type>
          const_reverse_iterator;
  typedef reverse_bidirectional_iterator<iterator,value_type,reference,
                                         difference_type>
          reverse_iterator; 
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
#ifdef __STL_HAS_NAMESPACES
  using _Base::_M_node;
  using _Base::_M_put_node;
  using _Base::_M_get_node;
#endif /* __STL_HAS_NAMESPACES */

protected:
	//创建值为x的节点，并返回该节点的地址
  _Node* _M_create_node(const _Tp& __x)
  {
    _Node* __p = _M_get_node();//分配一个节点空间
    __STL_TRY {//把x值赋予指定的地址，即是data值
      _Construct(&__p->_M_data, __x);
    }
    __STL_UNWIND(_M_put_node(__p));
    return __p;//返回节点地址
  }

  //创建默认值的节点
  _Node* _M_create_node()
  {
    _Node* __p = _M_get_node();
    __STL_TRY {
      _Construct(&__p->_M_data);
    }
    __STL_UNWIND(_M_put_node(__p));
    return __p;
  }

public:
	

  //以下是迭代器的定义
  iterator begin()             { return (_Node*)(_M_node->_M_next); }
  const_iterator begin() const { return (_Node*)(_M_node->_M_next); }

  iterator end()             { return _M_node; }
  const_iterator end() const { return _M_node; }

  reverse_iterator rbegin() 
    { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(end()); }

  reverse_iterator rend()
    { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }

  //判断链表是否为空链表
  bool empty() const { return _M_node->_M_next == _M_node; }
 
  //返回链表的大小
  size_type size() const {
    size_type __result = 0;
	//返回两个迭代器之间的距离
    distance(begin(), end(), __result);
	//返回链表的元素个数
    return __result;
  }
  size_type max_size() const { return size_type(-1); }

  //返回第一个节点数据的引用，reference相当于value_type&
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  //返回最后一个节点数据的引用
  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  //交换链表容器的内容
  void swap(list<_Tp, _Alloc>& __x) { __STD::swap(_M_node, __x._M_node); }

 //************************************
 //***********插入节点*********************
  /**********以下是插入节点函数的原型，也是公共接口********
	//在指定的位置pos之前插入值为value的数据节点
	iterator insert( iterator pos, const T& value );
	iterator insert( const_iterator pos, const T& value );

	//在指定的位置pos之前插入n个值为value的数据节点
	void insert( iterator pos, size_type count, const T& value );
	iterator insert( const_iterator pos, size_type count, const T& value );

	//在指定的位置pos之前插入[first,last)之间的数据节点
	template< class InputIt >
	void insert( iterator pos, InputIt first, InputIt last);
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last );
  *************************************/
  /**在整个链表的操作中，插入操作是非常重要的，很多成员函数会调用该函数**/
//*************************************
  //在指定的位置插入初始值为x的节点
  iterator insert(iterator __position, const _Tp& __x) {
	  //首先创建一个初始值为x的节点，并返回该节点的地址
    _Node* __tmp = _M_create_node(__x);
	//调整节点指针，把新节点插入到指定位置
    __tmp->_M_next = __position._M_node;
    __tmp->_M_prev = __position._M_node->_M_prev;
    __position._M_node->_M_prev->_M_next = __tmp;
    __position._M_node->_M_prev = __tmp;
	//返回新节点地址
    return __tmp;
  }
  //在指定的位置插入为默认值的节点
  iterator insert(iterator __position) { return insert(__position, _Tp()); }

  //在指定位置插入n个初始值为x的节点
  void insert(iterator __pos, size_type __n, const _Tp& __x)
    { _M_fill_insert(__pos, __n, __x); }
  void _M_fill_insert(iterator __pos, size_type __n, const _Tp& __x); 

#ifdef __STL_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.
  //这里采用__type_traits技术
 
   //在指定位置插入指定范围内的数据
  //首先判断输入迭代器类型_InputIterator是否为整数类型
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__pos, __first, __last, _Integral());
  }

  
  //若输入迭代器类型_InputIterator是为整数类型，调用此函数
  template<class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                          __true_type) {
    _M_fill_insert(__pos, (size_type) __n, (_Tp) __x);
  }

  //若输入迭代器类型_InputIterator是不为整数类型，调用此函数
  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,
                          _InputIterator __first, _InputIterator __last,
                          __false_type);

 #else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator __position, const _Tp* __first, const _Tp* __last);
  void insert(iterator __position,
              const_iterator __first, const_iterator __last);
#endif /* __STL_MEMBER_TEMPLATES */
  
  //在链表头插入节点
  void push_front(const _Tp& __x) { insert(begin(), __x); }
  void push_front() {insert(begin());}
  //在链表尾插入节点
  void push_back(const _Tp& __x) { insert(end(), __x); }
  void push_back() {insert(end());}

  //*******************************
  //**********在指定位置删除节点***********
  //**********以下是删除节点的公共接口*********
  /******************************
	//删除指定位置pos的节点
	iterator erase( iterator pos );
	iterator erase( const_iterator pos );

	//删除指定范围[first,last)的数据节点
	iterator erase( iterator first, iterator last );
	iterator erase( const_iterator first, const_iterator last );
  ******************************/
  //*******************************
  //在指定位置position删除节点，并返回直接后继节点的地址
  iterator erase(iterator __position) {
	  //调整前驱和后继节点的位置
    _List_node_base* __next_node = __position._M_node->_M_next;
    _List_node_base* __prev_node = __position._M_node->_M_prev;
    _Node* __n = (_Node*) __position._M_node;
    __prev_node->_M_next = __next_node;
    __next_node->_M_prev = __prev_node;
    _Destroy(&__n->_M_data);
    _M_put_node(__n);
    return iterator((_Node*) __next_node);
  }
  //删除两个迭代器之间的节点
  iterator erase(iterator __first, iterator __last);
  //清空链表，这里是调用父类的clear()函数
  void clear() { _Base::clear(); }

  //调整链表的大小
  void resize(size_type __new_size, const _Tp& __x);
  void resize(size_type __new_size) { this->resize(__new_size, _Tp()); }

  //取出第一个数据节点
  void pop_front() { erase(begin()); }
  //取出最后一个数据节点
  void pop_back() { 
    iterator __tmp = end();
    erase(--__tmp);
  }

  //************************************
  /*************以下是构造函数******************
  //***********默认构造函数*********************
	explicit list( const Allocator& alloc = Allocator() );
  //************具有初值和大小的构造函数************
	explicit list( size_type count,
               const T& value = T(),
               const Allocator& alloc = Allocator());
         list( size_type count,
               const T& value,
               const Allocator& alloc = Allocator());
  //********只有大小的构造函数********************
	explicit list( size_type count );
  //******某个范围的值为初始值的构造函数**************
	template< class InputIt >
	list( InputIt first, InputIt last,
      const Allocator& alloc = Allocator() );
 //******拷贝构造函数*************************
	list( const list& other );
  */
  //************************************
  //构造函数
  //链表的默认构造函数
  explicit list(const allocator_type& __a = allocator_type()) : _Base(__a) {}
  list(size_type __n, const _Tp& __value,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { insert(begin(), __n, __value); }
  explicit list(size_type __n)
    : _Base(allocator_type())
    { insert(begin(), __n, _Tp()); }

#ifdef __STL_MEMBER_TEMPLATES

  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.  
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { insert(begin(), __first, __last); }

#else /* __STL_MEMBER_TEMPLATES */

  list(const _Tp* __first, const _Tp* __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { this->insert(begin(), __first, __last); }
  list(const_iterator __first, const_iterator __last,
       const allocator_type& __a = allocator_type())
    : _Base(__a)
    { this->insert(begin(), __first, __last); }

#endif /* __STL_MEMBER_TEMPLATES */
  list(const list<_Tp, _Alloc>& __x) : _Base(__x.get_allocator())
    { insert(begin(), __x.begin(), __x.end()); }//拷贝构造函数

  ~list() { }//析构函数

  //赋值操作
  list<_Tp, _Alloc>& operator=(const list<_Tp, _Alloc>& __x);
  //构造函数，析构函数，赋值操作 定义到此结束
  //***********************************

public:
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
  /***********************************
  //assign()函数的两个版本原型，功能是在已定义的list容器填充值
	void assign( size_type count, const T& value );

	template< class InputIt >
	void assign( InputIt first, InputIt last );
  //***********************************
	例子：
	#include <list>
	#include <iostream>
 
	int main()
	{
		std::list<char> characters;
		//若定义characters时并初始化为字符b，下面的填充操作一样有效
		//std::list<char>characters(5,'b')
 
		characters.assign(5, 'a');
 
		for (char c : characters) {
			std::cout << c << ' ';
		}
 
		return 0;
	}
	输出结果：a a a a a
  ***********************************/
	//这里是第一个版本void assign( size_type count, const T& value );
  void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }

  //这里为什么要把_M_fill_assign这个函数放在public呢？？保护起来不是更好吗？？
  void _M_fill_assign(size_type __n, const _Tp& __val);

#ifdef __STL_MEMBER_TEMPLATES

  //以下是针对assign()函数的第二个版本
  /*
	template< class InputIt >
	void assign( InputIt first, InputIt last );
	这里有偏特化的现象，判断输入数据类型是否为整数型别
  */
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  //若输入数据类型为整数型别，则派送到此函数
  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Tp) __val); }

  //若输入数据类型不是整数型别，则派送到此函数
  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                          __false_type);

#endif /* __STL_MEMBER_TEMPLATES */
  //assign()函数定义结束
  //*********************************

protected:
	//把区间[first,last）的节点数据插入到指定节点position之前,position不能在区间内部
	//这个函数是list类的protected属性，不是公共接口，只为list类成员服务
	//为下面拼接函数void splice()服务
  void transfer(iterator __position, iterator __first, iterator __last) {
    if (__position != __last) {
      // Remove [first, last) from its old position.
      __last._M_node->_M_prev->_M_next     = __position._M_node;
      __first._M_node->_M_prev->_M_next    = __last._M_node;
      __position._M_node->_M_prev->_M_next = __first._M_node; 

      // Splice [first, last) into its new position.
      _List_node_base* __tmp      = __position._M_node->_M_prev;
      __position._M_node->_M_prev = __last._M_node->_M_prev;
      __last._M_node->_M_prev     = __first._M_node->_M_prev; 
      __first._M_node->_M_prev    = __tmp;
    }
  }

public:
	//******************************
	//***********拼接操作对外接口*************
	//把链表拼接到当前链表指定位置position之前
	/*void splice(const_iterator pos, list& other);
	
	//把it在链表other所指的位置拼接到当前链表pos之前，it和pos可指向同一链表
	void splice(const_iterator pos, list& other, const_iterator it);

	//把链表other的节点范围[first,last)拼接在当前链表所指定的位置pos之前
	//[first,last)和pos可指向同一链表
	void splice(const_iterator pos, list& other,
            const_iterator first, const_iterator last);
	*******************************/
	//******************************
	//将链表x拼接到当前链表的指定位置position之前
	//这里x和*this必须不同，即是两个不同的链表
  void splice(iterator __position, list& __x) {
    if (!__x.empty()) 
      this->transfer(__position, __x.begin(), __x.end());
  }
  //将i所指向的节点拼接到position所指位置之前
  //注意：i和position可以指向同一个链表
  void splice(iterator __position, list&, iterator __i) {
    iterator __j = __i;
    ++__j;
	//若i和position指向同一个链表，且指向同一位置
	//或者i和position指向同一个链表，且就在position的直接前驱位置
	//针对以上这两种情况，不做任何操作
    if (__position == __i || __position == __j) return;
	//否则，进行拼接操作
    this->transfer(__position, __i, __j);
  }
  //将范围[first,last)内所有节点拼接到position所指位置之前
  //注意：[first,last)和position可指向同一个链表，
  //但是position不能在[first,last)范围之内
  void splice(iterator __position, list&, iterator __first, iterator __last) {
    if (__first != __last) 
      this->transfer(__position, __first, __last);
  }
  //以下是成员函数声明，定义在list类外实现
  //******************************
  //删除链表中值等于value的所有节点
  void remove(const _Tp& __value);
  //删除连续重复的元素节点，使之唯一
  //注意：是连续的重复元素
  void unique();
  //合并两个已排序的链表
  void merge(list& __x);
  //反转链表容器的内容
  void reverse();
  //按升序排序链表内容
  void sort();

#ifdef __STL_MEMBER_TEMPLATES
  template <class _Predicate> void remove_if(_Predicate);
  template <class _BinaryPredicate> void unique(_BinaryPredicate);
  template <class _StrictWeakOrdering> void merge(list&, _StrictWeakOrdering);
  template <class _StrictWeakOrdering> void sort(_StrictWeakOrdering);
#endif /* __STL_MEMBER_TEMPLATES */
};
//list定义结束
//********************************

//********************************
//*********以下是比较运算符操作符重载***********
//********************************
template <class _Tp, class _Alloc>
inline bool 
operator==(const list<_Tp,_Alloc>& __x, const list<_Tp,_Alloc>& __y)
{
  typedef typename list<_Tp,_Alloc>::const_iterator const_iterator;
  const_iterator __end1 = __x.end();
  const_iterator __end2 = __y.end();

  const_iterator __i1 = __x.begin();
  const_iterator __i2 = __y.begin();
  while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2) {
    ++__i1;
    ++__i2;
  }
  return __i1 == __end1 && __i2 == __end2;
}

template <class _Tp, class _Alloc>
inline bool operator<(const list<_Tp,_Alloc>& __x,
                      const list<_Tp,_Alloc>& __y)
{
  return lexicographical_compare(__x.begin(), __x.end(),
                                 __y.begin(), __y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline bool operator!=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool operator>(const list<_Tp,_Alloc>& __x,
                      const list<_Tp,_Alloc>& __y) {
  return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool operator<=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Tp, class _Alloc>
inline bool operator>=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x < __y);
}

//交换两个链表内容
template <class _Tp, class _Alloc>
inline void 
swap(list<_Tp, _Alloc>& __x, list<_Tp, _Alloc>& __y)
{
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */
//操作符重载结束
//********************************

//以下是list类成员函数的具体定义
//********************************
#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _InputIter>
void 
list<_Tp, _Alloc>::_M_insert_dispatch(iterator __position,
                                      _InputIter __first, _InputIter __last,
                                      __false_type)
{
  for ( ; __first != __last; ++__first)//遍历范围[first,last)
    insert(__position, *__first);//一个一个节点插入
}

#else /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void 
list<_Tp, _Alloc>::insert(iterator __position, 
                          const _Tp* __first, const _Tp* __last)
{
  for ( ; __first != __last; ++__first)//遍历范围[first,last)
    insert(__position, *__first);//一个一个节点插入
}

template <class _Tp, class _Alloc>
void 
list<_Tp, _Alloc>::insert(iterator __position,
                         const_iterator __first, const_iterator __last)
{
  for ( ; __first != __last; ++__first)//遍历范围[first,last)
    insert(__position, *__first);//一个一个节点插入
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void 
list<_Tp, _Alloc>::_M_fill_insert(iterator __position,
                                  size_type __n, const _Tp& __x)
{
  for ( ; __n > 0; --__n)//插入n个节点
    insert(__position, __x);//在position之前插入x节点
}

template <class _Tp, class _Alloc>
typename list<_Tp,_Alloc>::iterator list<_Tp, _Alloc>::erase(iterator __first, 
                                                             iterator __last)
{
  while (__first != __last)//遍历范围[first,last)
    erase(__first++);//一个一个节点删除
  return __last;
}

//重新调整容器的大小
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::resize(size_type __new_size, const _Tp& __x)
{
  iterator __i = begin();
  size_type __len = 0;//表示容器的原始大小
  for ( ; __i != end() && __len < __new_size; ++__i, ++__len)
    ;
  if (__len == __new_size)//若容器新的长度比原来的小，则擦除多余的元素
    erase(__i, end());
  else//若容器新的长度比原来的大，则把其初始化为x值   // __i == end()
    insert(end(), __new_size - __len, __x);
}

//赋值操作
template <class _Tp, class _Alloc>
list<_Tp, _Alloc>& list<_Tp, _Alloc>::operator=(const list<_Tp, _Alloc>& __x)
{
  if (this != &__x) {
    iterator __first1 = begin();
    iterator __last1 = end();
    const_iterator __first2 = __x.begin();
    const_iterator __last2 = __x.end();
    while (__first1 != __last1 && __first2 != __last2) 
      *__first1++ = *__first2++;
    if (__first2 == __last2)//若当前容器的大小大于x容器大小
      erase(__first1, __last1);//则擦除多余部分
    else//若当前容器大小小于x容器大小，则把x容器剩下的数据插入到当前容器尾
      insert(__last1, __first2, __last2);
	//上面if语句里面的语句可以用下面代替
	/*
		clear();
		this->assign(__x.begin(),__x.end());
	*/
  }
  return *this;
}

//在已定义list容器中填充n个初始值为val的节点
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp& __val) {
  iterator __i = begin();
  for ( ; __i != end() && __n > 0; ++__i, --__n)
    *__i = __val;
  if (__n > 0)//若容器大小不够存储n个节点，则使用插入函数
    insert(end(), __n, __val);
  else//若容器原来的数据大小比n大，则擦除多余的数据
    erase(__i, end());
  //注：个人认为该函数也可以这样实现：
  //首先清空容器原来的内容
  //然后在容器插入n个值为val的数据节点
  /*
  _Tp tmp = __val;
  clear();
  insert(begin(),__n,__val);
  */
}

#ifdef __STL_MEMBER_TEMPLATES

//若输入数据类型不是整数型别时，assign(_InputIter __first, _InputIter __last)调用该函数
//在[first,last)实现填充数值操作
template <class _Tp, class _Alloc> template <class _InputIter>
void
list<_Tp, _Alloc>::_M_assign_dispatch(_InputIter __first2, _InputIter __last2,
                                      __false_type)
{
  //获取原始容器的大小
  iterator __first1 = begin();
  iterator __last1 = end();
  //若原始容器和[first2,last2)大小不为0或1，则进行赋值操作
  for ( ; __first1 != __last1 && __first2 != __last2; ++__first1, ++__first2)
    *__first1 = *__first2;
  if (__first2 == __last2)//若原始容器的大小比[first2,last2)大
    erase(__first1, __last1);
  else
	  //若原始容器的大小比[first2,last2)小
    insert(__last1, __first2, __last2);
}

#endif /* __STL_MEMBER_TEMPLATES */

//删除容器中值为value的所有数据节点
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::remove(const _Tp& __value)
{
  iterator __first = begin();
  iterator __last = end();
  while (__first != __last) {//遍历整个容器
    iterator __next = __first;
    ++__next;
    if (*__first == __value) erase(__first);//若存在该值，则擦除
    __first = __next;//继续查找，直到first == last
  }
}

//
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::unique()
{
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;//若为空容器，则退出
  iterator __next = __first;
  while (++__next != __last) {//若容器大小大于1，进入while循环
    if (*__first == *__next)//若相邻元素相同
      erase(__next);//则擦除
    else//否则，查找下一节点
      __first = __next;
    __next = __first;
  }
}

//合并两个已排序的链表,合并后的链表仍然是有序的
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::merge(list<_Tp, _Alloc>& __x)
{
  iterator __first1 = begin();
  iterator __last1 = end();
  iterator __first2 = __x.begin();
  iterator __last2 = __x.end();
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first2 < *__first1) {
      iterator __next = __first2;
      transfer(__first1, __first2, ++__next);//把first2拼接在first1之前
      __first2 = __next;
    }
    else
      ++__first1;
  //若链表x比当前链表长，则把剩余的数据节点拼接到当前链表的尾端
  if (__first2 != __last2) transfer(__last1, __first2, __last2);
}

inline void __List_base_reverse(_List_node_base* __p)
{
  _List_node_base* __tmp = __p;
  do {
    __STD::swap(__tmp->_M_next, __tmp->_M_prev);//交换指针所指的节点地址
    __tmp = __tmp->_M_prev;     // Old next node is now prev.
  } while (__tmp != __p);
}

//把当前链表逆序
template <class _Tp, class _Alloc>
inline void list<_Tp, _Alloc>::reverse() 
{
  __List_base_reverse(this->_M_node);
}    

//按升序进行排序，list链表的迭代器访问时双向迭代器
//因为STL的排序算法函数sort()是接受随机访问迭代器，在这里并不适合
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::sort()
{
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) 
  {
    list<_Tp, _Alloc> __carry;//carry链表起到搬运的作用
	//counter链表是中间存储作用
	/*
	*其中对于counter[i]里面最多的存储数据为2^(i+1)个节点
	*若超出则向高位进位即counter[i+1]
	*/
    list<_Tp, _Alloc> __counter[64];
    int __fill = 0;
    while (!empty()) 
	{//若不是空链表
		//第一步：
      __carry.splice(__carry.begin(), *this, begin());//把当前链表的第一个节点放在carry链表头
      int __i = 0;
      while(__i < __fill && !__counter[__i].empty()) 
	  {
        //第二步：
		  __counter[__i].merge(__carry);//把链表carry合并到counter[i]
        //第三步：
		  __carry.swap(__counter[__i++]);//交换链表carry和counter[i]内容
      }
      //第四步：
	  __carry.swap(__counter[__i]);//交换链表carry和counter[i]内容         
      //第五步：
	  if (__i == __fill) ++__fill;
    } 

    for (int __i = 1; __i < __fill; ++__i)
      //第六步：
		__counter[__i].merge(__counter[__i-1]);//把低位不满足进位的剩余数据全部有序的合并到上一位
    //第七步：
	swap(__counter[__fill-1]);//最后把已排序好的链表内容交换到当前链表
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _Predicate>
void list<_Tp, _Alloc>::remove_if(_Predicate __pred)
{
  iterator __first = begin();
  iterator __last = end();
  while (__first != __last) {
    iterator __next = __first;
    ++__next;
    if (__pred(*__first)) erase(__first);
    __first = __next;
  }
}

template <class _Tp, class _Alloc> template <class _BinaryPredicate>
void list<_Tp, _Alloc>::unique(_BinaryPredicate __binary_pred)
{
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;
  iterator __next = __first;
  while (++__next != __last) {
    if (__binary_pred(*__first, *__next))
      erase(__next);
    else
      __first = __next;
    __next = __first;
  }
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering>
void list<_Tp, _Alloc>::merge(list<_Tp, _Alloc>& __x,
                              _StrictWeakOrdering __comp)
{
  iterator __first1 = begin();
  iterator __last1 = end();
  iterator __first2 = __x.begin();
  iterator __last2 = __x.end();
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1)) {
      iterator __next = __first2;
      transfer(__first1, __first2, ++__next);
      __first2 = __next;
    }
    else
      ++__first1;
  if (__first2 != __last2) transfer(__last1, __first2, __last2);
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering>
void list<_Tp, _Alloc>::sort(_StrictWeakOrdering __comp)
{
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
    list<_Tp, _Alloc> __carry;
    list<_Tp, _Alloc> __counter[64];
    int __fill = 0;
    while (!empty()) {
      __carry.splice(__carry.begin(), *this, begin());
      int __i = 0;
      while(__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry, __comp);
        __carry.swap(__counter[__i++]);
      }
      __carry.swap(__counter[__i]);         
      if (__i == __fill) ++__fill;
    } 

    for (int __i = 1; __i < __fill; ++__i) 
      __counter[__i].merge(__counter[__i-1], __comp);
    swap(__counter[__fill-1]);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_LIST_H */

// Local Variables:
// mode:C++
// End:
```

参考资料：

《STL源码剖析》侯捷

[《](http://www.programlife.net/stl-list.html)[STL笔记之list](http://www.programlife.net/stl-list.html)[》](http://www.programlife.net/stl-list.html)

[list源码剖析](https://www.kancloud.cn/digest/stl-sources/177269)

