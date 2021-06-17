### 前言

本文介绍的 STL 算法中的 find、search 查找算法。

在 STL 源码中有关算法的函数大部分在本文介绍，包含 find and find_if、adjacent_find、search、search_n、lower_bound、upper_bound、equal_range、binary_search、find_first_of、find_end 相关算法。

下面对这些算法的源码进行了详细的剖析，并且适当给出应用例子，增加我们对其理解，方便我们使用这些算法。具体详见下面源码剖析。

### 查找算法源码剖析

```c++
// find and find_if.
//查找区间[first,last)内元素第一个与value值相等的元素，并返回其位置
//其中find函数是采用默认的equality操作operator==
//find_if是采用用户自行指定的操作pred

//若find函数萃取出来的迭代器类型为输入迭代器input_iterator_tag，则调用此函数
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val,
                       input_iterator_tag)
{//若尚未到达区间的尾端，且未找到匹配的值，则继续查找
  while (__first != __last && !(*__first == __val))
    ++__first;
  //若找到匹配的值，则返回该位置
  //若找不到，即到达区间尾端，此时first=last，则返回first
  return __first;
}
//若find_if函数萃取出来的迭代器类型为输入迭代器input_iterator_tag，则调用此函数
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred,
                          input_iterator_tag)
{//若尚未到达区间的尾端，且未找到匹配的值，则继续查找
  while (__first != __last && !__pred(*__first))
    ++__first;
  //若找到匹配的值，则返回该位置
  //若找不到，即到达区间尾端，此时first=last，则返回first
  return __first;
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
//若find函数萃取出来的迭代器类型为随机访问迭代器random_access_iterator_tag，则调用此函数
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter find(_RandomAccessIter __first, _RandomAccessIter __last,
                       const _Tp& __val,
                       random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (*__first == __val) return __first;
    ++__first;
  case 2:
    if (*__first == __val) return __first;
    ++__first;
  case 1:
    if (*__first == __val) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}
//若find_if函数萃取出来的迭代器类型为随机访问迭代器random_access_iterator_tag，则调用此函数
template <class _RandomAccessIter, class _Predicate>
_RandomAccessIter find_if(_RandomAccessIter __first, _RandomAccessIter __last,
                          _Predicate __pred,
                          random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (__pred(*__first)) return __first;
    ++__first;
  case 2:
    if (__pred(*__first)) return __first;
    ++__first;
  case 1:
    if (__pred(*__first)) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
/*find函数功能：Returns an iterator to the first element in the range [first,last) that compares equal to val. 
If no such element is found, the function returns last.
find函数原型：
	template <class InputIterator, class T>
	InputIterator find (InputIterator first, InputIterator last, const T& val);
*/
//find函数对外接口
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
            typename iterator_traits<_InputIter>::value_type, _Tp);
  //首先萃取出first迭代器的类型，根据迭代器的类型调用不同的函数
  return find(__first, __last, __val, __ITERATOR_CATEGORY(__first));
}
/*find_if函数功能：Returns an iterator to the first element in the range [first,last) for which pred returns true. 
If no such element is found, the function returns last.
find_if函数原型：
	template <class InputIterator, class UnaryPredicate>
	InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
*/
//find_if 函数对外接口
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
          typename iterator_traits<_InputIter>::value_type);
  //首先萃取出first迭代器的类型，根据迭代器的类型调用不同的函数
  return find_if(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}
//find和find_if函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_if
	#include <vector>       // std::vector

	bool IsOdd (int i) {
	  return ((i%2)==1);
	}

	int main () {
	  std::vector<int> myvector;

	  myvector.push_back(10);
	  myvector.push_back(25);
	  myvector.push_back(40);
	  myvector.push_back(55);

	  std::vector<int>::iterator it = std::find_if (myvector.begin(), myvector.end(), IsOdd);
	  std::cout << "The first odd value is " << *it << '\n';

	  // using std::find with vector and iterator:
	  it = find (myvector.begin(), myvector.end(), 40);
	  if (it != myvector.end())
		std::cout << "Element found in myvector: " << *it << '\n';
	  else
		std::cout << "Element not found in myints\n";

	  return 0;
	}
	Output:
	The first odd value is 25
	Element found in myvector: 40
 
*/

// adjacent_find.

//查找区间[first,last)内第一次重复的相邻元素
//若存在返回相邻元素的第一个元素位置
//若不存在返回last位置
/*该函数有两个版本：第一版本是默认操作operator==；第二版本是用户指定的二元操作pred
函数对外接口的原型：
equality (1)：默认操作是operator==
	template <class ForwardIterator>
	ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last);
predicate (2)：用户指定的二元操作pred	
	template <class ForwardIterator, class BinaryPredicate>
	ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last,
                                  BinaryPredicate pred);

*/
//版本一：默认操作是operator==
template <class _ForwardIter>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  /*
  情况1：若输入区间为空，则直接返回尾端last；
  情况2：若输入区间不为空，且存在相邻重复元素，则返回相邻元素的第一个元素的位置；
  情况3：若输入区间不为空，但是不存在相邻重复元素，则直接返回尾端last；
  */
  //情况1：
  if (__first == __last)//若输入区间为空
    return __last;//直接返回last
  //情况2：
  _ForwardIter __next = __first;//定义当前位置的下一个位置(即当前元素的相邻元素)
  while(++__next != __last) {//若还没到达尾端，执行while循环
    if (*__first == *__next)//相邻元素值相等，则找到相邻重复元素
      return __first;//返回第一个元素的位置
    __first = __next;//若暂时找不到，则继续找，直到到达区间尾端
  }
  //情况3：
  return __last;//直接返回尾端last
}

//版本二：用户指定的二元操作pred	
//实现过程和版本一一样，只是判断规则不同
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last,
                           _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
          typename iterator_traits<_ForwardIter>::value_type,
          typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
	  //如果找到相邻元素符合用户指定条件，就返回第一元素位置
    if (__binary_pred(*__first, *__next))
      return __first;
    __first = __next;
  }
  return __last;
}
//adjacent_find函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::adjacent_find
	#include <vector>       // std::vector

	bool myfunction (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[] = {5,20,5,30,30,20,10,10,20};
	  std::vector<int> myvector (myints,myints+8);
	  std::vector<int>::iterator it;

	  // using default comparison:
	  it = std::adjacent_find (myvector.begin(), myvector.end());

	  if (it!=myvector.end())
		std::cout << "the first pair of repeated elements are: " << *it << '\n';

	  //using predicate comparison:
	  it = std::adjacent_find (++it, myvector.end(), myfunction);

	  if (it!=myvector.end())
		std::cout << "the second pair of repeated elements are: " << *it << '\n';

	  return 0;
	}
	Output:
	the first pair of repeated elements are: 30
	the second pair of repeated elements are: 10

*/

// search.
//在序列一[first1,last1)所涵盖的区间中，查找序列二[first2,last2)的首次出现点
//该查找函数有两个版本：
//版本一：使用默认的equality操作operator==
//版本二：用户根据需要自行指定操作规则
/*search函数功能：Searches the range [first1,last1) for the first occurrence of the sequence defined by [first2,last2), 
and returns an iterator to its first element, or last1 if no occurrences are found.

search函数的原型：
equality (1)：版本一
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二	
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            BinaryPredicate pred);
*/
//版本一：使用默认的equality操作operator==
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2)
    return find(__first1, __last1, *__first2);

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {//若还没到达区间尾端
    __first1 = find(__first1, __last1, *__first2);//查找*first2在区间[first1,last1)首次出现的位置
    if (__first1 == __last1)//若在[first1,last1)中不存在*first2，即在[first1,last1)不存在子序列[first2,last2)
      return __last1;//则直接返回区间尾端

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1)//若[first1,last1)只有一个元素，即序列[first1,last1)小于序列[first2,last2)
      return __last1;//不可能成为其子序列，返回last1

    while (*__current == *__p) {//若两个序列相对应的值相同
      if (++__p == __last2)//若序列[first2,last2)只有两个元素，且与序列一匹配
        return __first1;//则返回匹配的首次位置
      if (++__current == __last1)//若第一个序列小于第二个序列
        return __last1;//返回last1
    }

    ++__first1;
  }
  return __first1;
}

//版本二：用户根据需要自行指定操作规则
template <class _ForwardIter1, class _ForwardIter2, class _BinaryPred>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2,
                     _BinaryPred  __predicate) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2) {
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    return __first1;    
  }

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {
    while (__first1 != __last1) {
      if (__predicate(*__first1, *__first2))
        break;
      ++__first1;
    }
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    if (__first1 == __last1)
      return __last1;

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1) return __last1;

    while (__predicate(*__current, *__p)) {
      if (++__p == __last2)
        return __first1;
      if (++__current == __last1)
        return __last1;
    }

    ++__first1;
  }
  return __first1;
}

// search_n.  Search for __count consecutive copies of __val.
//在序列[first,last)查找连续count个符合条件值value元素的位置
//该查找函数有两个版本：
//版本一：使用默认的equality操作operator==
//版本二：用户根据需要自行指定操作规则
/*search_n函数功能：Searches the range [first,last) for a sequence of count elements, 
each comparing equal to val (or for which pred returns true).

search_n函数的原型：
equality (1)：版本一	
	template <class ForwardIterator, class Size, class T>
	ForwardIterator search_n (ForwardIterator first, ForwardIterator last,
                             Size count, const T& val);
predicate (2)：版本二	
	template <class ForwardIterator, class Size, class T, class BinaryPredicate>
	ForwardIterator search_n ( ForwardIterator first, ForwardIterator last,
                              Size count, const T& val, BinaryPredicate pred );
*/
//版本一：使用默认的equality操作operator==
template <class _ForwardIter, class _Integer, class _Tp>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);

  if (__count <= 0)
    return __first;
  else {//首先查找value第一次出现的位置
    __first = find(__first, __last, __val);
    while (__first != __last) {//若出现的位置不是区间尾端
      _Integer __n = __count - 1;//更新个数，下面只需查找n=count-1个连续相同value即可
      _ForwardIter __i = __first;
      ++__i;//从当前位置的下一个位置开始查找
	  //若没有到达区间尾端，且个数n大于0，且区间元素与value值相等
      while (__i != __last && __n != 0 && *__i == __val) {
        ++__i;//继续查找
        --__n;//减少查找的次数，因为已经找到value再次出现
      }
      if (__n == 0)//若区间尚未到达尾端，但是count个value已经查找到
        return __first;//则输出查找到的首次出现value的位置
      else
        __first = find(__i, __last, __val);//若尚未找到连续count个value值的位置，则找出value下次出现的位置，并准备下一次while循环
    }
    return __last;
  }
}
//版本二：用户根据需要自行指定操作规则
template <class _ForwardIter, class _Integer, class _Tp, class _BinaryPred>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val,
                      _BinaryPred __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool, 
             typename iterator_traits<_ForwardIter>::value_type, _Tp);
  if (__count <= 0)
    return __first;
  else {
    while (__first != __last) {
      if (__binary_pred(*__first, __val))
        break;
      ++__first;
    }
    while (__first != __last) {
      _Integer __n = __count - 1;
      _ForwardIter __i = __first;
      ++__i;
      while (__i != __last && __n != 0 && __binary_pred(*__i, __val)) {
        ++__i;
        --__n;
      }
      if (__n == 0)
        return __first;
      else {
        while (__i != __last) {
          if (__binary_pred(*__i, __val))
            break;
          ++__i;
        }
        __first = __i;
      }
    }
    return __last;
  }
} 
//search和search_n函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::search_n
	#include <vector>       // std::vector

	bool mypredicate (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[]={10,20,30,30,20,10,10,20};
	  std::vector<int> myvector (myints,myints+8);
	  std::vector<int>::iterator it;

	  // using default comparison:
	  it = std::search_n (myvector.begin(), myvector.end(), 2, 30);
	  if (it!=myvector.end())
		std::cout << "two 30s found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "match not found\n";

	  // using predicate comparison:
	  it = std::search_n (myvector.begin(), myvector.end(), 2, 10, mypredicate);
	  if (it!=myvector.end())
		std::cout << "two 10s found at position " << int(it-myvector.begin()) << '\n';
	  else
		std::cout << "match not found\n";
    
	  int needle1[] = {10,20};
  
	  // using default comparison:
	  it = std::search (myvector.begin(), myvector.end(), needle1, needle1+2);  
	   if (it!=myvector.end())
		std::cout << "needle1 found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "needle1 not found\n";
    
	  // using predicate comparison:
	  int needle2[] = {30,20,10};
	  it = std::search (myvector.begin(), myvector.end(), needle2, needle2+3, mypredicate);
	  if (it!=myvector.end())
		std::cout << "needle2 found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "needle2 not found\n";

	  return 0;
	}
	Output:
	two 30s found at position 2
	two 10s found at position 5
	needle1 found at position 0
	needle2 found at position 3
*/

// Binary search (lower_bound, upper_bound, equal_range, binary_search).

template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*) 
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (*__middle < __val) {//将value值与中间值比较，即是二分查找,若中间值小于value，则继续查找右半部分
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//调整查找区间的长度
    }
    else
      __len = __half;//否则查找左半部分
  }
  return __first;
}
//在已排序区间[first,last)查找value值
//若该区间存在与value相等的元素，则返回指向第一个与value相等的迭代器
//若该区间不存在与value相等的元素，则返回指向第一个不小于value值的迭代器
//若该区间的任何元素都比value值小，则返回last
/*
函数功能：Returns an iterator pointing to the first element in the range [first,last) which does not compare less than val.
函数原型：
default (1)	：版本一采用operator<比较
	template <class ForwardIterator, class T>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val);
custom (2)	：版本二采用仿函数comp比较规则
	template <class ForwardIterator, class T, class Compare>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val, Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
				const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __lower_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                              const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (__comp(*__middle, __val)) {//若comp判断为true，则继续在右半部分查找
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//调整查找区间的长度
    }
    else
      __len = __half;//否则查找左半部分
  }
  return __first;
}
//版本二：
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __lower_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (__val < *__middle)//若value小于中间元素值
      __len = __half;//查找左半部分
    else {
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//更新len的值
    }
  }
  return __first;
}
//在已排序区间[first,last)查找value值
//返回大于value值的第一个元素的迭代器
/*
函数功能：Returns an iterator pointing to the first element in the range [first,last) which compares greater than val.
函数原型：
default (1)	：版本一采用operator<比较
	template <class ForwardIterator, class T>
	ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last,
                               const T& val);
custom (2)	：版本二采用仿函数comp比较规则
	template <class ForwardIterator, class T, class Compare>
	ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last,
                               const T& val, Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __upper_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(__val, *__middle))
      __len = __half;
    else {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}
//版本二
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __upper_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
//函数举例
/*
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
template <class _ForwardIter, class _Tp, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//计算区间的长度len
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {//若区间非空
    __half = __len >> 1;//len右移一位，相等于除以2，即half为区间的长度的一半
    __middle = __first;//初始化middle的值
    advance(__middle, __half);//前进middle位置，使其指向区间中间位置
    if (*__middle < __val) {//若指定元素value大于中间元素值，则在右半部分继续查找
		//下面两行使first指向middle的下一个位置，即右半区间的起始位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//更新待查找区间的长度
    }
    else if (__val < *__middle)//若指定元素value小于中间元素值，则在左半部分继续查找
      __len = __half;//更新待查找区间的长度
    else {//若指定元素value等于中间元素值
		//在前半部分找lower_bound位置
      __left = lower_bound(__first, __middle, __val);
      advance(__first, __len);
	  //在后半部分找upper_bound
      __right = upper_bound(++__middle, __first, __val);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);//返回pair对象，第一个迭代器为left，第二个迭代器为right
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}
//查找区间与value相等的相邻重复元素的起始位置和结束位置
//注意：[first,last)是已排序，思想还是采用二分查找法
//同样也有两个版本
/*
函数功能：Returns the bounds of the subrange that includes all the elements of the range [first,last) with values equivalent to val.
函数原型：
default (1)	：版本一默认operator<
	template <class ForwardIterator, class T>
		pair<ForwardIterator,ForwardIterator>
    equal_range (ForwardIterator first, ForwardIterator last, const T& val);
custom (2)	：版本二采用仿函数comp
	template <class ForwardIterator, class T, class Compare>
		pair<ForwardIterator,ForwardIterator>
    equal_range (ForwardIterator first, ForwardIterator last, const T& val,
                  Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __equal_range(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__comp(__val, *__middle))
      __len = __half;
    else {
      __left = lower_bound(__first, __middle, __val, __comp);
      advance(__first, __len);
      __right = upper_bound(++__middle, __first, __val, __comp);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}           
//版本二
template <class _ForwardIter, class _Tp, class _Compare>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
            _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __equal_range(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
} 
//equal_range函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::equal_range, std::sort
	#include <vector>       // std::vector

	bool mygreater (int i,int j) { return (i>j); }

	int main () {
	  int myints[] = {10,20,30,30,20,10,10,20};
	  std::vector<int> v(myints,myints+8);                         // 10 20 30 30 20 10 10 20
	  std::pair<std::vector<int>::iterator,std::vector<int>::iterator> bounds;

	  // using default comparison:
	  std::sort (v.begin(), v.end());                              // 10 10 10 20 20 20 30 30
	  bounds=std::equal_range (v.begin(), v.end(), 20);            //          ^        ^
  
	  std::cout << "bounds at positions " << (bounds.first - v.begin());
	  std::cout << " and " << (bounds.second - v.begin()) << '\n';
  
	  // using "mygreater" as comp:
	  std::sort (v.begin(), v.end(), mygreater);                   // 30 30 20 20 20 10 10 10
	  bounds=std::equal_range (v.begin(), v.end(), 20, mygreater); //       ^        ^

	  std::cout << "bounds at positions " << (bounds.first - v.begin());
	  std::cout << " and " << (bounds.second - v.begin()) << '\n';

	  return 0;
	}
	Output：
	bounds at positions 3 and 6
	bounds at positions 2 and 5 
*/

//二分查找法
//注意：[first,last)是已排序
//同样也有两个版本
/*
函数功能：Returns true if any element in the range [first,last) is equivalent to val, and false otherwise.
函数原型：
default (1)	：版本一默认operator<
	template <class ForwardIterator, class T>
	bool binary_search (ForwardIterator first, ForwardIterator last,
                      const T& val);
custom (2)	：版本二采用仿函数comp
	template <class ForwardIterator, class T, class Compare>
	bool binary_search (ForwardIterator first, ForwardIterator last,
                      const T& val, Compare comp);
*/
template <class _ForwardIter, class _Tp>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  _ForwardIter __i = lower_bound(__first, __last, __val);//调用二分查找函数，并返回不小于value值的第一个迭代器位置i
  return __i != __last && !(__val < *__i);
}

template <class _ForwardIter, class _Tp, class _Compare>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val,
                   _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  _ForwardIter __i = lower_bound(__first, __last, __val, __comp);//调用二分查找函数，并返回不小于value值的第一个迭代器位置i
  return __i != __last && !__comp(__val, *__i);
}
// find_first_of, with and without an explicitly supplied comparison function.
//以[first2,last2)区间内的某些元素为查找目标，寻找他们在[first1,last1)区间首次出现的位置
//find_first_of函数有两个版本：
//版本一：提供默认的equality操作operator==
//版本二：提供用户自行指定的操作规则comp
/*
函数功能：Returns an iterator to the first element in the range [first1,last1) that matches any of the elements in [first2,last2). 
If no such element is found, the function returns last1.
函数原型：
equality (1)：版本一	
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_first_of (ForwardIterator1 first1, ForwardIterator1 last1,
                                   ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二	
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 find_first_of (ForwardIterator1 first1, ForwardIterator1 last1,
                                   ForwardIterator2 first2, ForwardIterator2 last2,
                                   BinaryPredicate pred);
*/
//版本一：提供默认的equality操作operator==
template <class _InputIter, class _ForwardIter>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) //若序列一不为空，则遍历序列一，每次指定一个元素
	  //以下，根据序列二的每个元素
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (*__first1 == *__iter)//若序列一的元素等于序列二的元素，则表示找到
        return __first1;//返回找到的位置
  return __last1;//否则没找到
}
//版本二：提供用户自行指定的操作规则comp
template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2,
                         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) 
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (__comp(*__first1, *__iter))
        return __first1;
  return __last1;
}
//find_first_of函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_first_of
	#include <vector>       // std::vector
	#include <cctype>       // std::tolower

	bool comp_case_insensitive (char c1, char c2) {
	  return (std::tolower(c1)==std::tolower(c2));
	}

	int main () {
	  int mychars[] = {'a','b','c','A','B','C'};
	  std::vector<char> haystack (mychars,mychars+6);
	  std::vector<char>::iterator it;

	  int needle[] = {'A','B','C'};

	  // using default comparison:
	  it = find_first_of (haystack.begin(), haystack.end(), needle, needle+3);

	  if (it!=haystack.end())
		std::cout << "The first match is: " << *it << '\n';

	  // using predicate comparison:
	  it = find_first_of (haystack.begin(), haystack.end(),
						  needle, needle+3, comp_case_insensitive);

	  if (it!=haystack.end())
		std::cout << "The first match is: " << *it << '\n';

	  return 0;
	}
	Output:
	The first match is: A
	The first match is: a
*/

// find_end, with and without an explicitly supplied comparison function.
// Search [first2, last2) as a subsequence in [first1, last1), and return
// the *last* possible match.  Note that find_end for bidirectional iterators
// is much faster than for forward iterators.

// find_end for forward iterators. 
//若萃取出来的迭代器类型为正向迭代器forward_iterator_tag，则调用此函数
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag)
{
  if (__first2 == __last2)//若第二个区间为空
    return __last1;//则直接返回第一个区间的尾端
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
		//以下利用search函数查找出某个子序列的首次出现点；若找不到直接返回last1
      _ForwardIter1 __new_result

        = search(__first1, __last1, __first2, __last2);
      if (__new_result == __last1)//若返回的位置为尾端，则表示没找到
        return __result;//返回last1
      else {//若在[first1,last1)中找到[first2,last2)首次出现的位置，继续准备下一次查找
		  
        __result = __new_result;//更新返回的位置
        __first1 = __new_result;//更新查找的起始位置
        ++__first1;//确定正确查找起始位置
      }
    }
  }
}
//版本二：指定规则
template <class _ForwardIter1, class _ForwardIter2,
          class _BinaryPredicate>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag,
                         _BinaryPredicate __comp)
{
  if (__first2 == __last2)
    return __last1;
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
      _ForwardIter1 __new_result
        = search(__first1, __last1, __first2, __last2, __comp);
      if (__new_result == __last1)
        return __result;
      else {
        __result = __new_result;
        __first1 = __new_result;
        ++__first1;
      }
    }
  }
}

// find_end for bidirectional iterators.  Requires partial specialization.
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
//若萃取出来的迭代器类型为双向迭代器bidirectional_iterator_tag，则调用此函数
template <class _BidirectionalIter1, class _BidirectionalIter2>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  //利用反向迭代器很快就可以找到
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  //查找时将序列一和序列二逆方向
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2);

  if (__rresult == __rlast1)//表示没找到
    return __last1;
  else {//找到了
    _BidirectionalIter1 __result = __rresult.base();//转会正常迭代器
    advance(__result, -distance(__first2, __last2));//调整回到子序列的起始位置
    return __result;
  }
}
//版本二：指定规则comp
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BinaryPredicate>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag, 
           _BinaryPredicate __comp)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2,
                               __comp);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));
    return __result;
  }
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
		  // Dispatching functions for find_end.
//find_end函数有两个版本：
//版本一：提供默认的equality操作operator==
//版本二：提供用户自行指定的操作规则comp
//注意：这里也有偏特化的知识
/*函数功能：Searches the range [first1,last1) for the last occurrence of the sequence defined by [first2,last2), 
and returns an iterator to its first element, or last1 if no occurrences are found.
函数原型：
equality (1)：版本一	
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2,
                              BinaryPredicate pred);
*/
//对外接口的版本一
template <class _ForwardIter1, class _ForwardIter2>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  //首先通过iterator_traits萃取出first1和first2的迭代器类型
  //根据不同的迭代器类型调用不同的函数
  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2));
}
//对外接口的版本一
template <class _ForwardIter1, class _ForwardIter2, 
          class _BinaryPredicate>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2,
         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  //首先通过iterator_traits萃取出first1和first2的迭代器类型
  //根据不同的迭代器类型调用不同的函数
  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2),
                    __comp);
}
//find_end函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_end
	#include <vector>       // std::vector

	bool myfunction (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[] = {1,2,3,4,5,1,2,3,4,5};
	  std::vector<int> haystack (myints,myints+10);

	  int needle1[] = {1,2,3};

	  // using default comparison:
	  std::vector<int>::iterator it;
	  it = std::find_end (haystack.begin(), haystack.end(), needle1, needle1+3);

	  if (it!=haystack.end())
		std::cout << "needle1 last found at position " << (it-haystack.begin()) << '\n';

	  int needle2[] = {4,5,1};

	  // using predicate comparison:
	  it = std::find_end (haystack.begin(), haystack.end(), needle2, needle2+3, myfunction);

	  if (it!=haystack.end())
		std::cout << "needle2 last found at position " << (it-haystack.begin()) << '\n';

	  return 0;
	}
	Output:
	needle1 found at position 5
	needle2 found at position 3
*/
```

参考：https://www.kancloud.cn/digest/stl-sources/177294