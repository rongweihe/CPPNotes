
#include <list>
#include <algorithm>
#include <iostream>
using namespace std;


template <class T>
struct __list_node {
	typedef void* void_pointer;
	void_pointer prev;
	void_pointer next;
	T data;
};

template <class T, class Ref, class Ptr>
struct __list_iterator {
	typedef __list_iterator<T, T&, T*> iterator;
	typedef __list_iterator<T, Ref, Ptr> self;

	typedef bidirectional_iterator_tag iterator_category;
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef __list_node<T>* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	link_type node; //迭代器内部的一个普通指针 指向 list 的节点

	//contruuctor 构造函数
	__list_iterator(link_type x) : node(x){}
	__list_iterator() {}
	__list_iterator(const iterator& x) : node(x.node){}

	bool operator==(const self& x) const {	return node == x.node; }
	bool operator!=(const self& x) const {	return node != x.node; }

	//对迭代器取值 取的是节点的数据值
	reference operator*() const {	return (*node).data; }

	//对迭代器的成员存取
	pointer operator->() const {	return &(operator*()); }

	//对迭代器累加1 就是前进一个节点
	self& operator++() {
		node = (link_type)((*node).next);
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}

	//对迭代器递减1 就是后退一个节点
	self& operator--() {
		node = (link_type)((*node).prev);
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}

	//如果让指针 node 指向刻意置于尾端的一个空白节点，node 便能符合 STL 对于 “前闭后开”
	//区间的要求，成为 last 的迭代器
	iterator begin() {	return (link_type) ((*node).next); }
	iterator end() { return node;}
	bool empty() const {	return node->next == node; }
	size_type size() const {
		size_type result = 0;
		distance(begin(), end(), result);
		return result;
	}
	//取头结点的内容
	reference front() {	return *begin(); }
	reference back() {	return *(--end());	}
};


//list 源码

template <class T, class Alloc = alloc>
class list {
	//内存分配
protected: 
	typedef __list_node<T> list_node;
	typedef simple_alloc<list_node, Alloc> list_node_allocator;
	//list_node_allocator(n)表示分配n个节点空间，以下四个函数分别用来配置，释放，构造，销毁一个节点
protected:

	link_type get_node() { return list_node_allocator::allocate(); }//配置一个节点并传回
	void put_node(link_type p) { list_node_allocator::deallocate(p);	}//释放一个节点

	link_type create_node(const T& x) {
		link_type p = get_node();
		construct(&p->data, x);
		return p;
	} //产生一个节点(配置并且构造)带有元素值

	//销毁(释放)一个节点
	void destroy_node(link_type p) {
		destroy(&p->data);
		put_node(p);
	}

	//插入一个节点 作为头结点
	void push_front(cosnt T& x) { insert(begin(), x); }
	//插入一个节点 作为尾节点
	void push_back(const T& x) {  insert(end(), x); }

	//移除头结点
	void pop_front() {
		erase(begin());
	}
	//移除尾结点
	void pop_back() {
		iterator tmp = end();
		erase(--tmp);
	}

	//移除迭代器 position 位置所指节点 很简单 先保留前驱和后继节点 然后在调整指针位置即可
	iterator erase(iterator position) {
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);
		return iterator(next_ndoe);
	}

	//清除所有节点
	template <class T, class Alloc>
	void list<T, Alloc>::clear() {
		link_type cur = (link_type) node->next;//begin()
		while(cur != node) { //遍历每一个节点
			link_type tmp = cur;
			cur = (link_type) cur->next;
			destroy_node(tmp); //销毁并释放每一个节点
		}
		node->next = node;
		node->prev = node;
	}
	//将数值为 value 的所有元素移除
	template <class T, class Alloc>
	void list<T, Alloc>::remove(const T& value) {
		iterator first = begin();
		iterator last = end();
		while(first != last) {
			iterator next = first;
			++next;
			if (*first == value) erase(first);//找到就删除
			first = next;
		}
	}
	//移除数值相同的连续元素
	template<class T, class Alloc>
	void list<T, Alloc>::unique() {
		iterator first = begin();
		iterator last = end();
		if (first == last) return; //空链表什么都不做
		iterator next = first;
		while(++next != last) {
			if (*first == *next) erase(next);//找到相同的元素移除之
			else first = next;
			next = first;//修正区间范围
		}
	}
}

void transfer(iterator position, iterator first, iterator last) {
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

template <class T, class Alloc = alloc>
class list {
    //...
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


template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2)
    if (*first2 < *first1) {
      iterator next = first2;
      // 将first2到first+1的左闭右开区间插入到first1的前面
      // 这就是将first2合并到first1链表中
      transfer(first1, first2, ++next);
      first2 = next;
    }
    else
      ++first1;
      // 如果链表x还有元素则全部插入到first1链表的尾部
  if (first2 != last2) transfer(last1, first2, last2);
}


template <class T, class Alloc>
void list<T, Alloc>::reverse() 
{
  if (node->next == node || link_type(node->next)->next == node) 
  	return;
  iterator first = begin();
  ++first;
  while (first != end()) {
    iterator old = first;
    ++first;
      // 将元素插入到begin()之前
    transfer(begin(), old, first);
  }
}