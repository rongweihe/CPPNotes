## 序列式容器 deque 源码

```c++

#include <deque>
#include <algorithm>
#include <iostream>
using namespace std;

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	// 迭代器定义
  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); }
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */
template <class T, class Ref, class Ptr>
struct __deque_iterator {
  typedef __deque_iterator<T, T&, T*>             iterator;
  typedef __deque_iterator<T, const T&, const T*> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(0, sizeof(T)); }
#endif
	// deque是random_access_iterator_tag类型
  typedef random_access_iterator_tag iterator_category;
  // 基本类型的定义, 满足traits编程
  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  // node
  typedef T** map_pointer;
  map_pointer node;
  typedef __deque_iterator self;
  ...
};

// 满足traits编程
template <class T, class Ref, class Ptr, size_t BufSiz>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return random_access_iterator_tag();
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	typedef T value_type;
	T* cur;
	T* first;
	T* last;
	//...
};



template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	...
  	// 初始化cur指向当前数组位置, last指针数组的尾, node指向y
  	__deque_iterator(T* x, map_pointer y)  : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
  	// 初始化为一个空的deque
  	__deque_iterator() : cur(0), first(0), last(0), node(0) {}
  	// 接受一个迭代器
  	__deque_iterator(const iterator& x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}
    ...
};


template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // Basic types
  deque() : start(), finish(), map(0), map_size(0)	// 默认构造函数
  {
    create_map_and_nodes(0);
  }
    deque(const deque& x) : start(), finish(), map(0), map_size(0)	// 接受一个deque
  {
    create_map_and_nodes(x.size());
    __STL_TRY {
      uninitialized_copy(x.begin(), x.end(), start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }


     // 接受 n:初始化大小, value:初始化的值
  deque(size_type n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
  deque(int n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  } 
  deque(long n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }


template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
protected:                      // Internal typedefs
...
    // 获取缓冲区大小
  static size_type buffer_size() {
    return __deque_buf_size(BufSiz, sizeof(value_type));
  }
  static size_type initial_map_size() { return 8; }

protected:                      // Data members
  iterator start;	// 指向第一个元素的地址
  iterator finish;	// 指向最后一个元素的后一个地址, 即尾

  map_pointer map;		// 定义map, 指向指针的指针
  size_type map_size;	// map的实际大小
public:                         // Basic accessors
  iterator begin() { return start; }	// 获取头地址
  const_iterator begin() const { return start; }
  iterator end() { return finish; }		// 获取尾地址
  const_iterator end() const { return finish; }
	// 倒转后获取首尾地址.
  reverse_iterator rbegin() { return reverse_iterator(finish); }
  reverse_iterator rend() { return reverse_iterator(start); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(finish);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(start);
  }

  // 获取第一个和最后一个元素的值
  reference front() { return *start; }
  reference back() {
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  const_reference front() const { return *start; }
  const_reference back() const {
    const_iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  size_type size() const { return finish - start;; } 	// 获取数组的大小
  size_type max_size() const { return size_type(-1); }	
  bool empty() const { return finish == start; }	// 判断deque是否为空
    
  reference operator[](size_type n) { return start[difference_type(n)]; }
  const_reference operator[](size_type n) const {
    return start[difference_type(n)];
  }
    ...
};


template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // push_* and pop_*
    // 对尾进行插入
    // 判断函数是否达到了数组尾部. 没有达到就直接进行插入
  void push_back(const value_type& t) {
    if (finish.cur != finish.last - 1) {
      construct(finish.cur, t);
      ++finish.cur;
    }
    else
      push_back_aux(t);
  }
    // 对头进行插入
    // 判断函数是否达到了数组头部. 没有达到就直接进行插入
  void push_front(const value_type& t) {
    if (start.cur != start.first) {
      construct(start.cur - 1, t);
      --start.cur;
    }
    else
      push_front_aux(t);
  }
    ...
};

template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public: 
    // 对尾部进行操作
    // 判断是否达到数组的头部. 没有到达就直接释放
    void pop_back() {
    if (finish.cur != finish.first) {
      --finish.cur;
      destroy(finish.cur);
    }
    else
      pop_back_aux();
  }
    // 对头部进行操作
    // 判断是否达到数组的尾部. 没有到达就直接释放
  void pop_front() {
    if (start.cur != start.last - 1) {
      destroy(start.cur);
      ++start.cur;
    }
    else 
      pop_front_aux();
  }
    ...
};


template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
  difference_type index = pos - start;
  value_type x_copy = x;  // 判断插入的位置离头还是尾比较近
  if (index < size() / 2) { // 离头进
    push_front(front());	// 将头往前移动 
    iterator front1 = start;// 调整将要移动的距离
    ++front1;
    iterator front2 = front1;
    ++front2;
    pos = start + index;
    iterator pos1 = pos;
    ++pos1;
    copy(front2, pos1, front1); // 用copy进行调整
  }
    // 离尾近
  else {
    push_back(back());	// 将尾往前移动 调整将要移动的距离
    iterator back1 = finish;
    --back1;
    iterator back2 = back1;
    --back2;
    pos = start + index; // 用copy进行调整
    copy_backward(pos, back2, back1);
  }
  *pos = x_copy;
  return pos;
}
```

