### 前言

本文讲解的是 STL 算法中的 permutation 排列组合算法，根据输入序列，排列出下一个排列组合或前一个排列组合。

| 函数                             | 作用                                                |
| -------------------------------- | --------------------------------------------------- |
| `next_permutation(beg,end)`      | 取出[`beg`,`end`)内的下一个排列                     |
| `next_permutation(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`next_permutation()` |
| `prev_permutation(beg,end)`      | 取出[`beg`,`end`)内的上一个排列                     |
| `prev_permutation(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`prev_permutation()` |

### 排列组合源码剖析

```c++
// next_permutation and prev_permutation, with and without an explicitly 
// supplied comparison function.
//next_permutation获取[first,last)区间所标示序列的下一个排列组合，若果没有下一个排序组合，则返回false;否则返回true;
/*
函数功能：Rearranges the elements in the range [first,last) into the next lexicographically greater permutation.
函数原型：
default (1)	：版本一采用 less-than 操作符
	template <class BidirectionalIterator>
	bool next_permutation (BidirectionalIterator first,
                         BidirectionalIterator last);
custom (2)	：版本二采用仿函数 comp 决定
	template <class BidirectionalIterator, class Compare>
	bool next_permutation (BidirectionalIterator first,
                         BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;//若为空，则返回false
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)//区间只有一个元素
    return false;
  //若区间元素个数不小于两个
  __i = __last;//i指向尾端
  --__i;//不断后移

  for(;;) {
	  //下面两行是让ii和i成为相邻的元素
	  //其中i为第一个元素，ii为第二个元素
    _BidirectionalIter __ii = __i;//
    --__i;
	//以下在相邻元素判断
    if (*__i < *__ii) {//若前一个元素小于后一个元素，
		//则再从最尾端开始往前检查，找出第一个大于*i的元素，令该元素为*j，将*i和*j交换
		//再将ii之后的所有元素颠倒排序
      _BidirectionalIter __j = __last;//令j指向最尾端
      while (!(*__i < *--__j))//由尾端往前检查，直到遇到比*i大的元素
        {}
      iter_swap(__i, __j);//交换迭代器i和迭代器j所指的元素
      reverse(__ii, __last);//将ii之后的元素全部逆向重排
      return true;
    }
    if (__i == __first) {//进行到最前面
      reverse(__first, __last);//整个区间全部逆向重排
      return false;
    }
  }
}
//版本二
template <class _BidirectionalIter, class _Compare>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__i, *__ii)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*__i, *--__j))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
//next_permutation函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::next_permutation, std::sort

	int main () {
	  int myints[] = {1,2,3,4};

	  std::sort (myints,myints+4);

	  std::cout << "The 3! possible permutations with 3 elements:\n";
	  do {
		std::cout << myints[0] << ' ' << myints[1] << ' ' << myints[2] <<' ' << myints[3]<< '\n';
	  } while ( std::next_permutation(myints,myints+4) );
 
	  //std::next_permutation(myints,myints+4);
	  std::cout << "After loop: " << myints[0] << ' ' << myints[1] << ' ' << myints[2] << ' ' << myints[3]<<'\n';

	  return 0;
	}
	Output:
	The 3! possible permutations with 3 elements:
	1 2 3 4
	1 2 4 3
	1 3 2 4
	1 3 4 2
	1 4 2 3
	1 4 3 2
	2 1 3 4
	2 1 4 3
	2 3 1 4
	2 3 4 1
	2 4 1 3
	2 4 3 1
	3 1 2 4
	3 1 4 2
	3 2 1 4
	3 2 4 1
	3 4 1 2
	3 4 2 1
	4 1 2 3
	4 1 3 2
	4 2 1 3
	4 2 3 1
	4 3 1 2
	4 3 2 1
	After loop: 1 2 3 4
*/

//prev_permutation获取[first,last)区间所标示序列的上一个排列组合，若果没有上一个排序组合，则返回false;否则返回true;
/*
函数功能：Rearranges the elements in the range [first,last) into the previous lexicographically-ordered permutation.
函数原型：
default (1)	：版本一采用less-than操作符
	template <class BidirectionalIterator>
	bool prev_permutation (BidirectionalIterator first,
                         BidirectionalIterator last );
custom (2)	：版本二采用仿函数comp
	template <class BidirectionalIterator, class Compare>
	bool prev_permutation (BidirectionalIterator first,
                         BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;//若区间为空，返回false
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)//区间只有一个元素
    return false;//返回false
  //若区间元素个数不小于两个
  __i = __last;
  --__i;

  for(;;) {
	  //下面两行是让ii和i成为相邻的元素
	  //其中i为第一个元素，ii为第二个元素
    _BidirectionalIter __ii = __i;
    --__i;
	//以下在相邻元素判断
    if (*__ii < *__i) {//若前一个元素大于后一个元素，
		//则再从最尾端开始往前检查，找出第一个小于*i的元素，令该元素为*j，将*i和*j交换
		//再将ii之后的所有元素颠倒排序
      _BidirectionalIter __j = __last;//令j指向最尾端      
      while (!(*--__j < *__i))//由尾端往前检查，直到遇到比*i小的元素
        {}
      iter_swap(__i, __j); //交换迭代器i和迭代器j所指的元素
      reverse(__ii, __last);//将ii之后的元素全部逆向重排
      return true;
    }
    if (__i == __first) {//进行到最前面
      reverse(__first, __last);//把区间所有元素逆向重排
      return false;
    }
  }
}
//版本二
template <class _BidirectionalIter, class _Compare>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__ii, *__i)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*--__j, *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
//prev_permutation函数举例
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::next_permutation, std::sort, std::reverse

	int main () {
	  int myints[] = {1,2,3};

	  std::sort (myints,myints+3);
	  std::reverse (myints,myints+3);

	  std::cout << "The 3! possible permutations with 3 elements:\n";
	  do {
		std::cout << myints[0] << ' ' << myints[1] << ' ' << myints[2] << '\n';
	  } while ( std::prev_permutation(myints,myints+3) );

	  std::cout << "After loop: " << myints[0] << ' ' << myints[1] << ' ' << myints[2] << '\n';

	  return 0;
	}
	Output:
	The 3! possible permutations with 3 elements:
	3 2 1
	3 1 2
	2 3 1
	2 1 3
	1 3 2
	1 2 3
	After loop: 3 2 1
*/
```

参考：https://www.kancloud.cn/digest/stl-sources/177297