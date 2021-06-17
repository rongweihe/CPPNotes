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