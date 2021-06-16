# 前言

本节所分析的算法是在`stl_numeric.h`中，本节以源码 + 例子的形式一起分析这些实现的操作。

## 数值算法分析

### 1、accumulate

功能：将每个元素值累加到初值 init。

要求传入两个`InputIterator`类型的迭代器和一个初始化值, 第二个版本还支持在传入一个仿函数或函数.

源码：

```c++
//
//版本一
template <class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for (; first != last; ++first) {
        init = init + *first;
    }
    return init;
}
//版本二  对每一个元素执行二元操作
template <class InputIterator, class T,class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op){
    for (; first != last; ++first) {
        init = binary_op(init, *first);
    }
    return init;
}
```

实例：

```c++
// 函数
inline int  binary(int a1, int a2) {
	return a1 - a2;
}
// 仿函数
struct mybinary {
	int operator() (const int a1, const int a2)
	{
		return a1 + a2;
	}
}mybinary;

int main() {
	int a[4]= {1, 2, 3, 4};
	int sum = 0;
	sum = accumulate(a, a+4, sum);	// 版本一
	for(const auto &i : a)
		cout << i << " ";	// 1 2 3 4 
	cout << sum;	// 10

	sum = accumulate(a, a+4, sum, binary);	// 版本二, 函数
	cout << sum;	// 0 10-1=9;9-2=7;7-3=4;4-4=0

	sum = accumulate(a, a+4, sum, mybinary);	// 版本二, 仿函数
	cout << sum;	// 10

	exit(0);
}
```

### 2、inner_product

两个版本.

版本一 : 两个`InputIterator`类型的迭代器, 一个迭代器(所有的数据 * first2), 一个初始化值.

```c++
template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init) {
  for ( ; first1 != last1; ++first1, ++first2)
    init = init + (*first1 * *first2);
  return init;
}
```

版本二 : 比版本一多接受两个函数或仿函数, 先执行`binary_op2`操作再执行`binary_op1`. 与上面的accumulate一样。

```c++
template <class InputIterator1, class InputIterator2, class T,
          class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init, BinaryOperation1 binary_op1,
                BinaryOperation2 binary_op2) {
  for ( ; first1 != last1; ++first1, ++first2)
    init = binary_op1(init, binary_op2(*first1, *first2));
  return init;
}
```

### 3、partial_sum

两个版本. 功能 : 将传入的`InputIterator`两个迭代器范围内的值进行局部求和. 即 y0 = x0; y1 = x0 + x1; ...

版本一 : 默认操作

```c++
template <class InputIterator, class OutputIterator, class T>
OutputIterator __partial_sum(InputIterator first, InputIterator last,
                             OutputIterator result, T*) {
  T value = *first;
    // 跳过x0, 从x1开始局部求和
  while (++first != last) {
    value = value + *first;
    *++result = value;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result) {
  if (first == last) return result;
  *result = *first;
  return __partial_sum(first, last, result, value_type(first));
}
```

版本二 : 比版本一多了一个二元操作。

```c++
template <class InputIterator, class OutputIterator, class T,
          class BinaryOperation>
OutputIterator __partial_sum(InputIterator first, InputIterator last,
                             OutputIterator result, T*,
                             BinaryOperation binary_op) {
  T value = *first;
	// 跳过x0, 从x1开始局部求和
  while (++first != last) {
    value = binary_op(value, *first);
    *++result = value;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result, BinaryOperation binary_op) {
  if (first == last) return result;
  *result = *first;
  return __partial_sum(first, last, result, value_type(first), binary_op);
}
```

实例：

```c++
inline int partial_inc(int a1, int a2) {
	return a2+2;
}

int main() {
	int a[4] = {1, 2, 3, 4 };
	int a2[4];

	partial_sum(a, a+sizeof(a)/sizeof(int), a2);	// 版本一
	for(const auto &i : a)
    cout << i << " ";	// 1 2 3 4

	for(const auto &i : a2)
		cout << i << " ";	// 1 3 6 10 
	
	partial_sum(a, a+sizeof(a)/sizeof(int), a2, partial_inc);	// 版本二
	for(const auto &i : a2)
		cout << i << " ";	// 1 4 5 6 

	exit(0);
}
```

### 4、adjacent_difference

