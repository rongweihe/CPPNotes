

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
	iterator erase(iterator first, iterator last);
  void clear();   
// 参数是一个迭代器修改该元素的前后指针指向再单独释放节点就行了
iterator erase(iterator position) {
    link_type next_node = link_type(position.node->next);
    link_type prev_node = link_type(position.node->prev);
    prev_node->next = next_node;
    next_node->prev = prev_node;
    destroy_node(position.node);
    return iterator(next_node);
}
    ...
};
```



### transfer函数

**`transfer`函数功能是将一段链表插入到我们指定的位置之前**. 该函数一定要理解, 后面分析的所有函数都是该基础上进行修改的.

`transfer`函数接受3个迭代器. 第一个迭代器表示链表要插入的位置, `first`到`last`最闭右开区间插入到`position`之前.

从`if`下面开始分析(*这里我将源码的执行的先后顺序进行的部分调整, 下面我分析的都是调整顺序过后的代码. 当然我也会把源码顺序写下来, 以便参考*)

- **为了避免待会解释起来太绕口, 这里先统一一下部分名字**

1. `last`的前一个节点叫`last_but_one`
2. `first`的前一个节点叫`zero`

- 好, 现在我们开始分析`transfer`的每一步(*最好在分析的时候在纸上画出两个链表一步步来画*)

1. 第一行. `last_but_one`的`next`指向插入的`position`节点
2. 第二行. `position`的`next`指向`last_but_one`
3. 第三行. 临时变量`tmp`保存`position`的前一个节点
4. 第四行. `first`的`prev`指向`tmp`
5. 第五行. `position`的前一个节点的`next`指向`first`节点
6. 第六行. `zero`的`next`指向`last`节点
7. 第七行. `last`的`prev`指向`zero`

```c++
template <class T, class Alloc = alloc>
class list {
    ...
protected:
    void transfer(iterator position, iterator first, iterator last)  {
      if (position != last) {
        (*(link_type((*last.node).prev))).next = position.node;
        (*(link_type((*first.node).prev))).next = last.node;
        (*(link_type((*position.node).prev))).next = first.node;  
        link_type tmp = link_type((*position.node).prev);
        (*position.node).prev = (*last.node).prev;
        (*last.node).prev = (*first.node).prev; 
        (*first.node).prev = tmp;
      }
    }
    ...
};
```

**splice** 将两个链表进行合并.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
public:
    void splice(iterator position, list& x) {
      if (!x.empty()) 
        transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&, iterator i) {
      iterator j = i;
      ++j;
      if (position == i || position == j) return;
      transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first, iterator last) {
      if (first != last) 
        transfer(position, first, last);
    }
    ...
};
```

### 