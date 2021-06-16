#include <string>

template <class T, class Alloc = alloc>
class vector {
public:
	vector() : start(0), finish(0), end_of_storage(0) {}		// 默认构造函数
	explicit vector(size_type n) { fill_initialize(n, T()); } 	// 必须显示的调用这个构造函数, 接受一个值
	vector(size_type n, const T& value) { fill_initialize(n, value); } // 接受一个大小和初始化值. int和long都执行相同的函数初始化
	vector(int n, const T& value) { fill_initialize(n, value); }	
	vector(long n, const T& value) { fill_initialize(n, value); }
	vector(const vector<T, Alloc>& x); 							// 接受一个vector参数的构造函数
public:
    typedef T value_type;					// 定义vector自身的嵌套型别
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    
    typedef value_type* iterator;			// 定义迭代器, 这里就只是一个普通的指针
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
public:
    iterator begin()    {  return start;  }
    iterator end()  {   return finish;  }
    size_type size() const  {   return size_type(end() - begin());  }
    size_type max_size() const { return size_type(-1) / sizeof(T); }	// 最大能存储的元素个数
    size_type capacity() const  {   return size_type(end_of_storage - begin()); }
    bool empty() const  {   return begin() == end();    }
    reference operator[](size_type n)   {   return *(begin() + n);  }
    reference front()   {    return *begin();    }
    reference back()    {  return *(end() - 1);    }
  protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;	// 设置其空间配置器
    iterator start;							// 表示目前使用空间的头
    iterator finish;						// 表示目前使用空间的尾
    iterator end_of_storage;				// 表示目前可用空间的尾
    ...
};

void fill_initialize(size_type n, const T& value) {
    start = allocate_and_fill(n, value);	// 初始化并初始化值
    finish = start + n;
    end_of_storage = finish;
}

// 调用默认的第二配置器分配内存, 分配失败就释放所分配的内存
iterator allocate_and_fill(size_type n, const T& x) {
    iterator result = data_allocator::allocate(n);	// 申请n个元素的线性空间.
    __STL_TRY	// 对整个线性空间进行初始化, 如果有一个失败则删除全部空间并抛出异常.
    {
         uninitialized_fill_n(result, n, x);	
         return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
}


