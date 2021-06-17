### 前言

本文介绍的 STL 算法中的 remove 删除算法，源码中介绍了函数 remove、remove_copy、remove_if、remove_copy_if、unique、unique_copy。

并对这些函数的源码进行详细的剖析，并适当给出使用例子，具体详见下面源码剖析。

### remove 移除算法源码剖析

```c++
// remove, remove_if, remove_copy, remove_copy_if

//移除[first,last)区间内所有与value值相等的元素，并不是真正的从容器中删除这些元素(原容器的内容不会改变)
//而是将结果复制到一个以result为起始位置的容器中。新容器可以与原容器重叠
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter remove_copy(_InputIter __first, _InputIter __last,
                        _OutputIter __result, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first)//遍历容器
    if (!(*__first == __value)) {//如果不相等
      *__result = *__first;//赋值给新容器
      ++__result;//新容器前进一个位置
    }
  return __result;
}
//移除[first,last)区间内被仿函数pred判断为true的元素,并不是真正的从容器中删除这些元素(原容器的内容不会改变)
//而是将结果复制到一个以result为起始位置的容器中。新容器可以与原容器重叠
template <class _InputIter, class _OutputIter, class _Predicate>
_OutputIter remove_copy_if(_InputIter __first, _InputIter __last,
                           _OutputIter __result, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)//遍历容器
    if (!__pred(*__first)) {//若pred判断为false
      *__result = *__first;//赋值给新容器
      ++__result;//新容器前进一个位置
    }
  return __result;
}
//移除[first,last)区间内所有与value值相等的元素,该操作不会改变容器大小，只是容器中元素值改变
//即移除之后，重新整理容器的内容
template <class _ForwardIter, class _Tp>
_ForwardIter remove(_ForwardIter __first, _ForwardIter __last,
                    const _Tp& __value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __first = find(__first, __last, __value);//利用顺序查找找出第一个与value相等的元素
  _ForwardIter __i = __first;
  //下面调用remove_copy
  return __first == __last ? __first 
                           : remove_copy(++__i, __last, __first, __value);
}
//移除[first,last)区间内所有被pred判断为true的元素,该操作不会改变容器大小，只是容器中元素值改变
//即移除之后，重新整理容器的内容
template <class _ForwardIter, class _Predicate>
_ForwardIter remove_if(_ForwardIter __first, _ForwardIter __last,
                       _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
               typename iterator_traits<_ForwardIter>::value_type);
  __first = find_if(__first, __last, __pred);//利用顺序查找找出第一个与value相等的元素
  _ForwardIter __i = __first;
  //下面调用remove_copy_if
  return __first == __last ? __first 
                           : remove_copy_if(++__i, __last, __first, __pred);
}
//上面四个移除函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::remove

	bool IsOdd (int i) { return ((i%2)==1); }

	int main () {
	  int myints[] = {10,20,31,30,20,11,10,20};      // 10 20 31 30 20 11 10 20

	  std::vector<int> myvector (8);
	  std::remove_copy (myints,myints+8,myvector.begin(),20); // 10 31 30 11 10 0 0 0
	  std::cout << "myvector contains:";
	  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';
  
	  // bounds of range:
	  int* pbegin = myints;                          // ^
	  int* pend = myints+sizeof(myints)/sizeof(int); // ^                       ^
	  pend = std::remove (pbegin, pend, 20);         // 10 31 30 11 10 ?  ?  ?
													 // ^              ^
	  std::cout << "range contains:";
	  for (int* p=pbegin; p!=pend; ++p)
		std::cout << ' ' << *p;
	  std::cout << '\n';
  
	  std::vector<int> myvector2 (7);
	  std::remove_copy_if (myints,myints+7,myvector2.begin(),IsOdd);
	  std::cout << "myvector2 contains:";
	  for (std::vector<int>::iterator it=myvector2.begin(); it!=myvector2.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';
  
	  pend = std::remove_if (pbegin, pend, IsOdd);   // 10 30 10 ? ? ? ? ?
													 // ^       ^
	  std::cout << "the range contains:";
	  for (int* p=pbegin; p!=pend; ++p)
		std::cout << ' ' << *p;
	  std::cout << '\n';  

	  return 0;
	}
	Output:
	myvector contains: 10 31 30 11 10 0 0 0
	range contains: 10 31 30 11 10
	myvector2 contains: 10 30 10 10 0 0 0
	the range contains: 10 30 10
*/

// unique and unique_copy

template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result, _Tp*) {
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!(__value == *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}
//若result类型为output_iterator_tag，则调用该函数
template <class _InputIter, class _OutputIter>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result, 
                                 output_iterator_tag) {
		//判断first的value_type类型，根据不同类型调用不同函数
  return __unique_copy(__first, __last, __result, __VALUE_TYPE(__first));
}
//若result类型为forward_iterator_tag，则调用该函数
template <class _InputIter, class _ForwardIter>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, forward_iterator_tag) {
  *__result = *__first;//记录第一个元素
  while (++__first != __last)//遍历区间
	  //若不存在相邻重复元素，则继续记录到目标区result
    if (!(*__result == *__first))
      *++__result = *__first;//记录元素到目标区
  return ++__result;
}
////unique_copy将区间[first,last)内元素复制到以result开头的区间上，但是如果存在相邻重复元素时，只复制其中第一个元素
//和unique一样，这里也有两个版本
/*
函数原型：
equality (1)	
	template <class InputIterator, class OutputIterator>
	OutputIterator unique_copy (InputIterator first, InputIterator last,
                              OutputIterator result);
predicate (2)	
	template <class InputIterator, class OutputIterator, class BinaryPredicate>
	OutputIterator unique_copy (InputIterator first, InputIterator last,
                              OutputIterator result, BinaryPredicate pred);
*/
//版本一
template <class _InputIter, class _OutputIter>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  if (__first == __last) return __result;
  //根据result迭代器的类型，调用不同的函数
  return __unique_copy(__first, __last, __result,
                       __ITERATOR_CATEGORY(__result));
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate,
          class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result,
                          _BinaryPredicate __binary_pred, _Tp*) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, _Tp, _Tp);
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!__binary_pred(__value, *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result,
                                 _BinaryPredicate __binary_pred,
                                 output_iterator_tag) {
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __VALUE_TYPE(__first));
}

template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, 
                           _BinaryPredicate __binary_pred,
                           forward_iterator_tag) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_ForwardIter>::value_type,
     typename iterator_traits<_InputIter>::value_type);
  *__result = *__first;
  while (++__first != __last)
    if (!__binary_pred(*__result, *__first)) *++__result = *__first;
  return ++__result;
}
//版本二
template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result,
                               _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  if (__first == __last) return __result;
  //根据result迭代器的类型，调用不同的函数
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __ITERATOR_CATEGORY(__result));
}
//移除区间[first,last)相邻连续重复的元素
//unique有两个版本
//功能：Removes all but the first element from every consecutive group of equivalent elements in the range [first,last).
/*
函数原型：
equality (1)：版本一采用operator==	
	template <class ForwardIterator>
	ForwardIterator unique (ForwardIterator first, ForwardIterator last);
predicate (2)：版本二采用pred操作	
	template <class ForwardIterator, class BinaryPredicate>
	ForwardIterator unique (ForwardIterator first, ForwardIterator last,
                          BinaryPredicate pred);
*/
//版本一
template <class _ForwardIter>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __first = adjacent_find(__first, __last);//找出第一个相邻元素的起始位置
  return unique_copy(__first, __last, __first);//调用unique_copy完成操作
}
//版本二
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last,
                    _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, 
      typename iterator_traits<_ForwardIter>::value_type,
      typename iterator_traits<_ForwardIter>::value_type);
  __first = adjacent_find(__first, __last, __binary_pred);//找出第一个相邻元素的起始位置
  return unique_copy(__first, __last, __first, __binary_pred);//调用unique_copy完成操作
}
```

参考：https://www.kancloud.cn/digest/stl-sources/177296