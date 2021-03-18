

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
	iterator erase(iterator first, iterator last);
    void clear();   
    // 参数是一个迭代器
    // 修改该元素的前后指针指向再单独释放节点就行了
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

