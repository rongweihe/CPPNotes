
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