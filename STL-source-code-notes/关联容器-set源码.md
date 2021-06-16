## 前言

在 SGI STL 中的容器 set，其实现基本上是在 RB-Tree 的基础上，把 RB-Tree 作为其底层的实现机制，是因为 set的操作 RB-Tree 都能实现。

有关 RB-Tree 的剖析请看《[STL源码剖析——RB-Tree(红黑树)](http://blog.csdn.net/chenhanzhun/article/details/39523519)》。

在 set 容器里，键值 key 和实值 value 是相同的，且在容器里面的元素是根据元素的键值自动排序的，同时我们不能修改 set 容器里面的元素值，所以 set 的迭代器是采用 RB-Tree 的 const_iterator，不允许用户对其进行修改操作。

本文的源码出自 SGI STL 中的 <stl_set.h> 文件。

## set 容器源码剖析

```c++
#ifndef __SGI_STL_INTERNAL_SET_H
#define __SGI_STL_INTERNAL_SET_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif
//set内部元素默认使用递增排序less
//用户可自行制定比较类型
//内部维护的数据结构是红黑树, 具有非常优秀的最坏情况的时间复杂度  
//注意: set键值和实值是相同的,set内不允许重复元素的存在, 如果插入重复元素,则会忽略插入操作 
//且不能通过迭代器修改set的元素值,其迭代器类型是定义为RB-Tree的const_iterator

// Forward declarations of operators < and ==, needed for friend declaration.

template <class _Key, class _Compare __STL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __STL_DEFAULT_ALLOCATOR(_Key) >
class set;

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y);

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y);

template <class _Key, class _Compare, class _Alloc>
class set {
  // requirements:

  __STL_CLASS_REQUIRES(_Key, _Assignable);
  __STL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

public:
  // typedefs:
 //在set中key就是value, value同时也是key 
  typedef _Key     key_type;
  typedef _Key     value_type;
  //注意：以下key_compare和value_compare使用相同的比较函数 
  typedef _Compare key_compare;
  typedef _Compare value_compare;
private:
	//set的底层机制是采用RB-Tree数据结构，在<stl_tree.h>实现
  typedef _Rb_tree<key_type, value_type, 
                  _Identity<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing set
public:
  typedef typename _Rep_type::const_pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::const_reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  //set的迭代器iterator 定义为RB-Tree的const_iterator，不允许用户通过迭代器修改set的元素值  
  //因为set的元素有一定次序安排，修改其值会破坏排序规则 
  typedef typename _Rep_type::const_iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  // allocation/deallocation

 // set只能使用RB-tree的insert-unique(),不能使用insert-equal() 
  //当要插入键值和已经存在的键值相同时，就会被忽略 
  /*构造函数原型
	set();
	explicit set (const key_compare& comp = key_compare(),
              const allocator_type& alloc = allocator_type());
	
	template <class InputIterator>
	set (InputIterator first, InputIterator last,
       const key_compare& comp = key_compare(),
       const allocator_type& alloc = allocator_type());
	
	set (const set& x);
  */
  set() : _M_t(_Compare(), allocator_type()) {}
  explicit set(const _Compare& __comp,
               const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
#else
  set(const value_type* __first, const value_type* __last) 
    : _M_t(_Compare(), allocator_type()) 
    { _M_t.insert_unique(__first, __last); }

  set(const value_type* __first, 
      const value_type* __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }

  set(const_iterator __first, const_iterator __last)
    : _M_t(_Compare(), allocator_type()) 
    { _M_t.insert_unique(__first, __last); }

  set(const_iterator __first, const_iterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
#endif /* __STL_MEMBER_TEMPLATES */

  set(const set<_Key,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}
  set<_Key,_Compare,_Alloc>& operator=(const set<_Key, _Compare, _Alloc>& __x)
  { 
    _M_t = __x._M_t;//调用了底层红黑树的operator=操作函数
    return *this;
  }

  //以下所有的set操作行为，RB-tree都已提供，所以set只要调用即可 
  // accessors:

   //返回用于key比较的函数,调用RB-Tree的key_comp()
  key_compare key_comp() const { return _M_t.key_comp(); }
  //由于set的性质, value和key使用同一个比较函数
  value_compare value_comp() const { return _M_t.key_comp(); }
  //获取分配器类型
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  //以下是一些set的基本迭代器所指位置
  iterator begin() const { return _M_t.begin(); }
  iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() const { return _M_t.rbegin(); } 
  reverse_iterator rend() const { return _M_t.rend(); }
  //以下的函数都是调用了RB-Tree的实现,set不必自己定义
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  //这里调用的swap()函数是专属于RB-Tree的swap()，并不是STL的swap()算法
  void swap(set<_Key,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase

  // 根据返回值的情况，判断是否插入该元素
  //pair.second为true则表示已插入该元素  
  //为false则表示set中已存在与待插入相同的元素, 不会重复插入  
  pair<iterator,bool> insert(const value_type& __x) { 
    pair<typename _Rep_type::iterator, bool> __p = _M_t.insert_unique(__x); 
    return pair<iterator, bool>(__p.first, __p.second);
  }
  //在指定位置插入元素，但是会先遍历该集合,判断是否存在相同元素
  //若不存在才在指定位置插入该元素
  iterator insert(iterator __position, const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    return _M_t.insert_unique((_Rep_iterator&)__position, __x);
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_unique(__first, __last);
  }
#else
  void insert(const_iterator __first, const_iterator __last) {
    _M_t.insert_unique(__first, __last);
  }
  void insert(const value_type* __first, const value_type* __last) {
    _M_t.insert_unique(__first, __last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  //擦除指定位置的元素
  void erase(iterator __position) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__position); 
  }
  //擦除元素值为x的节点
  size_type erase(const key_type& __x) { 
    return _M_t.erase(__x); 
  }
  //擦除指定区间的节点
  void erase(iterator __first, iterator __last) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last); 
  }
  //清除set
  void clear() { _M_t.clear(); }

  // set operations:

  //查找元素值为x的节点
  iterator find(const key_type& __x) const { return _M_t.find(__x); }
  //返回指定元素的个数
  size_type count(const key_type& __x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1;
  }
  //返回不小于当前元素的第一个可插入的位置 
  iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }
  // 返回大于当前元素的第一个可插入的位置 
  iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x); 
  }
  pair<iterator,iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }

#ifdef __STL_TEMPLATE_FRIENDS
  template <class _K1, class _C1, class _A1>
  friend bool operator== (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
  template <class _K1, class _C1, class _A1>
  friend bool operator< (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
#else /* __STL_TEMPLATE_FRIENDS */
  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const set&, const set&);
  friend bool __STD_QUALIFIER
  operator<  __STL_NULL_TMPL_ARGS (const set&, const set&);
#endif /* __STL_TEMPLATE_FRIENDS */
};

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Compare, class _Alloc>
inline bool operator!=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Compare, class _Alloc>
inline void swap(set<_Key,_Compare,_Alloc>& __x, 
                 set<_Key,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_SET_H */

// Local Variables:
// mode:C++
// End:
```

参考资料：

《STL源码剖析》侯捷