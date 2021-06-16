# hashtable 

C++ 11已将哈希表纳入了标准之列。hashtable是hash_set、hash_map、hash_multiset、hash_multimap的底层机制，即这四种容器中都包含一个hashtable。

图-5-24

解决碰撞问题的办法有许多，线性探测、二次探测、开链等等。SGI STL的hashtable采用的开链方法，每个hash table中的元素用vector承载，每个元素称为桶（bucket），一个桶指向一个存储了实际元素的链表（list），链表节点（node）结构如下：

```c++
template <class Value>
struct __hashtable_node
{
  __hashtable_node* next;
  Value val;     // 存储实际值
}; 
```

再来看看hash table的迭代器定义：

```c++
template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {         // 迭代器
  typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
          hashtable;
  ....
 
  typedef __hashtable_node<Value> node;
 
  // 定义迭代器相应类型
  typedef forward_iterator_tag iterator_category;     // 前向迭代器
  typedef Value value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef Value& reference;
  typedef Value* pointer;
 
  node* cur;      // 迭代器目前所指节点
  hashtable* ht;  // 和hashtable之间的纽带
 
  __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
  __hashtable_iterator() {}
  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(operator*()); }
  iterator& operator++();
  iterator operator++(int);
  bool operator==(const iterator& it) const { return cur == it.cur; }
  bool operator!=(const iterator& it) const { return cur != it.cur; }
};
```

hash table的迭代器不能后退，这里关注迭代器的自增操作，代码如下：

```c++
template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() // 注意类模板成员函数的定义
{
  const node* old = cur;
  cur = cur->next;  // 移动到下一个node
  if (!cur) {       // 到了list结尾
    size_type bucket = ht->bkt_num(old->val);     // 根据节点值定位旧节点所在桶号
    while (!cur && ++bucket < ht->buckets.size()) // 计算下一个可用桶号
      cur = ht->buckets[bucket];    // 找到，另cur指向新桶的第一个node
  }
  return *this;
}
```

hashtable数据结构内容很多，这里只列出少量代码：

```c++
template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey,
          class Alloc>
class hashtable {   // hash table数据结构
public:
  typedef Key key_type;
  typedef Value value_type;
  typedef HashFcn hasher;          // 散列函数类型
  typedef EqualKey key_equal;
 
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;
  ....
 
private:
  hasher hash;          // 散列函数
  key_equal equals;     // 判断键值是否相等
  ExtractKey get_key;   // 从节点取出键值
 
  typedef __hashtable_node<Value> node;
  typedef simple_alloc<node, Alloc> node_allocator; // 空间配置器
 
  vector<node*,Alloc> buckets;  // 桶的集合，可以看出一个桶实值上是一个node*
  size_type num_elements;      // node个数
  ....
}
```

SGI STL将hash table的大小，也就是vector的大小设计为28个质数，并存放在一个数组中：

```c++
static const int __stl_num_primes = 28; // 28个质数
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53,         97,         193,       389,       769,
  1543,       3079,       6151,      12289,     24593,
  49157,      98317,      196613,    393241,    786433,
  1572869,    3145739,    6291469,   12582917,  25165843,
  50331653,   100663319,  201326611, 402653189, 805306457, 
  1610612741, 3221225473, 4294967291
};
```

当vector容量不足时，会以两倍的容量进行扩充。

下面介绍插入操作，以insert_unique为例：

```c++
// 插入新元素，键值不能重复
  pair<iterator, bool> insert_unique(const value_type& obj)
  {
    resize(num_elements + 1);           // 判断vector是否需要扩充
    return insert_unique_noresize(obj); // 直接插入obj
  }
```

insert操作大致分两步：第一步是扩充（如果需要的话），第二步是插入。

resize代码如下：

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint)  // 判断是否需要扩充vector
{
  const size_type old_n = buckets.size();
  if (num_elements_hint > old_n)
  {  // 元素个数大于vector容量，则需要扩充vector
    const size_type n = next_size(num_elements_hint);
    if (n > old_n)
    {
      vector<node*, A> tmp(n, (node*) 0); // 建立一个临时的vector作为转移目的地
      for (size_type bucket = 0; bucket < old_n; ++bucket)
      {  // 一个桶一个桶进行转移
        node* first = buckets[bucket];
        while (first)
        {   // 一个节点一个节点进行转移
            size_type new_bucket = bkt_num(first->val, n);  // 散列过程，对n取模
            buckets[bucket] = first->next;
            first->next = tmp[new_bucket];  // 这一句和下一句表示从链表前端插入
            tmp[new_bucket] = first;
            first = buckets[bucket];        // first指向旧vector的下一个node
        }
        buckets.swap(tmp);  // 两个vector的内容互换，使buckets彻底改变
      }
    }
  }
}
```

上述代码基本思路就是：先扩充，再移动，最后交换。

- 扩充利用next_size函数。next_size的作用就是从质数表中选取最接近并且不小于num_elements_hint的质数并返回，利用这个较大值开辟一个新vector。
- 移动实质上就是指针的移动。重新对每个节点进行散列，然后从前链入到新的vector中。
- 交换过程就是上面代码红色部分。这里使用了vector内部的swap成员函数，将*this和tmp的内容进行了互换，这是copy-and-swap技术，《Effective C++》条款11有说明这个技术。扩充完vector后，就可以顺利插入需要插入的元素了。

### 图-5-25

insert_unique_noresize代码如下：

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>                 // 注意，返回一个pair
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj) // 直接插入节点，无需扩充
{
  const size_type n = bkt_num(obj); // 对obj进行散列，然后模上vector大小，从而确定桶号
  node* first = buckets[n];         // first指向对应桶的第一个node
 
  for (node* cur = first; cur; cur = cur->next) 
    if (equals(get_key(cur->val), get_key(obj)))  // 遇到相同node，则直接返回这个node
      return pair<iterator, bool>(iterator(cur, this), false);
 
  // 没有遇到相同node，则在list开头插入
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return pair<iterator, bool>(iterator(tmp, this), true);
}
```

