## **C++ STL set 相关算法**

STL一共提供了四种与 set 集合相关的算法，分别是并集，交集，差集，对称差集。

但要注意的是：此处的 set 不同于数学中的集合。数学中的集合允许元素以任意次数、任意次序出现，但此处的不允许元素重复出现，而且所有元素按序出现。这四种算法处理的结构也是有序的。

有关 set 的四个算法提供两个版本。版本一接收五个迭代器，前四个分别两两确定一个集合区间，还有一个迭代器用于处理运算的结果。版本二和版本一的不同之处在于，版本二可以自定义元素的比较方式（模板参数定制比较器）。它们的返回值相同，都返回运算结果的尾端迭代器。

这四个算法虽然以 set 命名，但实际上也不局限于 set, 只要是元素有序出现，且提供迭代器的容器都可以用此算法，这一点才稍后的源码中可以看到。

## 1**、set_union**

算法 `set_union` 提供两个集合 `S1`，`S2` 之并集，即 `S1 U S2` 。

`set_union` 是一种稳定的操作，输入区间内的每个元素的相对顺序不会改变。

STL 提供两个版本，差异在于如何定义某个元素小于另一个元素：

- 第一个版本使用 operator< 进行比较。
- 第二个版本采用仿函数 comp 进行比较。

源码

```c++
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result) {
    //当两个区间都尚未到达尾端时，执行以下操作
    while(first1 != last1 && first2 != last2) {
        //在两个区间内分别移动迭代器 首先将元素较小者(a)记录于目标区
        //然后移动 a 区迭代器前进；同时保持另一个迭代器不动；进行下一次比较
        //记录小值，迭代器移动...直到两区中有一区达到尾端
        //如果元素相等 可以任取一个迭代器所指元素 然后同时移动两个迭代器
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        } else if (*first2 < *first2) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    //只要两个区间有一个到达尾端就结束while循环
    //将其中一个没有到达尾端的区间的剩余元素全部拷贝到目的区间。
    return std::copy(first2, last2, std::copy(first1, last1, result));
}
```

## **2、set_intersection**

算法 `set_intersection` 提供两个集合 `S1`，`S2` 之交集，即 `S1 S2` 。

`set_intersection` 是一种稳定的操作，输入区间内的每个元素的相对顺序不会改变。

STL 提供两个版本，差异在于如何定义某个元素小于另一个元素：

- 第一个版本使用 operator< 进行比较。
- 第二个版本采用仿函数 comp 进行比较。

源码

```c++
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result) {
    //当两个区间都尚未到达尾端时，执行以下操作
    while(first1 != last1 && first2 != last2) {
        //在两个区间内分别移动迭代器 取交集也很好理解 当遇到相等的元素才记录
        if (*first1 < *first2) {
            ++first1;
        } else if (*first2 < *first2) {
            ++first2;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}
```

## **3、set_difference**

算法 `set_difference` 提供两个集合 `S1`，`S2` 之差集，即 `S1 - S2` 。

`set_difference` 是一种稳定的操作，输入区间内的每个元素的相对顺序不会改变。

STL 提供两个版本，差异在于如何定义某个元素小于另一个元素：

- 第一个版本使用 operator< 进行比较。
- 第二个版本采用仿函数 comp 进行比较。

源码

```c++
//差集：计算 存在于[first1, last1) 且不存在于 [first2,last2)所有元素
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result) {
    //当两个区间都尚未到达尾端时，执行以下操作
    while(first1 != last1 && first2 != last2) {
        //在两个区间内分别移动迭代器 当第一区间的元素等于第二区间 就让两区间同时前进
        //当第一区间元素大于第二区间的元素 就让第二区间往前走
        //有了上面两步就能保证当第一区间的元素小于第二区间时 第一区间的元素只存在于第一区间而不存在第二区间
        //于是将它记录于目标区间
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (*first2 < *first2) {
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
    }
    return std::copy(first1, last1, result);
}
```

## **4、set_symmetric_difference**

算法 `set_symmetric_difference` 提供两个集合 `S1`，`S2` 之对称差集，即 `(S1 - S2) U (S2 - S1)` 。

`set_symmetric_difference` 是一种稳定的操作，输入区间内的每个元素的相对顺序不会改变。

