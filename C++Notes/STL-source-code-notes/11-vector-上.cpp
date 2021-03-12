<!-- ## STL 源码剖析之 vector 

`vector` 是一种常用的容器，基本能够支持任何类型的对象，同时也是一个可以动态增长的数组，它的背后蕴含着怎样精妙的设计呢？话不多说，马上就来分析关于`vector`是怎么实现这些功能.

 -->

template <class T, class Alloc = alloc>
class vector 
{
public:
  	// 定义vector自身的嵌套型别
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    // 定义迭代器, 这里就只是一个普通的指针
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    ...
  protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;	// 设置其空间配置器
    iterator start;				// 使用空间的头
    iterator finish;			// 使用空间的尾
    iterator end_of_storage;	// 可用空间的尾
    ...
};