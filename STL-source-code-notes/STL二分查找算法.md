# 二分查找

### 前言

`stl_algo.h`提供了`lower_bound`和`upper_bound`两种查找算法。

前者是找出第一个满足条件的迭代器；

后者则是找出最后一个满足条件的迭代器。

两者结合就能知道该容器中重复的个数.

### 二分查找分析

#### lower_bound

**ForwardIterator**版本

```c++
// 版本一
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value)
{
  return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
                              const T& value, Distance*,
                              forward_iterator_tag) {
  Distance len = 0;
  distance(first, last, len);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;	// 相当于除2
      // middle为区间的起始位置
    middle = first;
      // 设置middle为区间的中间值
    advance(middle, half);
      // 将value值与中间值比较, 即是二分查找, 若中间值小于value, 则继续查找右半部分
    if (*middle < value) {
      first = middle;
      ++first;
      len = len - half - 1;
    }
    else
      len = half;
  }
  return first;
}

// 版本二
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp) {
  return __lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
                              const T& value, Compare comp, Distance*,
                              forward_iterator_tag) {
  Distance len = 0;
  distance(first, last, len);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first;
      // 设置middle为区间的中间值
    advance(middle, half);
      // 满足的条件的进行比赛
    if (comp(*middle, value)) {
      first = middle;
      ++first;
      len = len - half - 1;
    }
    else
      len = half;
  }
  return first;
}
```

**RandomAccessIterator** 版本

```c++
// 版本一
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first,
                                   RandomAccessIterator last, const T& value,
                                   Distance*, random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;
	// 二分法
  while (len > 0) {
    half = len >> 1;	// 相当于除2
      // 设置middle为区间的中间值. 
      // 这里直接求出middle的值, 更加的快速
    middle = first + half;
    if (*middle < value) {
      first = middle + 1;
      len = len - half - 1;
    }
    else
      len = half;
  }
  return first;
}
// 版本二
template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first,
                                   RandomAccessIterator last,
                                   const T& value, Compare comp, Distance*,
                                   random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;

  while (len > 0) {
    half = len >> 1;
      // 设置middle为区间的中间值. 
      // 这里直接求出middle的值, 更加的快速
    middle = first + half;
    if (comp(*middle, value)) {
      first = middle + 1;
      len = len - half - 1;
    }
    else
      len = half;
  }
  return first;
}
```

#### upper_bound

这里就只分析`ForwardIterator`就行了, `RandomAccessIterator`也跟上面一样的操作

```c++
// 版本一
template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value) {
  return __upper_bound(first, last, value, distance_type(first),
                       iterator_category(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last,
                              const T& value, Compare comp, Distance*,
                              forward_iterator_tag) {
  Distance len = 0;
  distance(first, last, len);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;	// 相当于除2
      // middle为区间的起始位置
    middle = first;
      // 设置middle为区间的中间值
    advance(middle, half);
      // 将value值与中间值比较, 即是二分查找, 若中间值小于value, 则继续查找右半部分
    if (comp(value, *middle))
      len = half;
    else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}
// 版本二
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp) {
  return __upper_bound(first, last, value, comp, distance_type(first),
                       iterator_category(first));
}
```

### 总结

学习 STL 总能让人不禁认识到很多东西，学到很多细节的处理与完善。

参考：https://github.com/FunctionDou/STL