这里也是将新节点插入list的开头，详细过程已在注释中说明。

### 判断元素落脚点（bkt_num）

插入元素之后需要知道某个元素落脚于哪一个 bucket 内。

这本来是哈希函数的责任，但是SGI把这个任务包装了一层，先交给bkt_num()函数，再由此函数调用哈希函数，取得一个可以执行modulus（取模）运算的数值。

**为什么要这么做？**因为有些函数类型无法直接拿来对哈表表的大小进行模运算，例如字符串，这时候我们需要做一些转换，下面是bkt_num函数：

```c++
//版本1：接受实值（value）和buckets个数
size_type _M_bkt_num(const value_type& __obj, size_t __n) const
{
    return _M_bkt_num_key(_M_get_key(__obj), __n); //调用版本4
}
 
//版本2：只接受实值（value）
size_type _M_bkt_num(const value_type& __obj) const
{
    return _M_bkt_num_key(_M_get_key(__obj)); //调用版本3
}
 
//版本3，只接受键值
size_type _M_bkt_num_key(const key_type& __key) const
{
    return _M_bkt_num_key(__key, _M_buckets.size()); //调用版本4
}
 
//版本4：接受键值和buckets个数
size_type _M_bkt_num_key(const key_type& __key, size_t __n) const
{
    return _M_hash(__key) % __n; //SGI的所有内建的hash()，在后面的hash functions中介绍
}
```

### 复制（copy_from）和整体删除（clear）

- 由于整个哈希表由vector和链表组成，因此，复制和整体删除，都需要特别注意内存的释放问题

### hash functions

在<stl_hash_fun.h>中定义了一些hash functions，全都是仿函数
hash functions是计算元素位置的函数
SGI将这项任务赋予了先前提到过的bkt_num()函数，再由bkt_num()函数调用这些hash function，取得一个可以对hashtable进行模运算的值
针对char、int、long等整数类型，这里大部分的hash function什么都没有做，只是直接返回原值。但是对于字符串类型，就设计了一个转换函数如下：

```c++
template <class _Key> struct hash { };
 
inline size_t __stl_hash_string(const char* __s)
{
  unsigned long __h = 0; 
  for ( ; *__s; ++__s)
    __h = 5*__h + *__s;
  
  return size_t(__h);
}
 
//以下所有的__STL_TEMPLATE_NULL在<stl_config.h>中被定义为template<>
__STL_TEMPLATE_NULL struct hash<char*>
{
  size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
};
 
__STL_TEMPLATE_NULL struct hash<const char*>
{
  size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
};
 
__STL_TEMPLATE_NULL struct hash<char> {
  size_t operator()(char __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<unsigned char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<signed char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<short> {
  size_t operator()(short __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<unsigned short> {
  size_t operator()(unsigned short __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<int> {
  size_t operator()(int __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<unsigned int> {
  size_t operator()(unsigned int __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<long> {
  size_t operator()(long __x) const { return __x; }
};
__STL_TEMPLATE_NULL struct hash<unsigned long> {
  size_t operator()(unsigned long __x) const { return __x; }
};
```





### hash_set

 由于hash_set底层是以hash table实现的，因此hash_set只是简单的调用hash table的方法即可
与set的异同点：
hash_set与set都是用来快速查找元素的
但是set会对元素自动排序，而hash_set没有
hash_set和set的使用方法相同

### hash_map

 由于hash_map底层是以hash table实现的，因此hash_map只是简单的调用hash table的方法即可
与map的异同点：
hash_map与map都是用来快速查找元素的
但是map会对元素自动排序，而hash_map没有
hash_map和map的使用方法相同
在介绍hash table的hash functions的时候说过，hash table有一些无法处理的类型（除非用户自己书写hash function）。因此hash_map也无法自己处理



### 三、hash_multiset

hash_multiset与hash_set使用起来相同，只是hash_multiset中允许键值重复

在源码中，hash_multiset调用的是insert_equal()，而hash_set调用的是insert_unique()

### 四、hash_multimap

hash_multimap与hash_map使用起来相同，只是hash_multimap中允许键值重复

在源码中，hash_multimap调用的是insert_equal()，而hash_map调用的是insert_unique()

**这几个函数已经过时了，**被分别替换成了unordered_set、unordered_multiset、unordered_map、unordered_multimap。

STL内部预先定义好的无序容器有：
unordered_set：是无序元素的集合，其中每个元素不能重复
unordered_multiset：和unordered set的唯一差别是，其元素可以重复
unordered_map：元素都是key/value pair，每个key不能重复，value可以重复
unordered_multimap：和unordered_map的唯一差别是，其key可以重复

参考：

《STL源码剖析》 P253.