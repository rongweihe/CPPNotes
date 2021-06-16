## 1、accumulate

功能：将每个元素值累加到初值 init

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

