/*
[1]对vector的任何操作一旦引起空间重新配置指向原vector的所有迭代器就会失效

*/
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<_Tp>::other _Tp_alloc_type;

template<class T, class Alloc = alloc>
class vector {
public:
    typedef T value_type;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
protected:
    iterator start;//表示目前使用空间的头
    iterator finish;//表示目前使用空间的尾
    iterator end_of_storage;//表示目前可用空间的尾
public:
    iterator begin()    {   return start;   }
    iterator end()  {   return finish;  }
    size_type size() const  {   return size_type(end() - begin());  }
    size_type capacity() const  {   return size_type(end_of_storage - begin()); }
    bool empty() const  {   return begin() == end();    }
    reference operator[](size_type n)   {   return *(begin() + n);  }
    reference front()   {    return *begin();    }
    reference back()    {  return *(end() - 1);    }
public:
    //将尾端元素拿掉 并调整大小
    void pop_back() {
        --finish;//将尾端标记往前移动一个位置 放弃尾端元素
        destroy(finish);
    }
    //erase删除操作
    //清楚[first, last)中的所有元素
    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }
    //清楚某个位置上的元素
    iterator erase(iterator position) {
        if (position + 1 != end()) 
            copy(position + 1, finish, position);//copy 全局函数
        }      
        --finish;
        destroy(finish);
        return position;
    }
    void clear() {
        erase(begin(), end());
    }
    /*
    当调用push_back将新元素插入vector尾部的时候，首先会检查是否有备用空间 
    如果有就直接在备用空间上构造元素 并调整迭代器finish
    如果没有备用空间 就扩充空间(重新配置-移动数据-释放原空间)
    */
    void push_back(const T& x) {
        if (finish != end_of_storage) { //尚有可用空间                
            construct(finish, x);       //全局函数
            ++finish;                   //调整高度
        } else {
            insert_aux(end(), x);       //数组被填充满, 调用insert_aux必须重新寻找新的更大的连续空间, 再进行插入
        }
    }
    //<memory> -> <stl_construct.h>
    #include <new.h>
    template <class T1, class T2>
    inline void construct(T1 *p, const T2& value) {
        new (p) T1(value);
    }

    //使用空间的尾自减并调用其析构函数但并且有释放内存
    void pop_back() {
        --finish;
        destroy(finish);
    }
    

    template<class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
        if (finish != end_of_storage) { //为什么又重新判断一遍呢？原因在于该函数不仅仅会被这个类里调用 其它地方也会调用这个函数
            construct(finish, *(finish-1));//在备用空间起始处构造一个元素并以vector最后一个元素值为初始值
            ++finish;
            T x_copy = x;
            copy_backward(position, finish - 2, finish - 1);//TODO
            *position = x_copy;
        } else { //没有备用空间
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2* old_size : 1;
            //以上分配原则:如果原大小为0则分配1个元素 因为 0 的任意倍数仍然是0
            //如果原大小不为0则分配原大小的两倍
            //前半段用来放置原数据后半段用来放置新数据
            iterator new_start  = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                //将原来的vector的内容拷贝到新vector
                //为新元素赋值
                //调整水位
                //拷贝安插点后的内容 因为也可能被insert(p,x)调用
                new_finish = uninitialized_copy(start, position, new_start);
                construct(new_finish, x);
                ++new_finish;
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch(const std::exception& e) {
                //"commit or rollback"
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            //构造并释放原 vector
            destroy(begin(),end());
            deallocate();
            //调整迭代器指向新 vector
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
    //从 position 开始 插入 n 个元素 元素初始值为x
    //![image.png](https://i.loli.net/2020/11/10/xKML9Ia2FNmc3gb.png)
    //![image.png](https://i.loli.net/2020/11/10/5iFeDqdrobLJX2p.png)
    //![image.png](https://i.loli.net/2020/11/10/XLOpMIAyjrPvZf8.png)
    template<class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T&x) {
    if (n !=0) { // 当 n 不等于 0 时才进行以下操作
        if (size_type(end_of_storage - finish) >=n) {
            //备用空间大于等于 "新增元素个数"
            T x_copy = x;
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                //插入点之后的现有元素个数 大于 新增元素个数
                uninitialized_copy(finish - n, finish, finish);
                finish += n;//将 vector 尾端标记后移 没问题
                copy_backward(position, old_finish - n, old_finish);
                fill(position, position + n, x_copy);//从插入点开始填入新值;
            } else {
                //插入点之后的现有元素 小于等于 新增元素个数
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                fill(position, old_finish, x_copy);
            }
        } else { 
            //备用空间小于 新增元素个数 -必须配置额外的内存
            //首先决定新长度：旧长度的两倍 或 旧长度+新增元素个数
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            //以下配置新的 vector 空间
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            __STL_TRY {
                //首先将旧的 vector 的插入点之前的元素复制到新空间
                new_finish = uninitialized_copy(start, position, new_start);
                //再将新增元素填入新空间
                new_finish = uninitialized_fill_n(new_finish, n, x);
                //再将旧的vector的插入点之后的元素复制到新空间
                new_finish = uninitialized_copy(position, finish, new_finish);
            } catch(...) {
                destroy();
                //"commit or rollback"
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            //清楚并释放旧的vector
            destroy(start, finish);
            deallocate();
            //调整水位
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    //vector提供一个构造函数 允许我们指定空间大小及初值
    vector<size_type n, const T& value> {   fill_initialize(n, value);  }
    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
    //配置而后填充
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);//配置n个元素空间
        uninitialized_fill_n(result, n, x);
        return result;
    }
};


