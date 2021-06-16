### 前言

在 SGI STL 中的容器 map，底层实现机制是 RB-Tree，是因为 map 的操作 RB-Tree 都能实现，有关 RB-Tree 的剖析请看《[STL源码剖析——RB-Tree](http://blog.csdn.net/chenhanzhun/article/details/39523519)[(红黑树)](http://blog.csdn.net/chenhanzhun/article/details/39523519)》。

在 map 容器里，键值 key 和实值 value 是不相同的，键值 key 和实值 value 的比较函数也是不同的。

在容器里面的元素是根据元素的键值自动排序的，不能修改 map 容器的键值，但是可以修改容器的实值。

map 的所有节点元素都是 pair，pair 有两个成员变量 first，second；

第一个 first 是键值key。第二个 second 是实值 value；有关 pair 的介绍见前文《[stl_pair.h学习](http://blog.csdn.net/chenhanzhun/article/details/39526719)》剖析。

本文的源码出自 SGI STL 中的 <stl_map.h> 文件。

## map 容器源码剖析

在源码剖析的时候，会针对一些函数给出例子，例子包含在剖析文件里面。

```c++
#ifndef __SGI_STL_INTERNAL_MAP_H
#define __SGI_STL_INTERNAL_MAP_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif
/*
	map的所有节点元素都是pair,pair有两个成员变量first，second
	第一个first是键值key,第二个second是实值value
	有关pair的定义见前文<stl_pair.h>剖析
*/

//map内部元素根据键值key默认使用递增排序less
//用户可自行制定比较类型
//内部维护的数据结构是红黑树, 具有非常优秀的最坏情况的时间复杂度  
//注意：map键值和实值是分开的，map的键值key是唯一的，实值value可以重复
//不能通过迭代器修改map的键值key,其迭代器类型是定义为RB-Tree的const_iterator
//但是可以通过迭代器修改map的实值value

// Forward declarations of operators == and <, needed for friend declarations.
template <class _Key, class _Tp, 
          class _Compare __STL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class map;

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator==(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const map<_Key,_Tp,_Compare,_Alloc>& __y);

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const map<_Key,_Tp,_Compare,_Alloc>& __y);

//map定义
template <class _Key, class _Tp, class _Compare, class _Alloc>
class map {
public:

// requirements:

  __STL_CLASS_REQUIRES(_Tp, _Assignable);
  __STL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

// typedefs:

  typedef _Key                  key_type;//键值key类型
  typedef _Tp                   data_type;//数据(实值)value类型
  typedef _Tp                   mapped_type;
  typedef pair<const _Key, _Tp> value_type;//元素型别,包含(键值/实值),const保证键值key不被修改
  typedef _Compare              key_compare;//键值key比较函数
    
  //嵌套类,提供键值key比较函数接口
  //继承自<stl_function.h>中的binary_function
  /*
	template <class _Arg1, class _Arg2, class _Result>
	struct binary_function {
		typedef _Arg1 first_argument_type;
		typedef _Arg2 second_argument_type;
		typedef _Result result_type;
	}; 
  */
  class value_compare
    : public binary_function<value_type, value_type, bool> {
  friend class map<_Key,_Tp,_Compare,_Alloc>;
  protected :
    _Compare comp;
    value_compare(_Compare __c) : comp(__c) {}
  public:
    bool operator()(const value_type& __x, const value_type& __y) const {
      return comp(__x.first, __y.first);//以键值调用比较函数
    }
  };

private:
	//底层机制是RB-Tree
	//以map类型（一个pair）的第一个类型作为TB-tree的键值类型.  
	//所以在RB-tree中，键值key不能修改
  typedef _Rb_tree<key_type, value_type, 
                   _Select1st<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing map
public:
  typedef typename _Rep_type::pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  //map的迭代器不直接定义为const_iterator,而是分别定义iterator,const_iterator
  //是因为map的键值key不能被修改,所以必须定义为const_iterator
  //而map的实值value可以被修改,则定义为iterator
  typedef typename _Rep_type::iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  // allocation/deallocation
  // map只能使用RB-tree的insert-unique(),不能使用insert-equal() 
  //因为必须保证键值唯一
  /*
				构造函数
	map();
	explicit map (const key_compare& comp = key_compare(),
              const allocator_type& alloc = allocator_type());
	
	template <class InputIterator>
	map (InputIterator first, InputIterator last,
       const key_compare& comp = key_compare(),
       const allocator_type& alloc = allocator_type());
	
	map (const map& x);
  */
  /*
	example:
	#include <iostream>
	#include <map>

	bool fncomp (char lhs, char rhs) {return lhs<rhs;}

	struct classcomp {
	  bool operator() (const char& lhs, const char& rhs) const
	  {return lhs<rhs;}
	};

	int main ()
	{
	  std::map<char,int> first;

	  first['a']=10;
	  first['b']=30;
	  first['c']=50;
	  first['d']=70;

	  std::map<char,int> second (first.begin(),first.end());

	  std::map<char,int> third (second);

	  std::map<char,int,classcomp> fourth;                 // class as Compare

	  bool(*fn_pt)(char,char) = fncomp;
	  std::map<char,int,bool(*)(char,char)> fifth (fn_pt); // function pointer as Compare

	  return 0;
	}
  */

  map() : _M_t(_Compare(), allocator_type()) {}
  explicit map(const _Compare& __comp,
               const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  map(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  template <class _InputIterator>
  map(_InputIterator __first, _InputIterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
#else
  map(const value_type* __first, const value_type* __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  map(const value_type* __first,
      const value_type* __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }

  map(const_iterator __first, const_iterator __last)
    : _M_t(_Compare(), allocator_type()) 
    { _M_t.insert_unique(__first, __last); }

  map(const_iterator __first, const_iterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }

#endif /* __STL_MEMBER_TEMPLATES */

  //拷贝构造函数
  map(const map<_Key,_Tp,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}
  //这里提供了operator=,即可以通过=初始化对象
  map<_Key,_Tp,_Compare,_Alloc>&
  operator=(const map<_Key, _Tp, _Compare, _Alloc>& __x)
  {
    _M_t = __x._M_t;
    return *this; 
  }

  // accessors:

  //以下调用RB-Tree的操作
  //返回键值的比较函数,这里是调用RB-Tree的key_comp()
  key_compare key_comp() const { return _M_t.key_comp(); }
  //返回实值的比较函数
  //这里调用的是map嵌套类中定义的比较函数
  /*
	  class value_compare
		: public binary_function<value_type, value_type, bool> {
	  friend class map<_Key,_Tp,_Compare,_Alloc>;
	  protected :
		_Compare comp;
		value_compare(_Compare __c) : comp(__c) {}
	  public:
		bool operator()(const value_type& __x, const value_type& __y) const {
		  return comp(__x.first, __y.first);//以键值调用比较函数
		}
  */
  //实际上最终还是调用键值key的比较函数，即他们是调用同一个比较函数
  value_compare value_comp() const { return value_compare(_M_t.key_comp()); }
  //获得分配器的类型
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() { return _M_t.begin(); }
  const_iterator begin() const { return _M_t.begin(); }
  iterator end() { return _M_t.end(); }
  const_iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() { return _M_t.rbegin(); }
  const_reverse_iterator rbegin() const { return _M_t.rbegin(); }
  reverse_iterator rend() { return _M_t.rend(); }
  const_reverse_iterator rend() const { return _M_t.rend(); }
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  //重载operator[],返回是实值value(即pair.second)的引用
  //注意:若你原先没有定义map对象,即你访问的键值key不存在,则会自动新建一个map对象
  //键值key为你访问的键值key,实值value为空,看下面的例子就明白了
    _Tp& operator[](const key_type& __k) {
    iterator __i = lower_bound(__k);
    // __i->first is greater than or equivalent to __k.
    if (__i == end() || key_comp()(__k, (*__i).first))
      __i = insert(__i, value_type(__k, _Tp()));
    return (*__i).second;
	//其实简单的方式是直接返回
	//return (*((insert(value_type(k, T()))).first)).second;
  }
	/*
	example:
	#include <iostream>
	#include <map>
	#include <string>

	int main ()
	{
	  std::map<char,std::string> mymap;

	  mymap['a']="an element";
	  mymap['b']="another element";
	  mymap['c']=mymap['b'];

	  std::cout << "mymap['a'] is " << mymap['a'] << '\n';
	  std::cout << "mymap['b'] is " << mymap['b'] << '\n';
	  std::cout << "mymap['c'] is " << mymap['c'] << '\n';
	  std::cout << "mymap['d'] is " << mymap['d'] << '\n';

	  std::cout << "mymap now contains " << mymap.size() << " elements.\n";

	  return 0;
	}
	Ouput:
	mymap['a'] is an element
	mymap['b'] is another element
	mymap['c'] is another element
	mymap['d'] is
	mymap now contains 4 elements.
  */
	//交换map对象的内容
  void swap(map<_Key,_Tp,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  /*
	插入元素
	single element (1):	
	pair<iterator,bool> insert (const value_type& val);

	with hint (2):
	iterator insert (iterator position, const value_type& val);

	range (3):	
	template <class InputIterator>
	void insert (InputIterator first, InputIterator last);
  */

  //插入元素节点,调用RB-Tree的insert_unique(__x);
  //不能插入相同键值的元素
  pair<iterator,bool> insert(const value_type& __x) 
    { return _M_t.insert_unique(__x); }
  //在指定位置插入元素，但是会先遍历该集合,判断是否存在相同元素
  //若不存在才在指定位置插入该元素
  iterator insert(iterator position, const value_type& __x)
    { return _M_t.insert_unique(position, __x); }
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_unique(__first, __last);
  }
#else
  void insert(const value_type* __first, const value_type* __last) {
    _M_t.insert_unique(__first, __last);
  }
  void insert(const_iterator __first, const_iterator __last) {
    _M_t.insert_unique(__first, __last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  /*
  擦除元素
	 void erase (iterator position);
		
	size_type erase (const key_type& k);
	
     void erase (iterator first, iterator last);
  */

  //在指定位置擦除元素
  void erase(iterator __position) { _M_t.erase(__position); }
   //擦除指定键值的节点
  size_type erase(const key_type& __x) { return _M_t.erase(__x); }
  //擦除指定区间的节点
  void erase(iterator __first, iterator __last)
    { _M_t.erase(__first, __last); }
  //清空map
  void clear() { _M_t.clear(); }

  // map operations:

  //查找指定键值的节点
  iterator find(const key_type& __x) { return _M_t.find(__x); }
  const_iterator find(const key_type& __x) const { return _M_t.find(__x); }
  //计算指定键值元素的个数
  size_type count(const key_type& __x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1; 
  }

  /*
	Example:
	#include <iostream>
	#include <map>

	int main ()
	{
	  std::map<char,int> mymap;
	  std::map<char,int>::iterator itlow,itup;

	  mymap['a']=20;
	  mymap['b']=40;
	  mymap['c']=60;
	  mymap['d']=80;
	  mymap['e']=100;

	  itlow=mymap.lower_bound ('b');  // itlow points to b
	  itup=mymap.upper_bound ('d');   // itup points to e (not d!)

	  mymap.erase(itlow,itup);        // erases [itlow,itup)

	  // print content:
	  for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';

	  return 0;
	}
	Output:
	a => 20
	e => 100
  */
  //Returns an iterator pointing to the first element in the container 
  //whose key is not considered to go before k (i.e., either it is equivalent or goes after).
  //this->first is greater than or equivalent to __x.
  iterator lower_bound(const key_type& __x) {return _M_t.lower_bound(__x); }
  const_iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x); 
  }
  //Returns an iterator pointing to the first element that is greater than key.
  iterator upper_bound(const key_type& __x) {return _M_t.upper_bound(__x); }
  const_iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x); 
  }
  
  //Returns the bounds of a range that includes all the elements in the container 
  //which have a key equivalent to k
  //Because the elements in a map container have unique keys, 
  //the range returned will contain a single element at most.
  pair<iterator,iterator> equal_range(const key_type& __x) {
    return _M_t.equal_range(__x);
  }
  pair<const_iterator,const_iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }
  /*
  Example:
	#include <iostream>
	#include <map>

	int main ()
	{
	  std::map<char,int> mymap;

	  mymap['a']=10;
	  mymap['b']=20;
	  mymap['c']=30;

	  std::pair<std::map<char,int>::iterator,std::map<char,int>::iterator> ret;
	  ret = mymap.equal_range('b');

	  std::cout << "lower bound points to: ";
	  std::cout << ret.first->first << " => " << ret.first->second << '\n';

	  std::cout << "upper bound points to: ";
	  std::cout << ret.second->first << " => " << ret.second->second << '\n';

	  return 0;
	}
	Output:
		lower bound points to: 'b' => 20
		upper bound points to: 'c' => 30
  */

//以下是操作符重载
#ifdef __STL_TEMPLATE_FRIENDS 
  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator== (const map<_K1, _T1, _C1, _A1>&,
                          const map<_K1, _T1, _C1, _A1>&);
  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator< (const map<_K1, _T1, _C1, _A1>&,
                         const map<_K1, _T1, _C1, _A1>&);
#else /* __STL_TEMPLATE_FRIENDS */
  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const map&, const map&);
  friend bool __STD_QUALIFIER
  operator< __STL_NULL_TMPL_ARGS (const map&, const map&);
#endif /* __STL_TEMPLATE_FRIENDS */
};

//比较两个map的内容
template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator==(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator!=(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<=(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>=(const map<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline void swap(map<_Key,_Tp,_Compare,_Alloc>& __x, 
                 map<_Key,_Tp,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_MAP_H */

// Local Variables:
// mode:C++
// End:
```

参考资料：

《STL源码剖析》侯捷