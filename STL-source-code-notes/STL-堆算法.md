## STL 堆的主要相关算法介绍

### push_heap算法

此操作是向堆中添加一个节点。为了满足完全二叉树的条件，新加入的元素一定放在最下面的一层作为叶节点，并填补在由左至右的第一个空格，在这里放在底层容器vector的end()处。

下面是一个演示：
1.将插入的新元素50放在vector的末尾
2.因为其父节点24比自己小，所以将父节点与自己交换位置
3.交换之后，其父节点变为31，其值比31大，所以继续交换位置
4.接着其父节点变为68，其值比68小，所以结束push_heap算法

![img](https://img-blog.csdnimg.cn/20191220143728954.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNDUzMjg1,size_16,color_FFFFFF,t_70)


上面这种比较方式我们称之为：percolate up（上溯）程序 

很显然，新元素的加入很可能使得堆不在满足大顶堆的性质---每个节点的键值都大于或等于其子节点的键值。为了调整使得其重新满足大顶堆的特点，在这里执行一个上溯（percolate up）操作：将新节点与父节点比较，如果其键值比父节点大，就交换父子的位置，如此一直上溯，直到不需要交换或者到根节点为止。 

```c++
template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first,RandomAccessIterator last) 
{
    // 注意，此函式被调用时，新元素已置于底部容器的最尾端
    __push_heap_aux(first, last, distance_type(first),value_type(first));
}
template <class RandomAccessIterator, class Distance, class T>
inline void push_heap_aux(RandomAccessIterator first,RandomAccessIterator last, Distance*, T*) 
{
    __push_heap(first, Distance((last - first) - 1), Distance(0),T(*(last - 1)));
    // 以上系根据implicit representation heap的结构特性：新值必置于底部
    // 容器的最尾端，此即第一个洞号：(last-first)–1
}
// 以下这组 push_back()不允许指定“大小比较标准”
template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator first, Distance holeIndex,Distance topIndex, T value) 
{
    Distance parent = (holeIndex - 1) / 2; // 找出父节点
    
    while (holeIndex > topIndex && *(first + parent) < value) {
        // 当尚未到达顶端，且父节点小于新值（于是不符合 heap 的次序特性）
        // 由于以㆖使用 operator<，可知 STL heap 是一种 max-heap（大者为父）
        *(first + holeIndex) = *(first + parent); //令洞值为父值
        holeIndex = parent; // percolate up：调整洞号，向上提升至父节点
        parent = (holeIndex - 1) / 2; // 新洞的父节点
    } // 持续至顶端，或满足heap的次序特性为止
    
    *(first + holeIndex) = value; //令洞值为新值，完成插入动作
}
```

### pop_heap算法

此操作取走根节点。对于大顶堆，取得的是堆中值最大的节点，对于小顶堆，取得的是堆中值最小的节点。STL实现并不是将这个节点直接删除，而是将其放在底层容器vector的尾端。而原尾端的节点插入到前面的适当位置。

我们首先保存原vector尾端的节点值，然后将根节点值存储在此处。为了实将原尾端节点的值插入适当位置，重新构建大顶堆，我们实施如下调整堆的操作：

先执行下溯（percolate down）操作：从根节点开始将空洞节点（一开始是根节点）和较大子节点交换，并持续向下进行，直到到达叶节点为止。然后将已保存的原容器vector尾端节点赋给这个已到达叶层的空洞节点。

注意，到这里并没有结束。因为这时候可能还没有满足大顶堆的特性。还需要执行一次上溯操作。这样，便重新构建了大顶堆。

下面是一个演示：
1.取出根节点元素68，取出最后一个节点的元素24
2.将原根节点元素68放在vector的最后一个位置
3.将原先最后一个节点的元素24放置到原根节点68处作为新根节点
4.接着其值24比左、右节点50、65都小，但是65比50大，于是就将其值24与值68的子节点互换
5.接着其值24又比左、右节点32、26都小，但是32比26大，于是就将其值24与值32的子节点互换
6.最后其没有子节点可以比较了，结束算法

![img](https://img-blog.csdnimg.cn/20191220144911344.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNDUzMjg1,size_16,color_FFFFFF,t_70)

```c++
template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last) 
{
    __pop_heap_aux(first, last, value_type(first));
}
template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last, T*) 
{
    __pop_heap(first, last-1, last-1, T(*(last-1)),
    distance_type(first));
    // 以上，根据 implicit representation heap 的次序特性，pop操作的结果
    // 应为底部容器的第一个元素。因此，首先设定欲调整值为尾值，然后将首值调至
    // 尾节点（所以以上将迭代器 result设为 last-1）。然后重整 [first, last-1)，
    // 使之重新成一个合格的 heap
}
// 以下这组 pop_heap() 不允许指定“大小比较标准”
template <class RandomAccessIterator, class T, class Distance>
inline void __pop_heap(RandomAccessIterator first,
        RandomAccessIterator last,
        RandomAccessIterator result,
        T value, Distance*) 
{
    *result = *first; // 设定尾值为首值，于是尾值即为欲求结果，
                      // 可由客端稍后再以底层容器之 pop_back() 取出尾值
 
    __adjust_heap(first, Distance(0), Distance(last - first), value);
    // 以上欲重新调整 heap，洞号为 0（亦即树根处），欲调整值为 value（原尾值）
}
// 以下这个 __adjust_heap() 不允许指定“大小比较标准”
template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,Distance len, T value) 
{
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2; // 洞节点之右子节点
 
    while (secondChild < len) {
        // 比较洞节点之左右两个子值，然后以 secondChild 代表较大子节点
        if (*(first + secondChild) < *(first + (secondChild - 1)))
            secondChild--;
        // Percolate down：令较大子值为洞值，再令洞号下移至较大子节点处
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        // 找出新洞节点的右子节点
        secondChild = 2 * (secondChild + 1);
    }
 
    if (secondChild == len) { // 没有右子节点，只有左子节点
        // Percolate down：令左子值为洞值，再令洞号下移至左子节点处
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
 
    // 将欲调整值填入目前的洞号内。注意，此时肯定满足次序特性
    push_heap(first, holeIndex, topIndex, value);
}
```



### sort_heap算法

堆排序算法。执行此操作之后，容器vector中的元素按照从小到大的顺序排列。

构建大顶堆之后，不断执行pop_heap算法取出堆顶的元素，即可。因为每次取得的是最大的元素，将其放在容器vector的最尾端。所以到最后vector中的元素是从小到大排列的。

![img](https://img-blog.csdnimg.cn/20191220173517553.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNDUzMjg1,size_16,color_FFFFFF,t_70)

```c++
// 以下这个sort_heap() 不允许指定“大小比较标准”
template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first,RandomAccessIterator last) 
{
    // 以下，每执行一次 pop_heap()，极值（在STL heap中为极大值）即被放在尾端
    // 扣除尾端再执行一次 pop_heap()，次极值又被放在新尾端。一直下去，最后即得排序结果
    while (last - first > 1)
        pop_heap(first, last--); // 每执行 pop_heap() 一次，操作范围即退缩一格
}
```



### make_heap算法

此操作是依据已有的各元素构建堆。

其中，各元素已经存放在底层容器vector中。

构建堆实质是一个不断调整堆（即前面pop_heap算法中的调整堆的操作）的过程---通过不断调整子树，使得子树满足堆的特性来使得整个树满足堆的性质。

叶节点显然需要调整，第一个需要执行调整操作的子树的根节点是从后往前的第一个非叶结点。从此节点往前到根节点对每个子树执行调整操作，即可构建堆。

```c++
// 将 [first,last) 排列为一个heap
template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first,RandomAccessIterator last) 
{
    __make_heap(first, last, value_type(first), distance_type(first));
}
// 以下这组 make_heap() 不允许指定“大小比较标准”
template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first,RandomAccessIterator last, T*,Distance*) 
{
    if (last - first < 2) 
        return; //如果长度为0或1，不必重新排列
 
    Distance len = last - first;
    // 找出第一个需要重排的子树头部，以parent标示出。由于任何叶节点都不需执行
    // perlocate down，所以有以下计算。parent命名不佳，名为holeIndex 更好
    Distance parent = (len - 2)/2;
    
    while (true) {
        // 重排以 parent 为首的子树。len是为了让__adjust_heap() 判断操作范围
        __adjust_heap(first, parent, len, T(*(first + parent)));
        if (parent == 0) 
            return; // 走完根节点，就结束。
        parent--; // （即将重排之子树的）头部向前一个节点
    }
}
```

