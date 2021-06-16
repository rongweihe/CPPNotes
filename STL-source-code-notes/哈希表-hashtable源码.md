## 前言

前面介绍的关联容器 set、multiset、map 和multimap 的底层机制都是基于 RB-Tree 红黑树，虽然能够实现在插入、删除和搜素操作能够达到对数平均时间，可是要求输入数据有足够的随机性。

本文介绍的 hash table 不需要要求输入数据具有随机性，在插入、删除和搜素操作都能达到常数平均时间。

来自 SGI STL 中的 <stl_hashtable.h> 文件，在这里为了避免冲突（即不同元素映射到相同的键值位置），采用了拉链法来解决冲突问题。

SGI 中实现 hash table 的方式，在每个表格元素中维护一个链表，然后在链表上执行元素的插入、搜寻、删除等操作，该表格中的每个元素被称为桶(bucket)。

## hashtable 散列表源码剖析

```c++
#ifndef __SGI_STL_INTERNAL_HASHTABLE_H
#define __SGI_STL_INTERNAL_HASHTABLE_H

// Hashtable class, used to implement the hashed associative containers
// hash_set, hash_map, hash_multiset, and hash_multimap.

//SGI STL的hashtable的实现方法是拉链法.
//拉链法可以避免hashtable的冲突问题,即不同数据映射到同一的hash值

//有关hashtable的介绍见前文:
//http://blog.csdn.net/chenhanzhun/article/details/38091431

#include <stl_algobase.h>
#include <stl_alloc.h>
#include <stl_construct.h>
#include <stl_tempbuf.h>
#include <stl_algo.h>
#include <stl_uninitialized.h>
#include <stl_function.h>
#include <stl_vector.h>
#include <stl_hash_fun.h>

__STL_BEGIN_NAMESPACE

//hashtable中链表的节点结构
//类似于单链表的节点结构
template <class _Val>
struct _Hashtable_node
{
  _Hashtable_node* _M_next;//指向下一节点
  _Val _M_val;//节点元素值
};  

//这里使用前置声明, 避免在后面交叉引用会导致编译错误
template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc = alloc>
class hashtable;

template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc>
struct _Hashtable_iterator;

template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc>
struct _Hashtable_const_iterator;

//hashtable迭代器定义
//注意：hash table迭代器没有提供后退操作operator--
//也没用提供逆向迭代器reverse iterator
template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc>
struct _Hashtable_iterator {
	//内嵌类型别名
  typedef hashtable<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,_Alloc>
          _Hashtable;
  typedef _Hashtable_iterator<_Val, _Key, _HashFcn, 
                              _ExtractKey, _EqualKey, _Alloc>
          iterator;
  typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, 
                                    _ExtractKey, _EqualKey, _Alloc>
          const_iterator;
  typedef _Hashtable_node<_Val> _Node;

  typedef forward_iterator_tag iterator_category;//采用正向迭代器
  typedef _Val value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef _Val& reference;
  typedef _Val* pointer;

  _Node* _M_cur;//当前迭代器所指位置
  _Hashtable* _M_ht;//hashtable中的位置,控制访问桶子连续性

  _Hashtable_iterator(_Node* __n, _Hashtable* __tab) 
    : _M_cur(__n), _M_ht(__tab) {}
  _Hashtable_iterator() {}
  //返回当前节点元素值的引用
  reference operator*() const { return _M_cur->_M_val; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
  //操作符重载定义在后面定义
  iterator& operator++();
  iterator operator++(int);
  //比较两个迭代器是否指向同一个节点
  bool operator==(const iterator& __it) const
    { return _M_cur == __it._M_cur; }
  bool operator!=(const iterator& __it) const
    { return _M_cur != __it._M_cur; }
};

//下面是const iterator的定义，基本和上面相同
template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc>
struct _Hashtable_const_iterator {
  typedef hashtable<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,_Alloc>
          _Hashtable;
  typedef _Hashtable_iterator<_Val,_Key,_HashFcn, 
                              _ExtractKey,_EqualKey,_Alloc>
          iterator;
  typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, 
                                    _ExtractKey, _EqualKey, _Alloc>
          const_iterator;
  typedef _Hashtable_node<_Val> _Node;

  typedef forward_iterator_tag iterator_category;
  typedef _Val value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef const _Val& reference;
  typedef const _Val* pointer;

  const _Node* _M_cur;
  const _Hashtable* _M_ht;

  _Hashtable_const_iterator(const _Node* __n, const _Hashtable* __tab)
    : _M_cur(__n), _M_ht(__tab) {}
  _Hashtable_const_iterator() {}
  _Hashtable_const_iterator(const iterator& __it) 
    : _M_cur(__it._M_cur), _M_ht(__it._M_ht) {}
  reference operator*() const { return _M_cur->_M_val; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
  const_iterator& operator++();
  const_iterator operator++(int);
  bool operator==(const const_iterator& __it) const 
    { return _M_cur == __it._M_cur; }
  bool operator!=(const const_iterator& __it) const 
    { return _M_cur != __it._M_cur; }
};

// Note: assumes long is at least 32 bits.
// 注意：假设long至少为32-bits, 可以根据自己需要修改
//定义28个素数用作hashtable的大小 
enum { __stl_num_primes = 28 };

static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};

//返回大于n的最小素数
inline unsigned long __stl_next_prime(unsigned long __n)
{
  const unsigned long* __first = __stl_prime_list;
  const unsigned long* __last = __stl_prime_list + (int)__stl_num_primes;
  const unsigned long* pos = lower_bound(__first, __last, __n);
  /*
  上面的lower_bound调用的是STL中的算法lower_bound();

  该算法的功能：
  Returns an iterator pointing to the first element in the range [first,last) which does not compare less than val.
  The elements in the range shall already be sorted according to this same criterion (operator< or comp)
  即返回在[first,last)范围内第一个不小于val的位置
  注意：调用该算法之前，[first,last)范围里面的元素必须已排序

  该算法的原型如下：
  第一个版本：采用默认比较准则operator<
	template <class ForwardIterator, class T>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val);
  第二版本：采用用户指定的比较函数comp
	template <class ForwardIterator, class T, class Compare>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val, Compare comp);

	其实该算法的实现机制使用二分查找法进行查找元素val：
	template <class ForwardIterator, class T>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last, const T& val)
	{
	  ForwardIterator it;
	  iterator_traits<ForwardIterator>::difference_type count, step;
	  count = distance(first,last);
	  while (count>0)
	  {
		it = first; step=count/2; advance (it,step);
		if (*it<val) {                 // or: if (comp(*it,val)), for version (2)
		  first=++it;
		  count-=step+1;
		}
		else count=step;
	  }
	  return first;
	}
	下面给出例子：lower_bound/upper_bound example
	#include <iostream>     // std::cout
	#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort
	#include <vector>       // std::vector

	int main () {
	  int myints[] = {10,20,30,30,20,10,10,20};
	  std::vector<int> v(myints,myints+8);           // 10 20 30 30 20 10 10 20

	  std::sort (v.begin(), v.end());                // 10 10 10 20 20 20 30 30

	  std::vector<int>::iterator low,up;
	  low=std::lower_bound (v.begin(), v.end(), 20); //          ^
	  up= std::upper_bound (v.begin(), v.end(), 20); //                   ^

	  std::cout << "lower_bound at position " << (low- v.begin()) << '\n';
	  std::cout << "upper_bound at position " << (up - v.begin()) << '\n';

	  return 0;
	}
	Output:
	lower_bound at position 3
	upper_bound at position 6
  */
  return pos == __last ? *(__last - 1) : *pos;
}

// Forward declaration of operator==.

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
class hashtable;

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
bool operator==(const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht1,
                const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht2);

// Hashtables handle allocators a bit differently than other containers
//  do.  If we're using standard-conforming allocators, then a hashtable
//  unconditionally has a member variable to hold its allocator, even if
//  it so happens that all instances of the allocator type are identical.
// This is because, for hashtables, this extra storage is negligible.  
//  Additionally, a base class wouldn't serve any other purposes; it 
//  wouldn't, for example, simplify the exception-handling code.

//hash table的定义
//模板参数定义
/* 
Value： 节点的实值类型 
Key：   节点的键值类型 
HashFcn： hash function的类型 
ExtractKey：从节点中取出键值的方法（函数或仿函数） 
EqualKey：判断键值是否相同的方法（函数或仿函数） 
Alloc：空间配置器
*/ 
//hash table的线性表是用vector容器维护
template <class _Val, class _Key, class _HashFcn,
          class _ExtractKey, class _EqualKey, class _Alloc>
class hashtable {
public:
  typedef _Key key_type;
  typedef _Val value_type;
  typedef _HashFcn hasher;
  typedef _EqualKey key_equal;

  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;

  hasher hash_funct() const { return _M_hash; }
  key_equal key_eq() const { return _M_equals; }

private:
  typedef _Hashtable_node<_Val> _Node;

#ifdef __STL_USE_STD_ALLOCATORS
public:
  typedef typename _Alloc_traits<_Val,_Alloc>::allocator_type allocator_type;
  allocator_type get_allocator() const { return _M_node_allocator; }
private:
  typename _Alloc_traits<_Node, _Alloc>::allocator_type _M_node_allocator;
  _Node* _M_get_node() { return _M_node_allocator.allocate(1); }
  void _M_put_node(_Node* __p) { _M_node_allocator.deallocate(__p, 1); }
# define __HASH_ALLOC_INIT(__a) _M_node_allocator(__a), 
#else /* __STL_USE_STD_ALLOCATORS */
public:
  typedef _Alloc allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }
private:
  typedef simple_alloc<_Node, _Alloc> _M_node_allocator_type;
  _Node* _M_get_node() { return _M_node_allocator_type::allocate(1); }
  void _M_put_node(_Node* __p) { _M_node_allocator_type::deallocate(__p, 1); }
# define __HASH_ALLOC_INIT(__a)
#endif /* __STL_USE_STD_ALLOCATORS */

//以下是hash table的成员变量
private:
  hasher                _M_hash;
  key_equal             _M_equals;
  _ExtractKey           _M_get_key;
  vector<_Node*,_Alloc> _M_buckets;//用vector维护buckets
  size_type             _M_num_elements;//hashtable中list节点个数

public:
  typedef _Hashtable_iterator<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,_Alloc>
          iterator;
  typedef _Hashtable_const_iterator<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,
                                    _Alloc>
          const_iterator;

  friend struct
  _Hashtable_iterator<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,_Alloc>;
  friend struct
  _Hashtable_const_iterator<_Val,_Key,_HashFcn,_ExtractKey,_EqualKey,_Alloc>;

public:
	//构造函数
  hashtable(size_type __n,
            const _HashFcn&    __hf,
            const _EqualKey&   __eql,
            const _ExtractKey& __ext,
            const allocator_type& __a = allocator_type())
    : __HASH_ALLOC_INIT(__a)
      _M_hash(__hf),
      _M_equals(__eql),
      _M_get_key(__ext),
      _M_buckets(__a),
      _M_num_elements(0)
  {
    _M_initialize_buckets(__n);//预留空间,并将其初始化为空0
	//预留空间大小为大于n的最小素数
  }

  hashtable(size_type __n,
            const _HashFcn&    __hf,
            const _EqualKey&   __eql,
            const allocator_type& __a = allocator_type())
    : __HASH_ALLOC_INIT(__a)
      _M_hash(__hf),
      _M_equals(__eql),
      _M_get_key(_ExtractKey()),
      _M_buckets(__a),
      _M_num_elements(0)
  {
    _M_initialize_buckets(__n);
  }

  //拷贝构造函数
  hashtable(const hashtable& __ht)
    : __HASH_ALLOC_INIT(__ht.get_allocator())
      _M_hash(__ht._M_hash),
      _M_equals(__ht._M_equals),
      _M_get_key(__ht._M_get_key),
      _M_buckets(__ht.get_allocator()),
      _M_num_elements(0)
  {
    _M_copy_from(__ht);//复制hashtable内容
  }

#undef __HASH_ALLOC_INIT

  //可以通过operator=初始化hashtable对象
  hashtable& operator= (const hashtable& __ht)
  {
    if (&__ht != this) {
      clear();
      _M_hash = __ht._M_hash;
      _M_equals = __ht._M_equals;
      _M_get_key = __ht._M_get_key;
      _M_copy_from(__ht);
    }
    return *this;
  }

  ~hashtable() { clear(); }

  //返回hashtable元素的个数
  size_type size() const { return _M_num_elements; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return size() == 0; }

  //交换两个hashtable的内容
  void swap(hashtable& __ht)
  {
    __STD::swap(_M_hash, __ht._M_hash);
    __STD::swap(_M_equals, __ht._M_equals);
    __STD::swap(_M_get_key, __ht._M_get_key);
    _M_buckets.swap(__ht._M_buckets);
    __STD::swap(_M_num_elements, __ht._M_num_elements);
  }

  iterator begin()
  { 
    for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
      if (_M_buckets[__n])//若hashtable中的桶子_M_buckets有链表list
        return iterator(_M_buckets[__n], this);//返回链表的第一个节点位置
    return end();//若list链表为空,则返回尾端end(),其实这里尾端和起始端一样
  }

  iterator end() { return iterator(0, this); }//返回桶子list链表的null指针,即尾端

  const_iterator begin() const
  {
    for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
      if (_M_buckets[__n])
        return const_iterator(_M_buckets[__n], this);
    return end();
  }

  const_iterator end() const { return const_iterator(0, this); }

#ifdef __STL_MEMBER_TEMPLATES
  template <class _Vl, class _Ky, class _HF, class _Ex, class _Eq, class _Al>
  friend bool operator== (const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al>&,
                          const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al>&);
#else /* __STL_MEMBER_TEMPLATES */
  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const hashtable&, const hashtable&);
#endif /* __STL_MEMBER_TEMPLATES */

public:

  //返回桶子个数,即线性表中节点数
  size_type bucket_count() const { return _M_buckets.size(); }

  //线性表最多分配节点数
  size_type max_bucket_count() const
    { return __stl_prime_list[(int)__stl_num_primes - 1]; } 

  //指定桶子键值key中list链表的元素个数
  size_type elems_in_bucket(size_type __bucket) const
  {
    size_type __result = 0;
    for (_Node* __cur = _M_buckets[__bucket]; __cur; __cur = __cur->_M_next)
      __result += 1;
    return __result;
  }

  //插入元素节点,不允许存在重复元素
  pair<iterator, bool> insert_unique(const value_type& __obj)
  {
    //判断容量是否够用, 否则就重新配置 
	resize(_M_num_elements + 1);
	//插入元素,不允许存在重复元素
    return insert_unique_noresize(__obj);
  }

   //插入元素节点,允许存在重复元素
  iterator insert_equal(const value_type& __obj)
  {//判断容量是否够用, 否则就重新配置
    resize(_M_num_elements + 1);
	//插入元素,允许存在重复元素
    return insert_equal_noresize(__obj);
  }
  //具体定义见hashtable类外后面的剖析
  pair<iterator, bool> insert_unique_noresize(const value_type& __obj);
  iterator insert_equal_noresize(const value_type& __obj);
 
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert_unique(_InputIterator __f, _InputIterator __l)
  {
    insert_unique(__f, __l, __ITERATOR_CATEGORY(__f));
  }

  template <class _InputIterator>
  void insert_equal(_InputIterator __f, _InputIterator __l)
  {
    insert_equal(__f, __l, __ITERATOR_CATEGORY(__f));
  }

  template <class _InputIterator>
  void insert_unique(_InputIterator __f, _InputIterator __l,
                     input_iterator_tag)
  {
    for ( ; __f != __l; ++__f)
      insert_unique(*__f);
  }

  template <class _InputIterator>
  void insert_equal(_InputIterator __f, _InputIterator __l,
                    input_iterator_tag)
  {
    for ( ; __f != __l; ++__f)
      insert_equal(*__f);
  }

  template <class _ForwardIterator>
  void insert_unique(_ForwardIterator __f, _ForwardIterator __l,
                     forward_iterator_tag)
  {
    size_type __n = 0;
    distance(__f, __l, __n);
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_unique_noresize(*__f);
  }

  template <class _ForwardIterator>
  void insert_equal(_ForwardIterator __f, _ForwardIterator __l,
                    forward_iterator_tag)
  {
    size_type __n = 0;
    distance(__f, __l, __n);
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_equal_noresize(*__f);
  }

#else /* __STL_MEMBER_TEMPLATES */
  void insert_unique(const value_type* __f, const value_type* __l)
  {
    size_type __n = __l - __f;
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_unique_noresize(*__f);
  }

  void insert_equal(const value_type* __f, const value_type* __l)
  {
    size_type __n = __l - __f;
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_equal_noresize(*__f);
  }

  void insert_unique(const_iterator __f, const_iterator __l)
  {
    size_type __n = 0;
    distance(__f, __l, __n);
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_unique_noresize(*__f);
  }

  void insert_equal(const_iterator __f, const_iterator __l)
  {
    size_type __n = 0;
    distance(__f, __l, __n);
    resize(_M_num_elements + __n);
    for ( ; __n > 0; --__n, ++__f)
      insert_equal_noresize(*__f);
  }
#endif /*__STL_MEMBER_TEMPLATES */

  reference find_or_insert(const value_type& __obj);
  //查找指定键值的元素
  iterator find(const key_type& __key) 
  {
    size_type __n = _M_bkt_num_key(__key);//获取键值
    _Node* __first;
    for ( __first = _M_buckets[__n];
          __first && !_M_equals(_M_get_key(__first->_M_val), __key);
          __first = __first->_M_next)
      {}
    return iterator(__first, this);
  } 

  const_iterator find(const key_type& __key) const
  {
    size_type __n = _M_bkt_num_key(__key);
    const _Node* __first;
    for ( __first = _M_buckets[__n];
          __first && !_M_equals(_M_get_key(__first->_M_val), __key);
          __first = __first->_M_next)
      {}
    return const_iterator(__first, this);
  } 

  //返回键值为key的元素的个数
  size_type count(const key_type& __key) const
  {
    const size_type __n = _M_bkt_num_key(__key);
    size_type __result = 0;

    for (const _Node* __cur = _M_buckets[__n]; __cur; __cur = __cur->_M_next)
      if (_M_equals(_M_get_key(__cur->_M_val), __key))
        ++__result;
    return __result;
  }

  pair<iterator, iterator> 
  equal_range(const key_type& __key);

  pair<const_iterator, const_iterator> 
  equal_range(const key_type& __key) const;

  //擦除元素
  size_type erase(const key_type& __key);
  void erase(const iterator& __it);
  void erase(iterator __first, iterator __last);

  void erase(const const_iterator& __it);
  void erase(const_iterator __first, const_iterator __last);

  void resize(size_type __num_elements_hint);
  void clear();

private:
	//返回大于n的最小素数
	//实际上调用__stl_next_prime(__n); 
  size_type _M_next_size(size_type __n) const
    { return __stl_next_prime(__n); }

  //预留空间,并将其初始化为0
  void _M_initialize_buckets(size_type __n)
  {
    //返回大于n的最小素数__n_buckets
	 const size_type __n_buckets = _M_next_size(__n);
	 //这里调用vector的成员函数reserve
	 //reserve该函数功能是改变可用空间的大小
	 //Requests that the vector capacity be at least enough to contain __n_buckets elements.
    _M_buckets.reserve(__n_buckets);
	//调用vector的插入函数insert
	//在原始end后面连续插入__n_buckets个0
    _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node*) 0);
    _M_num_elements = 0;
  }

  //获取键值key在桶子的位置
  size_type _M_bkt_num_key(const key_type& __key) const
  {
    return _M_bkt_num_key(__key, _M_buckets.size());
  }

  //获取在桶子的序号，也就是键值
  //输入参数是实值value
  size_type _M_bkt_num(const value_type& __obj) const
  {
    return _M_bkt_num_key(_M_get_key(__obj));
  }

  size_type _M_bkt_num_key(const key_type& __key, size_t __n) const
  {
    return _M_hash(__key) % __n;//采用除法取余hash函数
  }

  size_type _M_bkt_num(const value_type& __obj, size_t __n) const
  {
    return _M_bkt_num_key(_M_get_key(__obj), __n);
  }

  //分配节点空间，并构造对象
  _Node* _M_new_node(const value_type& __obj)
  {
    _Node* __n = _M_get_node();
    __n->_M_next = 0;
    __STL_TRY {
      construct(&__n->_M_val, __obj);
      return __n;
    }
    __STL_UNWIND(_M_put_node(__n));
  }
  
  //析构对象，并释放空间
  void _M_delete_node(_Node* __n)
  {
    destroy(&__n->_M_val);
    _M_put_node(__n);
  }

  void _M_erase_bucket(const size_type __n, _Node* __first, _Node* __last);
  void _M_erase_bucket(const size_type __n, _Node* __last);

  void _M_copy_from(const hashtable& __ht);

};

//前缀operator++重载，前进一个list节点
template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
_Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&
_Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>::operator++()
{
  const _Node* __old = _M_cur;
  _M_cur = _M_cur->_M_next;//若存在，则返回
  //若当前节点为空，则需前进到下一个桶子的节点
  if (!_M_cur) {
	  //根据元素值，定位出下一个bucket的位置，其起始位置就是我们的目的地
    size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
    while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
      _M_cur = _M_ht->_M_buckets[__bucket];
  }
  return *this;
}

//后缀operator++重载

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline _Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>
_Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>::operator++(int)
{
  iterator __tmp = *this;
  ++*this;//调用operator++
  return __tmp;
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
_Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&
_Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>::operator++()
{
  const _Node* __old = _M_cur;
  _M_cur = _M_cur->_M_next;
  if (!_M_cur) {
    size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
    while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
      _M_cur = _M_ht->_M_buckets[__bucket];
  }
  return *this;
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline _Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>
_Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>::operator++(int)
{
  const_iterator __tmp = *this;
  ++*this;
  return __tmp;
}

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline forward_iterator_tag
iterator_category(const _Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&)
{
  return forward_iterator_tag();
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline _Val* 
value_type(const _Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&)
{
  return (_Val*) 0;
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline hashtable<_Val,_Key,_HF,_ExK,_EqK,_All>::difference_type*
distance_type(const _Hashtable_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&)
{
  return (hashtable<_Val,_Key,_HF,_ExK,_EqK,_All>::difference_type*) 0;
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline forward_iterator_tag
iterator_category(const _Hashtable_const_iterator<_Val,_Key,_HF,
                                                  _ExK,_EqK,_All>&)
{
  return forward_iterator_tag();
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline _Val* 
value_type(const _Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&)
{
  return (_Val*) 0;
}

template <class _Val, class _Key, class _HF, class _ExK, class _EqK, 
          class _All>
inline hashtable<_Val,_Key,_HF,_ExK,_EqK,_All>::difference_type*
distance_type(const _Hashtable_const_iterator<_Val,_Key,_HF,_ExK,_EqK,_All>&)
{
  return (hashtable<_Val,_Key,_HF,_ExK,_EqK,_All>::difference_type*) 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
bool operator==(const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht1,
                const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht2)
{
  typedef typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::_Node _Node;
  if (__ht1._M_buckets.size() != __ht2._M_buckets.size())
    return false;
  for (int __n = 0; __n < __ht1._M_buckets.size(); ++__n) {
    _Node* __cur1 = __ht1._M_buckets[__n];
    _Node* __cur2 = __ht2._M_buckets[__n];
    for ( ; __cur1 && __cur2 && __cur1->_M_val == __cur2->_M_val;
          __cur1 = __cur1->_M_next, __cur2 = __cur2->_M_next)
      {}
    if (__cur1 || __cur2)
      return false;
  }
  return true;
}  

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
inline bool operator!=(const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht1,
                       const hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>& __ht2) {
  return !(__ht1 == __ht2);
}

template <class _Val, class _Key, class _HF, class _Extract, class _EqKey, 
          class _All>
inline void swap(hashtable<_Val, _Key, _HF, _Extract, _EqKey, _All>& __ht1,
                 hashtable<_Val, _Key, _HF, _Extract, _EqKey, _All>& __ht2) {
  __ht1.swap(__ht2);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

//插入元素，不需要重新调整内存空间,不允许存在重复元素
template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator, bool> 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::insert_unique_noresize(const value_type& __obj)
{
  //获取待插入元素在hashtable中的桶子位置
  const size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  //判断hashtable中是否存在与之相等的键值元素
  //若存在则不插入
  //否则插入该元素
  for (_Node* __cur = __first; __cur; __cur = __cur->_M_next) 
    if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
      return pair<iterator, bool>(iterator(__cur, this), false);

  //把元素插入到第一个节点位置
  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return pair<iterator, bool>(iterator(__tmp, this), true);
}

//插入元素，允许重复,不需要分配新的空间
//也就是说有足够的空间
template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::insert_equal_noresize(const value_type& __obj)
{
	//获取待插入元素在hashtable中的桶子位置
  const size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for (_Node* __cur = __first; __cur; __cur = __cur->_M_next) 
	  //若存在键值相同的元素，则插在相同元素下一个位置
    if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj))) {
      _Node* __tmp = _M_new_node(__obj);//创建新节点
      __tmp->_M_next = __cur->_M_next;//将新节点插在当前节点之后
      __cur->_M_next = __tmp;
      ++_M_num_elements;//节点数加1
      return iterator(__tmp, this);//返回指向新增节点迭代器
    }
	//若不存在相同键值的元素,则插在第一个位置
  _Node* __tmp = _M_new_node(__obj);//创建新节点
  __tmp->_M_next = __first;//插入在链表表头
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;//节点数加1
  return iterator(__tmp, this);//返回指向新增节点的迭代器
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::reference 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::find_or_insert(const value_type& __obj)
{
  resize(_M_num_elements + 1);

  size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
    if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
      return __cur->_M_val;

  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return __tmp->_M_val;
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator,
     typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator> 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::equal_range(const key_type& __key)
{
  typedef pair<iterator, iterator> _Pii;
  const size_type __n = _M_bkt_num_key(__key);

  for (_Node* __first = _M_buckets[__n]; __first; __first = __first->_M_next)
    if (_M_equals(_M_get_key(__first->_M_val), __key)) {
      for (_Node* __cur = __first->_M_next; __cur; __cur = __cur->_M_next)
        if (!_M_equals(_M_get_key(__cur->_M_val), __key))
          return _Pii(iterator(__first, this), iterator(__cur, this));
      for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
        if (_M_buckets[__m])
          return _Pii(iterator(__first, this),
                     iterator(_M_buckets[__m], this));
      return _Pii(iterator(__first, this), end());
    }
  return _Pii(end(), end());
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::const_iterator, 
     typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::const_iterator> 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::equal_range(const key_type& __key) const
{
  typedef pair<const_iterator, const_iterator> _Pii;
  const size_type __n = _M_bkt_num_key(__key);

  for (const _Node* __first = _M_buckets[__n] ;
       __first; 
       __first = __first->_M_next) {
    if (_M_equals(_M_get_key(__first->_M_val), __key)) {
      for (const _Node* __cur = __first->_M_next;
           __cur;
           __cur = __cur->_M_next)
        if (!_M_equals(_M_get_key(__cur->_M_val), __key))
          return _Pii(const_iterator(__first, this),
                      const_iterator(__cur, this));
      for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
        if (_M_buckets[__m])
          return _Pii(const_iterator(__first, this),
                      const_iterator(_M_buckets[__m], this));
      return _Pii(const_iterator(__first, this), end());
    }
  }
  return _Pii(end(), end());
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::size_type 
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const key_type& __key)
{
  const size_type __n = _M_bkt_num_key(__key);
  _Node* __first = _M_buckets[__n];
  size_type __erased = 0;

  if (__first) {
    _Node* __cur = __first;
    _Node* __next = __cur->_M_next;
    while (__next) {
      if (_M_equals(_M_get_key(__next->_M_val), __key)) {
        __cur->_M_next = __next->_M_next;
        _M_delete_node(__next);
        __next = __cur->_M_next;
        ++__erased;
        --_M_num_elements;
      }
      else {
        __cur = __next;
        __next = __cur->_M_next;
      }
    }
    if (_M_equals(_M_get_key(__first->_M_val), __key)) {
      _M_buckets[__n] = __first->_M_next;
      _M_delete_node(__first);
      ++__erased;
      --_M_num_elements;
    }
  }
  return __erased;
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const iterator& __it)
{
  _Node* __p = __it._M_cur;
  if (__p) {
    const size_type __n = _M_bkt_num(__p->_M_val);
    _Node* __cur = _M_buckets[__n];

    if (__cur == __p) {
      _M_buckets[__n] = __cur->_M_next;
      _M_delete_node(__cur);
      --_M_num_elements;
    }
    else {
      _Node* __next = __cur->_M_next;
      while (__next) {
        if (__next == __p) {
          __cur->_M_next = __next->_M_next;
          _M_delete_node(__next);
          --_M_num_elements;
          break;
        }
        else {
          __cur = __next;
          __next = __cur->_M_next;
        }
      }
    }
  }
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::erase(iterator __first, iterator __last)
{
  size_type __f_bucket = __first._M_cur ? 
    _M_bkt_num(__first._M_cur->_M_val) : _M_buckets.size();
  size_type __l_bucket = __last._M_cur ? 
    _M_bkt_num(__last._M_cur->_M_val) : _M_buckets.size();

  if (__first._M_cur == __last._M_cur)
    return;
  else if (__f_bucket == __l_bucket)
    _M_erase_bucket(__f_bucket, __first._M_cur, __last._M_cur);
  else {
    _M_erase_bucket(__f_bucket, __first._M_cur, 0);
    for (size_type __n = __f_bucket + 1; __n < __l_bucket; ++__n)
      _M_erase_bucket(__n, 0);
    if (__l_bucket != _M_buckets.size())
      _M_erase_bucket(__l_bucket, __last._M_cur);
  }
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
inline void
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const_iterator __first,
                                             const_iterator __last)
{
  erase(iterator(const_cast<_Node*>(__first._M_cur),
                 const_cast<hashtable*>(__first._M_ht)),
        iterator(const_cast<_Node*>(__last._M_cur),
                 const_cast<hashtable*>(__last._M_ht)));
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
inline void
hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const const_iterator& __it)
{
  erase(iterator(const_cast<_Node*>(__it._M_cur),
                 const_cast<hashtable*>(__it._M_ht)));
}

//调整hashtable的容量
//新的容量大小为__num_elements_hint
template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::resize(size_type __num_elements_hint)
{
  //hashtable原始大小
  const size_type __old_n = _M_buckets.size();
  if (__num_elements_hint > __old_n) {//若新的容量大小比原始的大
	  //查找不低于__num_elements_hint的最小素数
    const size_type __n = _M_next_size(__num_elements_hint);
    if (__n > __old_n) {
		//创建新的线性表,容量为__n，只是起到中介作用
      vector<_Node*, _All> __tmp(__n, (_Node*)(0),
                                 _M_buckets.get_allocator());
      __STL_TRY {//以下是复制数据
        for (size_type __bucket = 0; __bucket < __old_n; ++__bucket) {
          _Node* __first = _M_buckets[__bucket];
          while (__first) {
			  //获取实值在新桶子的键值位置
            size_type __new_bucket = _M_bkt_num(__first->_M_val, __n);
			//这个只是为了方便while循环里面__first的迭代
            _M_buckets[__bucket] = __first->_M_next;
			//将当前节点插入到新的桶子__new_bucket里面,成为list的第一个节点
            __first->_M_next = __tmp[__new_bucket];//__first->_M_next指向null指针,因为新桶子是空的
            __tmp[__new_bucket] = __first;//新桶子对应键值指向第一个节点
            __first = _M_buckets[__bucket];//更新当前指针          
          }
        }
        _M_buckets.swap(__tmp);//交换内容
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {//释放临时hashtable的线性表tmp
        for (size_type __bucket = 0; __bucket < __tmp.size(); ++__bucket) {
          while (__tmp[__bucket]) {
            _Node* __next = __tmp[__bucket]->_M_next;
            _M_delete_node(__tmp[__bucket]);
            __tmp[__bucket] = __next;
          }
        }
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
    }
  }
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::_M_erase_bucket(const size_type __n, _Node* __first, _Node* __last)
{
  _Node* __cur = _M_buckets[__n];
  if (__cur == __first)
    _M_erase_bucket(__n, __last);
  else {
    _Node* __next;
    for (__next = __cur->_M_next; 
         __next != __first; 
         __cur = __next, __next = __cur->_M_next)
      ;
    while (__next != __last) {
      __cur->_M_next = __next->_M_next;
      _M_delete_node(__next);
      __next = __cur->_M_next;
      --_M_num_elements;
    }
  }
}

template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::_M_erase_bucket(const size_type __n, _Node* __last)
{
  _Node* __cur = _M_buckets[__n];
  while (__cur != __last) {
    _Node* __next = __cur->_M_next;
    _M_delete_node(__cur);
    __cur = __next;
    _M_buckets[__n] = __cur;
    --_M_num_elements;
  }
}

//清空hashtable，但是没有释放bucket vector空间
template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::clear()
{
  for (size_type __i = 0; __i < _M_buckets.size(); ++__i) {//遍历每个桶子
    _Node* __cur = _M_buckets[__i];//当前节点为桶子的第一个节点
    while (__cur != 0) {//遍历桶子维护的链表，并释放每个链表节点
      _Node* __next = __cur->_M_next;
      _M_delete_node(__cur);
      __cur = __next;
    }
    _M_buckets[__i] = 0;//桶子链表为空
  }
  _M_num_elements = 0;//链表节点数为0
}

    
//拷贝hashtable对象
//实现机制：首先把原始对象清空，再把空间变成被复制对象__ht一样的大小
//最后把__ht的内容复制到目标对象
template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
  ::_M_copy_from(const hashtable& __ht)
{//_M_buckets是vector维护的，可以调用vector的成员函数
  _M_buckets.clear();//清空线性表调用vector::clear()
  _M_buckets.reserve(__ht._M_buckets.size());//把线性表内存空间变成与__ht一样大
  //在_M_buckets vector尾端插入size个元素，初始值为空指针
  //注意：此时_M_buckets vector为空，即尾端也是起始端
  _M_buckets.insert(_M_buckets.end(), __ht._M_buckets.size(), (_Node*) 0);
  __STL_TRY {//开始复制操作
    for (size_type __i = 0; __i < __ht._M_buckets.size(); ++__i) {
		//复制vector的每一个元素(是指向hashtable节点指针)
      const _Node* __cur = __ht._M_buckets[__i];
      if (__cur) {
        _Node* __copy = _M_new_node(__cur->_M_val);
        _M_buckets[__i] = __copy;

        //针对每一个hashtable节点对应的list，复制list的每一个节点
		for (_Node* __next = __cur->_M_next; 
             __next; 
             __cur = __next, __next = __cur->_M_next) {
          __copy->_M_next = _M_new_node(__next->_M_val);
          __copy = __copy->_M_next;
        }
      }
    }
    _M_num_elements = __ht._M_num_elements;//更新节点个数
  }
  __STL_UNWIND(clear());
}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_HASHTABLE_H */

// Local Variables:
// mode:C++
// End:
```