两个版本. 功能 : 将传入的`InputIterator`两个迭代器范围内的值进行局部相减. 即 y0 = x0; y1 = x0 - x1 ...

版本一

```c++
template <class InputIterator, class OutputIterator, class T>
OutputIterator __adjacent_difference(InputIterator first, InputIterator last, 
                                     OutputIterator result, T*) {
  T value = *first;
  while (++first != last) {
    T tmp = *first;
    *++result = tmp - value;	// 进行相减
    value = tmp;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
                                   OutputIterator result) {
  if (first == last) return result;
  *result = *first;
  return __adjacent_difference(first, last, result, value_type(first));
}
```

版本二 : 指定二元操作

```c++
template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator __adjacent_difference(InputIterator first, InputIterator last, 
                                     OutputIterator result, T*,
                                     BinaryOperation binary_op) {
  T value = *first;
  while (++first != last) {
    T tmp = *first;
    *++result = binary_op(tmp, value);	
    value = tmp;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result,
                                   BinaryOperation binary_op) {
  if (first == last) return result;
  *result = *first;
  return __adjacent_difference(first, last, result, value_type(first),
                               binary_op);
}
```

实例：

```c++
struct adjacent{
	int operator() (const int tmp, const int a) { return tmp - a - a; }
}adjacent;

int main()
{
	int a[4] = {1, 2, 3, 4};
	int a2[4];

	for(const auto &i : a)
		cout << i <<  " ";	// 1 2 3 4 

	adjacent_difference(a,  a+4, a2);
	for(const auto &i : a2)
		cout << i <<  " ";	// 1 1 1 1
    
	partial_sum(a2, a2+4, a2);
	for(const auto &i : a2)
		cout << i <<  " ";	// 1 2 3 4 
    
	adjacent_difference(a, a+4, a2, adjacent);
	for(const auto &i : a2)
		cout << i <<  " ";	// 1 0 -1 -2

	exit(0);
}
```

从实例可以看出来 `adjacent_difference`与 `partial_sum`相结合可以恢复。

### 5、power

一个版本：幂次方。如果指定为乘法运算，则当n >= 0 时传回 x^n

```c++
// Returns x ** n, where n >= 0.  Note that "multiplication"
//  is required to be associative, but not necessarily commutative.   
template <class T, class Integer, class MonoidOperation>
T power(T x, Integer n, MonoidOperation op) {
  if (n == 0)
    return identity_element(op);
  else {
    while ((n & 1) == 0) {
      n >>= 1;
      x = op(x, x);
    }

    T result = x;
    n >>= 1;
    while (n != 0) {
      x = op(x, x);
      if ((n & 1) != 0)
        result = op(result, x);
      n >>= 1;
    }
    return result;
  }
}

template <class T, class Integer>
inline T power(T x, Integer n) {
  return power(x, n, multiplies<T>());
}
```

打过竞赛的同学，看到这个函数肯定不陌生，在 C++ 的快速幂就是一样的逻辑。

这里用到了一个技巧：

- 当 b 为偶数时，a^b 可以转为 a^2的 b/2次方。
- 当 b 为奇数时，a^b 可以转为 a^2 的 b/2次方，再乘以 a。

像这样不断递归下去，每次 n 都减半，于是可以在 n(logn) 时间内完成幂运算。

```c++
ll q_pow(ll x,ll n,ll m){
	if(n == 0)	return 1;
	ll res = q_pow(x * x % m,n/2,m);
	if(n & 1)	res = res * x % m;
	return res;
}
```

### 6、iota 

设定某个区间的内容, 使其每个元素从指定值value开始, 呈现递增。

```c++
template <class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value) {
  while (first != last) *first++ = value++;
}
```

实例：

```c++
int main() {
	int a[4] = {1, 2, 3, 4};
	iota(a+1, a+4, 10);
	for(const auto &i : a)
		cout << i << ' '; // 1 10 11 12
	return 0;
}
```

### 总结

本节对`numeric.h`中的算法进行了分析，这些都是很简单的函数实现，但是大都提供了两个版本，可接受二元操作的函数，这由用户定义，也就提高了STL的灵活性。

参考：

《STL源码剖析》-侯捷

https://github.com/FunctionDou/STL/