由于 `S1`，`S2` 内的每个元素都不需要唯一，因此如果某个值在 `S1` 出现 `n` 次，在 `S2` 中出现 `m` 次，那么改值在输出区间出现 `|n-m|` 次。

STL 提供两个版本，差异在于如何定义某个元素小于另一个元素：

- 第一个版本使用 operator< 进行比较。
- 第二个版本采用仿函数 comp 进行比较。

源码

```c++
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result) {
    //当两个区间都尚未到达尾端时，执行以下操作
    while(first1 != last1 && first2 != last2) {
        //在两个区间内分别移动迭代器 当两个区间元素相等 就让两区间同时前进
        //当两个区间元素不等，记录较小值于目标区间 并让较小值区间继续前进
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (*first2 < *first2) {
            *result = *first2;
            ++first2;
            ++result;
        } else { // *first1 == *first2
            ++first1;
            ++first2;
        }
    }
    return std::copy(first2, last2, std::copy(first1, last1, result));
}
```

### 5、merge 合并算法源码剖析

包括函数 merge、inplace_merge。

```c++
// merge, with and without an explicitly supplied comparison function.
//将两个已排序的区间[first1,last1)和区间[first2,last2)合并
/*
函数功能：Combines the elements in the sorted ranges [first1,last1) and [first2,last2), 
into a new range beginning at result with all its elements sorted.

函数原型：
default (1)	：版本一
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator merge (InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result);
custom (2)	：版本二
	template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
	OutputIterator merge (InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result, Compare comp);
*/
//版本一：
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //两个序列都尚未到达尾端，则执行while循环
  /*
  情况1：若序列二元素较小,则记录到目标区，且移动序列二的迭代器，但是序列一的迭代器不变.
  情况2：若序列一元素较小或相等,则记录到目标区，且移动序列一的迭代器，但是序列二的迭代器不变.
  最后：把剩余元素的序列复制到目标区
  */
  while (__first1 != __last1 && __first2 != __last2) {
	  //情况1
    if (*__first2 < *__first1) {//若序列二元素较小
      *__result = *__first2;//将元素记录到目标区
      ++__first2;//移动迭代器
    }
	//情况2
    else {//若序列一元素较小或相等
      *__result = *__first1;//将元素记录到目标区
      ++__first1;//移动迭代器
    }
    ++__result;//更新目标区位置，以便下次记录数据
  }
  //若有序列到达尾端，则把没到达尾端的序列剩余元素复制到目标区
  //此时，区间[first1,last1)和区间[first2,last2)至少一个必定为空
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
//版本二
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter1>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
//merge函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::merge, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);

	  std::sort (first,first+5);
	  std::sort (second,second+5);
	  std::merge (first,first+5,second,second+5,v.begin());

	  std::cout << "The resulting vector contains:";
	  for (std::vector<int>::iterator it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	The resulting vector contains: 5 10 10 15 20 20 25 30 40 50
*/

// inplace_merge and its auxiliary functions. 
//版本一的辅助函数，无缓冲区的操作
template <class _BidirectionalIter, class _Distance>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (*__middle < *__first)
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle,
                         __len11, __len22);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22);
}

template <class _BidirectionalIter, class _Distance, class _Compare>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2,
                            _Compare __comp) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (__comp(*__middle, *__first))
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle, __len11, __len22,
                         __comp);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22, __comp);
}
//版本一的辅助函数，有缓冲区的操作
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _Distance>
_BidirectionalIter1 __rotate_adaptive(_BidirectionalIter1 __first,
                                      _BidirectionalIter1 __middle,
                                      _BidirectionalIter1 __last,
                                      _Distance __len1, _Distance __len2,
                                      _BidirectionalIter2 __buffer,
                                      _Distance __buffer_size) {
  _BidirectionalIter2 __buffer_end;
  if (__len1 > __len2 && __len2 <= __buffer_size) {//缓冲区足够放置序列二
    __buffer_end = copy(__middle, __last, __buffer);
    copy_backward(__first, __middle, __last);
    return copy(__buffer, __buffer_end, __first);
  }
  else if (__len1 <= __buffer_size) {//缓冲区足够放置序列一
    __buffer_end = copy(__first, __middle, __buffer);
    copy(__middle, __last, __first);
    return copy_backward(__buffer, __buffer_end, __last);
  }
  else//若缓冲区仍然不够，则调用STL算法rotate，不使用缓冲区
    return rotate(__first, __middle, __last);
}

template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (*__last2 < *__last1) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}

template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3, class _Compare>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result,
                                     _Compare __comp) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (__comp(*__last2, *__last1)) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}
//版本一的辅助函数，有缓冲区的操作
template <class _BidirectionalIter, class _Distance, class _Pointer>
void __merge_adaptive(_BidirectionalIter __first,
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
	  //case1：把序列一放在缓冲区
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
	//直接调用归并函数merge
    merge(__buffer, __buffer_end, __middle, __last, __first);
  }
  else if (__len2 <= __buffer_size) {
	  //case2：把序列二放在缓冲区
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last);
  }
  else {//case3：缓冲区不足放置任何一个序列
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {//若序列一比较长
      __len11 = __len1 / 2;//计算序列一的一半
      advance(__first_cut, __len11);//让first_cut指向序列一的中间位置
	  //找出*__first_cut在[middle,last)区间中的第一个不小于*__first_cut的元素位置
      __second_cut = lower_bound(__middle, __last, *__first_cut);
	  //计算middle到__second_cut之间的距离，保存在__len22
      distance(__middle, __second_cut, __len22); 
    }
    else {//若序列二比较长
      __len22 = __len2 / 2;//计算序列二的一半
      advance(__second_cut, __len22);//让__second_cut指向序列二的中间位置
	  //找出*__second_cut在[first,middle)区间中的第一个大于*__second_cut的元素位置
      __first_cut = upper_bound(__first, __middle, *__second_cut);
	  //计算__first到__first_cut之间的距离，保存在__len11
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
	//对左半段递归调用
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size);
	//对右半段递归调用
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size);
  }
}

template <class _BidirectionalIter, class _Distance, class _Pointer,
          class _Compare>
void __merge_adaptive(_BidirectionalIter __first, 
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size,
                      _Compare __comp) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
    merge(__buffer, __buffer_end, __middle, __last, __first, __comp);
  }
  else if (__len2 <= __buffer_size) {
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last,
                     __comp);
  }
  else {
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {
      __len11 = __len1 / 2;
      advance(__first_cut, __len11);
      __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
      distance(__middle, __second_cut, __len22);   
    }
    else {
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size, __comp);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size, __comp);
  }
}
//版本一的辅助函数
template <class _BidirectionalIter, class _Tp, class _Distance>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);//计算序列一的长度
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);//计算序列二的长度

  //使用暂时缓冲区
  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)//若缓冲区配置失败
	  //则调用不使用缓冲区的合并操作
    __merge_without_buffer(__first, __middle, __last, __len1, __len2);
  else//若分配成功
	  //则调用具有缓冲区的合并操作
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()));
}

template <class _BidirectionalIter, class _Tp, 
          class _Distance, class _Compare>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*,
                                _Compare __comp) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)
    __merge_without_buffer(__first, __middle, __last, __len1, __len2, __comp);
  else
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()),
                     __comp);
}
//将两个已排序的序列[first,middle)和[middle,last)合并成单一有序序列.
//若原来是增序，现在也是递增排序，若原来是递减排序，现在也是递减排序
/*
函数功能：Merges two consecutive sorted ranges: [first,middle) and [middle,last), 
putting the result into the combined sorted range [first,last).
函数原型：
default (1)	：版本一
	template <class BidirectionalIterator>
	void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
                      BidirectionalIterator last);
custom (2)	：版本二
	template <class BidirectionalIterator, class Compare>
	void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
                      BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __middle || __middle == __last)//若有空序列，则之间返回
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}
//版本二
template <class _BidirectionalIter, class _Compare>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last, _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
           typename iterator_traits<_BidirectionalIter>::value_type,
           typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __middle || __middle == __last)
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first),
                      __comp);
}
//inplace_merge函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::inplace_merge, std::sort, std::copy
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);
	  std::sort (second,second+5);

	  it=std::copy (first, first+5, v.begin());
		 std::copy (second,second+5,it);

	  std::inplace_merge (v.begin(),v.begin()+5,v.end());

	  std::cout << "The resulting vector contains:";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output：
	The resulting vector contains: 5 10 10 15 20 20 25 30 40 50
*/
```

参考：https://www.kancloud.cn/digest/stl-sources/177295