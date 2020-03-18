#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct Node
	{
		Node* lc;
		Node* rc;
		int dist;
		T val;
		Node(const T &vall)
		{
			lc = rc = nullptr;
			dist = 0;
			val = vall;
		}
	}; 
	Node *copy_node(const Node *other_node)
	{
		Node *new_node = new Node(other_node->val);
		if(other_node->lc != nullptr)
			new_node->lc = copy_node(other_node->lc);
		if(other_node->rc != nullptr)
			new_node->rc = copy_node(other_node->rc);
		return new_node;
	}
	void delete_node(Node *to_delete_node)
	{
		if(to_delete_node == nullptr)
			return;
		if(to_delete_node->lc != nullptr)
			delete_node(to_delete_node->lc);
		if(to_delete_node->rc != nullptr)
			delete_node(to_delete_node->rc);
		delete to_delete_node;
	}
	Node *merge(Node *x, Node *y)
	{
		if(x == nullptr)
			return y;
		if(y == nullptr)
			return x;
		if(Compare()(x->val, y->val))
		{
			Node *tmp = x;
			x = y;
			y = tmp;
		}
		x->rc = merge(x->rc, y);
		if(x->lc == nullptr || (x->rc != nullptr && x->lc->dist < x->rc->dist))
		{
			Node *tmp = x->lc;
			x->lc = x->rc;
			x->rc = tmp;
		}
		if(x->rc == nullptr)
			x->dist = 0;
		else
			x->dist = x->rc->dist + 1;
		return x;
	}
	Node *head = nullptr;
public:
	size_t sizee = 0;
	priority_queue() {}
	priority_queue(const priority_queue &other)
	{
		head = copy_node(other.head);
		sizee = other.sizee;
	}
	~priority_queue()
	{
		delete_node(head);
	}
	priority_queue &operator=(const priority_queue &other)
	{
		delete_node(head);
		head = copy_node(other.head);
		sizee = other.sizee;
		return *this;
	}
	const T & top() const
	{
		if(head == nullptr)
			throw container_is_empty();
		return head->val;
	}
	void push(const T &e)
	{
		sizee++;
		Node *new_node = new Node(e);
		head = merge(head, new_node);
	}
	void pop()
	{
		if(head == nullptr)
			throw container_is_empty();
		else
		{
			Node *old_head = head;
			head = merge(head->lc, head->rc);
			delete old_head;
			sizee--;
		}
	}
	size_t size() const
	{
		return sizee;
	}
	bool empty() const 
	{
		return head == nullptr;
	}
	void merge(priority_queue &other)
	{
		sizee += other.sizee;
		head = merge(head, other.head);
		other.head = nullptr;
	}
};

}

#endif
