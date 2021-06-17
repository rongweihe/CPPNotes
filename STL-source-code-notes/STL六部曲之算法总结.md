STL  算法博大精深，涵盖诸多内容，其算法之大观，细节之深入，每次阅读每次都有不同的收获。

![](https://cdn.jsdelivr.net/gh/rongweihe/ImageHost01/images/STL%E5%85%AD%E9%83%A8%E6%9B%B2%E4%B9%8B%E7%AE%97%E6%B3%95.png)

### 构成

| 头文件         | 功能            |
| :------------- | :-------------- |
| `<algorithm>`  | 算法函数        |
| `<numeric>`    | 数值算法        |
| `<functional>` | 函数对象/仿函数 |

### 分类

| No.  | 分类       | 说明                              |                                |
| :--: | :--------- | :-------------------------------- | ------------------------------ |
|  1   | 非质变算法 | Non-modifying sequence operations | 不直接修改容器内容的算法       |
|  2   | 质变算法   | Modifying sequence operations     | 可以修改容器内容的算法         |
|  3   | 排序算法   | Sorting/Partitions/Binary search/ | 对序列排序、合并、搜索算法操作 |
|  4   | 数值算法   | Merge/Heap/Min/max                | 对容器内容进行数值计算         |

### 填充

| 函数                     | 作用                                                  |
| :----------------------- | :---------------------------------------------------- |
| `fill(beg,end,val)`      | 将值`val`赋给[`beg`,`end`)范围内的所有元素            |
| `fill_n(beg,n,val)`      | 将值`val`赋给[`beg`,`beg+n`)范围内的所有元素          |
| `generate(beg,end,func)` | 连续调用函数`func`填充[`beg`,`end`)范围内的所有元素   |
| `generate_n(beg,n,func)` | 连续调用函数`func`填充[`beg`,`beg+n`)范围内的所有元素 |

- `fill()`/`fill_n()`用于填充相同值，`generate()`/`generate_n()`用于填充不同值。

### 遍历/变换

| 函数                                   | 作用                                                         |
| :------------------------------------- | :----------------------------------------------------------- |
| `for_each(beg,end,func)`               | 将[`beg`,`end`)范围内所有元素依次调用函数`func`，返回`func`。不修改序列中的元素 |
| `transform(beg,end,res,func)`          | 将[`beg`,`end`)范围内所有元素依次调用函数`func`，结果放入`res`中 |
| `transform(beg2,end1,beg2,res,binary)` | 将[`beg`,`end`)范围内所有元素与[`beg2`,`beg2+end-beg`)中所有元素依次调用函数`binnary`，结果放入`res`中 |

### 最大最小

| 函数                       | 作用                                                         |
| :------------------------- | :----------------------------------------------------------- |
| `max(a,b)`                 | 返回两个元素中较大一个                                       |
| `max(a,b,cmp)`             | 使用自定义比较操作`cmp`,返回两个元素中较大一个               |
| `max_element(beg,end)`     | 返回一个`ForwardIterator`，指出[`beg`,`end`)中最大的元素     |
| `max_element(beg,end,cmp)` | 使用自定义比较操作`cmp`,返回一个`ForwardIterator`，指出[`beg`,`end`)中最大的元素 |
| `min(a,b)`                 | 返回两个元素中较小一个                                       |
| `min(a,b,cmp)`             | 使用自定义比较操作`cmp`,返回两个元素中较小一个               |
| `min_element(beg,end)`     | 返回一个`ForwardIterator`，指出[`beg`,`end`)中最小的元素     |
| `min_element(beg,end,cmp)` | 使用自定义比较操作`cmp`,返回一个`ForwardIterator`，指出[`beg`,`end`)中最小的元素 |

###  排序算法(12个)：提供元素排序策略

| 函数                                          | 作用                                                         |
| :-------------------------------------------- | :----------------------------------------------------------- |
| `sort(beg,end)`                               | 默认升序重新排列元素                                         |
| `sort(beg,end,comp)`                          | 使用函数`comp`代替比较操作符执行`sort()`                     |
| `partition(beg,end,pred)`                     | 元素重新排序，使用`pred`函数，把结果为`true`的元素放在结果为`false`的元素之前 |
| `stable_sort(beg,end)`                        | 与`sort()`类似，保留相等元素之间的顺序关系                   |
| `stable_sort(beg,end,pred)`                   | 使用函数`pred`代替比较操作符执行`stable_sort()`              |
| `stable_partition(beg,end)`                   | 与`partition()`类似，保留容器中的相对顺序                    |
| `stable_partition(beg,end,pred)`              | 使用函数`pred`代替比较操作符执行`stable_partition()`         |
| `partial_sort(beg,mid,end)`                   | 部分排序，被排序元素个数放到[beg,end)内                      |
| `partial_sort(beg,mid,end,comp)`              | 使用函数`comp`代替比较操作符执行`partial_sort()`             |
| `partial_sort_copy(beg1,end1,beg2,end2)`      | 与`partial_sort()`类似，只是将[beg1,end1)排序的序列复制到[beg2,end2) |
| `partial_sort_copy(beg1,end1,beg2,end2,comp)` | 使用函数`comp`代替比较操作符执行`partial_sort_copy()`        |
| `nth_element(beg,nth,end)`                    | 单个元素序列重新排序，使所有小于第`n`个元素的元素都出现在它前面，而大于它的都出现在后面 |
| `nth_element(beg,nth,end,comp)`               | 使用函数`comp`代替比较操作符执行`nth_element()`              |

###  反转/旋转

| 函数                           | 作用                                        |
| :----------------------------- | :------------------------------------------ |
| `reverse(beg,end)`             | 元素重新反序排序                            |
| `reverse_copy(beg,end,res)`    | 与`reverse()`类似，结果写入`res`            |
| `rotate(beg,mid,end)`          | 元素移到容器末尾，由`mid`成为容器第一个元素 |
| `rotate_copy(beg,mid,end,res)` | 与`rotate()`类似，结果写入`res`             |

### 随机

| 函数                          | 作用                                                |
| :---------------------------- | :-------------------------------------------------- |
| `random_shuffle(beg,end)`     | 元素随机调整次序                                    |
| `random_shuffle(beg,end,gen)` | 使用函数`gen`代替随机生成函数执行`random_shuffle()` |

### 查找算法(13个)：判断容器中是否包含某个值

### 统计

| 函数                     | 作用                                                         |
| :----------------------- | :----------------------------------------------------------- |
| `count(beg,end,val)`     | 利用`==`操作符，对[`beg`,`end`)的元素与`val`进行比较，返回相等元素个数 |
| `count_if(beg,end,pred)` | 使用函数`pred`代替`==`操作符执行`count()`                    |

### 查找

| 函数                                      | 作用                                                         |
| :---------------------------------------- | :----------------------------------------------------------- |
| `find(beg,end,val)`                       | 利用`==`操作符，对[`beg`,`end`)的元素与`val`进行比较。当匹配时结束搜索，返回该元素的`InputIterator` |
| `find_if(beg,end,pred)`                   | 使用函数`pred`代替`==`操作符执行`find()`                     |
| `find_first_of(beg1,end1,beg2,end2)`      | 在[`beg1`,`end1`)范围内查找[`beg2`,`end2`)中任意一个元素的第一次出现。返回该元素的`Iterator` |
| `find_first_of(beg1,end1,beg2,end2,pred)` | 使用函数`pred`代替`==`操作符执行`find_first_of()`。返回该元素的`Iterator` |
| `find_end(beg1,end1,beg2,end2)`           | 在[`beg1`,`end1`)范围内查找[`beg2`,`end2`)最后一次出现。找到则返回最后一对的第一个`ForwardIterator`，否则返回`end1` |
| `find_end(beg1,end1,beg2,end2,pred)`      | 使用函数`pred`代替`==`操作符执行`find_end()`。返回该元素的`Iterator` |
| `adjacent_find(beg,end)`                  | 对[`beg`,`end`)的元素，查找一对相邻重复元素，找到则返回指向这对元素的第一个元素的`ForwardIterator`。否则返回`end` |
| `adjacent_find(beg,end,pred)`             | 使用函数`pred`代替`==`操作符执行`adjacent_find()`            |

### 搜索

| 函数                               | 作用                                                         |
| :--------------------------------- | :----------------------------------------------------------- |
| `search(beg1,end1,beg2,end2)`      | 在[`beg1`,`end1`)范围内查找[`beg2`,`end2`)首一次出现，返回一个`ForwardIterator`，查找成功,返回[`beg1`,`end1`)内第一次出现[`beg2`,`end2`)的位置，查找失败指向`end1` |
| `search(beg1,end1,beg2,end2,pred)` | 使用函数`pred`代替`==`操作符执行`search()`                   |
| `search_n(beg,end,n,val)`          | 在[`beg`,`end`)范围内查找`val`出现`n`次的子序列              |
| `search_n(beg,end,n,val,pred)`     | 使用函数`pred`代替`==`操作符执行`search_n()`                 |
| `binary_search(beg,end,val)`       | 二分查找，在[`beg`,`end`)中查找`val`，找到返回`true`         |
| `binary_search(beg,end,val,comp)`  | 使用函数`comp`代替比较操作符执行`binary_search()`            |

### 边界

| 函数                            | 作用                                                         |
| :------------------------------ | :----------------------------------------------------------- |
| `lower_bound(beg,end,val)`      | 在[`beg`,`end`)范围内的可以插入`val`而不破坏容器顺序的第一个位置，返回一个`ForwardIterator`（返回范围内第一个大于等于值val的位置） |
| `lower_bound(beg,end,val,comp)` | 使用函数`comp`代替比较操作符执行`lower_bound()`              |
| `upper_bound(beg,end,val)`      | 在[`beg`,`end`)范围内插入`val`而不破坏容器顺序的最后一个位置，该位置标志一个大于`val`的值，返回一个`ForwardIterator`（返回范围内第一个大于val的位置） |
| `upper_bound(beg,end,val,comp)` | 使用函数`comp`代替比较操作符执行`upper_bound()`              |
| `equal_range(beg,end,val)`      | 返回一对`iterator`，第一个表示`lower_bound`，第二个表示`upper_bound` |
| `equal_range(beg,end,val,comp)` | 使用函数`comp`代替比较操作符执行`lower_bound()`              |

## 删除和替换算法(15个)

### 复制

| 函数                         | 作用                                       |
| :--------------------------- | :----------------------------------------- |
| `copy(beg,end,res)`          | 复制[`beg`,`end`)到`res`                   |
| `copy_backward(beg,end,res)` | 与`copy()`相同，不过元素是以相反顺序被拷贝 |

### 移除

| 函数                               | 作用                                                         |
| :--------------------------------- | :----------------------------------------------------------- |
| `remove(beg,end,val)`              | 移除`[first,last)`区间内所有与`val`值相等的元素，并不是真正的从容器中删除这些元素(原容器的内容不会改变)而是将结果复制到一个以`result`为起始位置的容器中。新容器可以与原容器重叠 |
| `remove_if(beg,end,pred)`          | 删除`[`beg`,`end`)`内`pred`结果为`true`的元素                |
| `remove_copy(beg,end,res,val)`     | 将所有不等于`val`元素复制到`res`，返回`OutputIterator`指向被拷贝的末元素的下一个位置 |
| `remove_copy_if(beg,end,res,pred)` | 将所有使`pred`结果为`true`的元素拷贝到`res`                  |

### 替换

| 函数                                     | 作用                                                  |
| :--------------------------------------- | :---------------------------------------------------- |
| `replace(beg,end,oval,nval)`             | 将[`beg`,`end`)内所有等于`oval`的元素都用`nval`代替   |
| `replace_copy(beg,end,res,oval,nval)`    | 与`replace()`类似，不过将结果写入`res`                |
| `replace_if(beg,end,pred,nval)`          | 将[`beg`,`end`)内所有`pred`为`true`的元素用`nval`代替 |
| `replace_copy_if(beg,end,res,pred,nval)` | 与`replace_if()`，不过将结果写入`res`                 |

### 去重

| 函数                            | 作用                                                         |
| :------------------------------ | :----------------------------------------------------------- |
| `unique(beg,end)`               | 清除序列中相邻重复元素，不真正删除元素。重载版本使用自定义比较操作 |
| `unique(beg,end,pred)`          | 将所有使`pred`结果为`true`的相邻重复元素去重                 |
| `unique_copy(beg,end,res)`      | 与`unique`类似，不过把结果输出到`res`                        |
| `unique_copy(beg,end,res,pred)` | 与`unique`类似，不过把结果输出到`res`                        |

### 交换

| 函数                         | 作用                                                         |
| :--------------------------- | :----------------------------------------------------------- |
| `swap(a,b)`                  | 交换存储在`a`与`b`中的值                                     |
| `swap_range(beg1,end1,beg2)` | 将[`beg1`,`end1`)内的元素[`beg2`,`beg2+beg1-end1`)元素值进行交换 |
| `iter_swap(it_a,it_b)`       | 交换两个`ForwardIterator`的值                                |

## 算术算法(4个)`<numeric>`

| 函数                                                | 作用                                                         |
| :-------------------------------------------------- | :----------------------------------------------------------- |
| `accumulate(beg,end,val)`                           | 对[`beg`,`end`)内元素之和，加到初始值`val`上                 |
| `accumulate(beg,end,val,binary)`                    | 将函数`binary`代替加法运算，执行`accumulate()`               |
| `partial_sum(beg,end,res)`                          | 将[`beg`,`end`)内该位置前所有元素之和放进`res`中             |
| `partial_sum(beg,end,res,binary)`                   | 将函数`binary`代替加法运算，执行`partial_sum()`              |
| `adjacent_difference(beg1,end1,res)`                | 将[`beg`,`end`)内每个新值代表当前元素与上一个元素的差放进`res`中 |
| `adjacent_difference(beg1,end1,res,binary)`         | 将函数`binary`代替减法运算，执行`adjacent_difference()`      |
| `inner_product(beg1,end1,beg2,val)`                 | 对两个序列做内积(对应元素相乘，再求和)并将内积加到初始值`val`上 |
| `inner_product(beg1,end1,beg2,val,binary1,binary2)` | 将函数`binary1`代替加法运算,将`binary2`代替乘法运算，执行`inner_product()` |

## 关系算法(4个)`<stl_algobase.h>`

| 函数                                                | 作用                                                         |
| :-------------------------------------------------- | :----------------------------------------------------------- |
| `equal(beg1,end1,beg2)`                             | 判断[`beg1`,`end1`)与[`beg2`,`end2`)内元素都相等             |
| `equal(beg1,end1,beg2,pred)`                        | 使用`pred`函数代替默认的`==`操作符                           |
| `includes(beg1,end1,beg2,end2)`                     | 判断[`beg1`,`end1`)是否包含[`beg2`,`end2`)，使用底层元素的`<`操作符，成功返回true。重载版本使用用户输入的函数 |
| `includes(beg1,end1,beg2,end2,comp)`                | 将函数`comp`代替`<`操作符，执行`includes()`                  |
| `lexicographical_compare(beg1,end1,beg2,end2)`      | 按字典序判断[`beg1`,`end1`)是否小于[`beg2`,`end2`)           |
| `lexicographical_compare(beg1,end1,beg2,end2,comp)` | 将函数`comp`代替`<`操作符，执行`lexicographical_compare()`   |
| `mismatch(beg1,end1,beg2)`                          | 并行比较[`beg1`,`end1`)与[`beg2`,`end2`)，指出第一个不匹配的位置，返回一对`iterator`，标志第一个不匹配元素位置。如果都匹配，返回每个容器的`end` |
| `mismatch(beg1,end1,beg2,pred)`                     | 使用`pred`函数代替默认的`==`操作符                           |

## 集合算法(6个)

| 函数                                                | 作用                                                    |
| :-------------------------------------------------- | :------------------------------------------------------ |
| `merge(beg1,end1,beg2,end2,res)`                    | 合并[`beg1`,`end1`)与[`beg2`,`end2`)存放到`res`         |
| `merge(beg1,end1,beg2,end2,res,comp)`               | 将函数`comp`代替`<`操作符，执行`merge()`                |
| `inplace_merge(beg,mid,end)`                        | 合并[`beg`,`mid`)与[`mid`,`end`)，结果覆盖[`beg`,`end`) |
| `inplace_merge(beg,mid,end,cmp)`                    | 将函数`comp`代替`<`操作符，执行`inplace_merge()`        |
| `set_union(beg1,end1,beg2,end2,res)`                | 取[`beg1`,`end1`)与[`beg2`,`end2`)元素并集存放到`res`   |
| `set_union(beg1,end1,beg2,end2,res,comp)`           | 将函数`comp`代替`<`操作符，执行`set_union()`            |
| `set_intersection(beg1,end1,beg2,end2,res)`         | 取[`beg1`,`end1`)与[`beg2`,`end2`)元素交集存放到`res`   |
| `set_intersection(beg1,end1,beg2,end2,res,comp)`    | 将函数`comp`代替`<`操作符，执行`set_intersection()`     |
| `set_difference(beg1,end1,beg2,end2,res)`           | 取[`beg1`,`end1`)与[`beg2`,`end2`)元素内差集存放到`res` |
| `set_difference(beg1,end1,beg2,end2,res,comp)`      | 将函数`comp`代替`<`操作符，执行`set_difference()`       |
| `set_symmetric_difference(beg1,end1,beg2,end2,res)` | 取[`beg1`,`end1`)与[`beg2`,`end2`)元素外差集存放到`res` |

## 排列组合算法：提供计算给定集合按一定顺序的所有可能排列组合

| 函数                             | 作用                                                |
| :------------------------------- | :-------------------------------------------------- |
| `next_permutation(beg,end)`      | 取出[`beg`,`end`)内的下移一个排列                   |
| `next_permutation(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`next_permutation()` |
| `prev_permutation(beg,end)`      | 取出[`beg`,`end`)内的上移一个排列                   |
| `prev_permutation(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`prev_permutation()` |

###  堆算法(4个)

| 函数                      | 作用                                                         |
| :------------------------ | :----------------------------------------------------------- |
| `make_heap(beg,end)`      | 把[`beg`,`end`)内的元素生成一个堆                            |
| `make_heap(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`make_heap()`                 |
| `pop_heap(beg,end)`       | 重新排序堆。它把first和last-1交换，然后重新生成一个堆。可使用容器的back来访问被"弹出"的元素或者使用pop_back进行真正的删除。并不真正把最大元素从堆中弹出 |
| `pop_heap(beg,end,comp)`  | 将函数`comp`代替`<`操作符，执行`pop_heap()`                  |
| `push_heap(beg,end)`      | 假设first到last-1是一个有效堆，要被加入到堆的元素存放在位置`last-1`，重新生成堆。在指向该函数前，必须先把元素插入容器后 |
| `push_heap(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`push_heap()`                 |
| `sort_heap(beg,end)`      | 对[`beg`,`end`)内的序列重新排序                              |
| `sort_heap(beg,end,comp)` | 将函数`comp`代替`<`操作符，执行`push_heap()`                 |

参考：

《STL源码剖析》-侯捷

https://www.jianshu.com/p/eb554b0943ab


