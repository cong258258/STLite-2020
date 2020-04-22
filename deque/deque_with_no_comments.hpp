#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 
int blocksize = 3000;
int allowblocksize = 1500;
int start_from = 500;//就从500开始存
int allow_min_start_from = 100;
int allow_max_start_from = 1200;
template<class T>
class deque {
private:
	int totalsize = 0;
	struct datatype
	{
        T* data;
        datatype():data(nullptr){}
        datatype(datatype &x):data(x.data){}
        datatype(const T &x)
        {
        	data = new T(x);
        }
        ~datatype()
        {
        	if(data != nullptr)
        		delete data;
        }
        datatype& operator=(datatype &&x)
		{
			if(this == &x)
				return *this;
			data = x.data;
			x.data = nullptr;
			return *this;
		}
    };
	struct Block
	{
		datatype* datas;
    	int start;
    	int end;
    	Block *prv;
    	Block *next;
    	Block():prv(nullptr), next(nullptr), start(start_from), end(start_from-1)
    	{
    		datas = new datatype[blocksize];
    	}
    	Block(const datatype* b, int sizee):prv(nullptr), next(nullptr), start(start_from), end(start_from+sizee-1)
    	{
    		datas = new datatype[blocksize];
    		for(int i = 0; i < sizee; i++)
    			datas[start+i].data = b[i].data;
    	}
    	Block(Block &b):prv(nullptr), next(nullptr), start(b.start), end(b.end)
    	{
    		datas = new datatype[blocksize];
    		for(int i = start; i <= end; i++)
    			datas[i].data = new T(*b.datas[i].data);
    	}
    	Block(Block *b):prv(nullptr), next(nullptr), start(b->start), end(b->end)
    	{
    		datas = new datatype[blocksize];
    		for(int i = start; i <= end; i++)
    			datas[i].data = new T(*b->datas[i].data);
    	}
    	~Block()
    	{
    		delete[] datas;
    	}
    	int size()
    	{
    		return end-start+1;
    	}
    	const int size() const
    	{
    		return end-start+1;
    	}
    	void delete_empty_block()
    	{
            if(size())
            	return;
            if(prv != nullptr && prv == next)
            	return;
            if(prv != nullptr)
            	prv->next = next;
            if(next != nullptr)
            	next->prv = prv;
            delete this;
        }
        void zhenli()
    	{    	
    		int oldsize = size();
            if(start > allow_max_start_from)
            {	
            	for(int i = 0; i < size(); i++)
            	{
            		datas[start_from+i].data = datas[start+i].data;
            		datas[start+i].data = nullptr;
            	}
            	start = start_from;
            	end = start + oldsize - 1;
            }
            else if(start < allow_min_start_from)
            {
            	for(int i = size()-1; i >= 0; i--)
            	{
            		datas[start_from+i].data = datas[start+i].data;
            		datas[start+i].data = nullptr;
            	}
            	start = start_from;
            	end = start + oldsize - 1;
            }
    	}
        void split()
        {
        	if(size() >= allowblocksize)
        	{
	            int size1 = size()/2;
	            int size2 = size()-size1;
	            Block *new1 = new Block();
	            new1->start = start_from;
	            new1->end = start_from + size1 - 1;
	            for(int i = 0; i < size1; i++)
    				new1->datas[new1->start+i].data = new T(*datas[start+i].data);
	            Block *new2 = new Block();
	            new2->start = start_from;
	            new2->end = start_from + size2 - 1;
	            for(int i = 0; i < size2; i++)
    				new2->datas[new2->start+i].data = new T(*datas[start+size1+i].data);
	            new1->prv = prv;
	            new1->next = new2;
	            new2->prv = new1;
	            new2->next = next;
	            prv->next = new1;
	            next->prv = new2;
	            // for(int i = start; i <= end; i++)
	            //   	datas[i] = nullptr;
	            // printf("Caution: before delete\n");
	            delete this;
	            // printf("Caution: after delete\n");
	            // Block *new1 = new Block(datas+start, size1);
	            // Block *new2 = new Block(datas+start+size1, size2);
	            // new1->prv = prv;
	            // new1->next = new2;
	            // new2->prv = new1;
	            // new2->next = next;
	            // prv->next = new1;
	            // next->prv = new2;
	            // for(int i = start; i <= end; i++)
	            //     datas[i].data = nullptr;
	            // delete this;
    		}
        }
    	void push_front_block(const T &x)
    	{
    		start--;
    		datas[start].data = new T(x);
    		zhenli();
    		split();
    	}
        void push_back_block(const T &x)
        {
        	end++;
        	datas[end].data = new T(x);
        	zhenli();
        	split();
        }
        void pop_front_block()
        {
        	delete datas[start].data;
        	datas[start].data = nullptr;
        	start++;
        	delete_empty_block();
        }
        void pop_back_block()
        {
        	delete datas[end].data;
        	datas[end].data = nullptr;
        	end--;
        	delete_empty_block();
        }
        friend void it_move_right(Block* &itblock, datatype* &itdata, int n)
        {
            int cnt = itdata - &(itblock->datas[itblock->start]);
            int alltomove = cnt + n;
            while(alltomove >= itblock->size())
            {
                if(!itblock->next->size())
                {
                	itdata = &itblock->datas[itblock->start];
                	itdata += alltomove;
                	return;
                }
                alltomove -= itblock->size();
                itblock = itblock->next;
            }
            itdata = &itblock->datas[itblock->start+alltomove];
        }
        friend void it_move_left(Block* &itblock, datatype* &itdata, int n)
        {
            int cnt = &(itblock->datas[itblock->end]) - itdata;
        //    printf("iterator in block pos back %d\n", cnt);
//            printf("%d\n", itdata->data); 
            // if(cnt - n <= 0)
            // {
            //     if(!itblock->prv->size())
            //     {
            //     	itdata -= n;
            //     	return;
            //     }
            //     n -= itblock->size() - cnt;
            //     itblock = itblock->prv;
            //     while(n > itblock->size())
            //     {
            //     	n -= itblock->size();
            //     	itblock = itblock->prv;
            //     }
            //     itdata = &itblock->datas[itblock->end-n];
            // }
            // else
            // 	itdata -= n;
            int alltomove = n + cnt;
            while(alltomove >= itblock->size())
            {
                if(!itblock->prv->size())
                {
                	itdata = &itblock->datas[itblock->end];
                	itdata -= alltomove;
                	return;
                }
                alltomove -= itblock->size();
                itblock = itblock->prv;
            }
            itdata = &itblock->datas[itblock->end-alltomove];
        }
        friend void it_move_right(const Block* &itblock, const datatype* &itdata, int n)
        {
            int cnt = itdata - &(itblock->datas[itblock->start]);
            int alltomove = cnt + n;
            while(alltomove >= itblock->size())
            {
                if(!itblock->next->size())
                {
                	itdata = &itblock->datas[itblock->start];
                	itdata += alltomove;
                	return;
                }
                alltomove -= itblock->size();
                itblock = itblock->next;
            }
            itdata = &itblock->datas[itblock->start+alltomove];
        }
        friend void it_move_left(const Block* &itblock, const datatype* &itdata, int n)
        {
            int cnt = &(itblock->datas[itblock->end]) - itdata;
            int alltomove = n + cnt;
            while(alltomove >= itblock->size())
            {
                if(!itblock->prv->size())
                {
                	itdata = &itblock->datas[itblock->end];
                	itdata -= alltomove;
                	return;
                }
                alltomove -= itblock->size();
                itblock = itblock->prv;
            }
            itdata = &itblock->datas[itblock->end-alltomove];
        }
        void insert_at_id_block(int id, const T &value)
        {
            id += start;
            end++;
            for(int i = end; i > id; i--)
            	datas[i].data = datas[i-1].data;
            datas[id].data = new T(value);
            split();
        }
        void remove_at_id_block(int id)
        {
            id += start;
            end--;
            for(int i = id; i <= end; i++)
            {
            	delete datas[i].data;
            	datas[i].data = new T(*datas[i+1].data);
            }
            delete datas[end+1].data;
            datas[end+1].data = nullptr;
            delete_empty_block(); 
        }
	};
	Block *root;
	void copy(const Block *otherroot)
    {
    	if(otherroot->next == nullptr)
    		return;
       	root->next = new Block(otherroot->next);
       	root->next->prv = root;
       	Block *cur = root->next;
       	Block *cur2 = otherroot->next;
	    while(cur2->next != otherroot)
	    {
	        cur->next = new Block(cur2->next);
	        cur->next->prv = cur;
	        cur = cur->next;
	        cur2 = cur2->next;
	    }
	    root->prv = cur;
	    root->prv->next = root;
    }
    void deleteall()
    {
    	Block *p = root->next;
    	while(p != nullptr && p != root)
        {
        	Block *tmp = p;
        	p = p->next;
        	delete tmp;
	    }
	    root->next = root->prv = nullptr;
    }
    T& findpos(int n)
    {
    	Block *p = root->next;
       	while(n >= p->size())
       	{
       		n -= p->size();
       		p = p->next;
       	}
       	return *p->datas[p->start+n].data;
    }
    const T& findpos(int n) const
    {
    	Block *p = root->next;
       	while(n >= p->size())
       	{
       		n -= p->size();
       		p = p->next;
       	}
       	return *p->datas[p->start+n].data;
    }
    void allocateprp()
    {
    	if(root == nullptr)
    		root = new Block();
    	if(root->next == nullptr)
    	{
    		root->prv = root->next = new Block();
    		root->prv->next = root;
    		root->next->prv = root;
    	}
    }
    void insert_at_id(int id, const T &value)
    {
        allocateprp();
        Block *p = root->next;
        while(id > p->size())
        {
        	id -= p->size();
        	 p = p->next;
        }
        p->insert_at_id_block(id, value);
    }
    void remove_at_id(int id)
    {
    	Block *p = root->next;
        while(id + 1> p->size())
        {
        	id -= p->size();
        	 p = p->next;
        }
        p->remove_at_id_block(id);
	}
public:
	class const_iterator;
	class iterator
	{
	public:
		Block *itblock;
		datatype *itdata;
	public:
		deque *dq;
		int id;
		iterator() = default;
		iterator(deque* dd, Block* bb, datatype* tt, int iidd): dq(dd), itblock(bb), itdata(tt), id(iidd) {}
		iterator operator+(const int &n) const
		{
			iterator ret = *this;
			ret.id += n;
			if(n > 0)
				it_move_right(ret.itblock, ret.itdata, n);
			else if(n < 0)
				it_move_left(ret.itblock, ret.itdata, -n);
			return ret; 
		}
		iterator operator-(const int &n) const
		{
			iterator ret = *this;
			ret.id -= n;
			if(n > 0)
				it_move_left(ret.itblock, ret.itdata, n);
			else if(n < 0)
				it_move_right(ret.itblock, ret.itdata, -n);
			return ret; 
		}
		int operator-(const iterator &rhs) const
		{
			if(dq != rhs.dq)
				throw invalid_iterator();
			else
				return id - rhs.id;
		}
		iterator& operator+=(const int &n)
		{
			return *this = *this + n;
		}
		iterator& operator-=(const int &n)
		{
			return *this = *this - n;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			*this = *this + 1;
			return tmp;
		}
		iterator& operator++()
		{
			return *this = *this + 1;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			*this = *this - 1;
			return tmp;
		}
		iterator& operator--()
		{
			return *this = *this - 1;
		}
		T& operator*() const
		{
			 if(this->id >= dq->size() || this->id < 0)
			 	 throw invalid_iterator();
			 else
			 	return *itdata->data;
		}
		T* operator->() const noexcept
		{
			if(this->id > dq->size() || this->id < 0)
			 	 throw invalid_iterator();
			 else
			 	return itdata->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return dq == rhs.dq && itblock == rhs.itblock && itdata == rhs.itdata && id == rhs.id;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return dq == rhs.dq && itblock == rhs.itblock && itdata == rhs.itdata && id == rhs.id;
		}
		bool operator!=(const iterator &rhs) const 
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}
	};
	class const_iterator
	{
	private:
		const Block *itblock;
		const datatype *itdata;
	public:
		const deque* dq;
		int id;
		const_iterator():itblock(nullptr), itdata(nullptr), dq(nullptr), id(-1) {}
		const_iterator(const deque* dd, const Block* bb, const datatype* tt, int iidd): dq(dd), itblock(bb), itdata(tt), id(iidd) {}
		const_iterator(const const_iterator &other):dq(other.dq), itblock(other.itblock), itdata(other.itdata), id(other.id){}
		const_iterator(const iterator &other):dq(other.dq), itblock(other.itblock), itdata(other.itdata), id(other.id) {}
		const_iterator operator+(const int &n) const
		{
			auto ret = *this;
			ret.id += n;
			if(n > 0)
				it_move_right(ret.itblock, ret.itdata, n);
			else if(n < 0)
				it_move_left(ret.itblock, ret.itdata, -n);
			return ret; 
		}
		const_iterator operator-(const int &n) const
		{
			const_iterator ret = *this;
			ret.id -= n;
			if(n > 0)
				it_move_left(ret.itblock, ret.itdata, n);
			else if(n < 0)
				it_move_right(ret.itblock, ret.itdata, -n);
			return ret; 
		}
		int operator-(const const_iterator &rhs) const
		{
			if(dq != rhs.dq)
				throw invalid_iterator();
			else
				return id - rhs.id;
		}
		const_iterator& operator+=(const int &n)
		{
			return *this = *this + n;
		}
		const_iterator& operator-=(const int &n)
		{
			return *this = *this - n;
		}
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			*this = *this + 1;
			return tmp;
		}
		const_iterator& operator++()
		{
			return *this = *this + 1;
		}
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			*this = *this - 1;
			return tmp;
		}
		const_iterator& operator--()
		{
			return *this = *this - 1;
		}
		const T& operator*() const
		{
			 if(this->id >= dq->size() || this->id < 0)
			 	 throw invalid_iterator();
			 else
			 	return *itdata->data;
		}
		const T* operator->() const noexcept
		{
			if(this->id > dq->size() || this->id < 0)
			 	 throw invalid_iterator();
			 else
			 	return itdata->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return dq == rhs.dq && itblock == rhs.itblock && itdata == rhs.itdata && id == rhs.id;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return dq == rhs.dq && itblock == rhs.itblock && itdata == rhs.itdata && id == rhs.id;
		}
		bool operator!=(const iterator &rhs) const 
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}
	};
	deque() 
	{
		root = new Block();
		allocateprp();
	}
	deque(const deque &other):totalsize(other.totalsize)
	{
		root = new Block();
		copy(other.root);
	}
	~deque()
	{
		deleteall();
		if(root != nullptr)
			delete root;
	}
	deque &operator=(const deque &other)
	{
		if(&other != this)
		{
			deleteall();
			copy(other.root);
			totalsize = other.totalsize;
		}
		return *this;
	}
	T & at(const size_t &pos)
	{
		if(pos >= size() || pos < 0)
			throw index_out_of_bound();
		else return findpos(pos);
	}
	const T & at(const size_t &pos) const
	{
		if(pos >= size() || pos < 0)
			throw index_out_of_bound();
		else return findpos(pos);
	}
	T & operator[](const size_t &pos)
	{
		if(pos >= totalsize || pos < 0)
			throw index_out_of_bound();
		else return findpos(pos);
	}
	const T & operator[](const size_t &pos) const
	{
		if(pos >= totalsize || pos < 0)
			throw index_out_of_bound();
		else return findpos(pos);
	}
	const T & front() const
	{
		if(totalsize == 0)
			throw container_is_empty();
		else
			return *root->next->datas[root->next->start].data;
	}
	const T & back() const
	{
		if(totalsize == 0)
			throw container_is_empty();
		else
			return *root->prv->datas[root->prv->end].data;
	}
	iterator begin()
	{
		return iterator(this, root->next, root->next->datas + root->next->start, 0);
	}
	const_iterator cbegin() const
	{
		return const_iterator(this, root->next, root->next->datas + root->next->start, 0);
	}
	iterator end()
	{
		return iterator(this, root->prv, root->prv->datas + root->prv->end+1, size());
	}
	const_iterator cend() const
	{
		return const_iterator(this, root->prv, root->prv->datas + root->prv->end+1, size());
	}
	bool empty() const
	{
		return totalsize == 0;
	}
	size_t size() const
	{
		return totalsize;
	}

	void clear() 
	{
		deleteall();
		totalsize = 0;
		allocateprp();
	}
	iterator insert(iterator pos, const T &value) 
	{
		allocateprp();
		if(pos.dq != this)
			throw invalid_iterator();
        if(size_t(pos.id) > size())
        	throw invalid_iterator();
        insert_at_id(pos.id, value);
        totalsize++;
        int tmpid = pos.id;
        Block *p = root->next;
        while(tmpid > p->size())
        {
        	tmpid -= p->size();
        	p = p->next;
        }
        return iterator(this, p, p->datas + p->start+tmpid, pos.id);
	}

	iterator erase(iterator pos)
	{
		if(this != pos.dq)
			throw invalid_iterator();
		if(size_t(pos.id) + 1 > size())
        	throw invalid_iterator();
        remove_at_id(pos.id);
        totalsize--;
        if(pos.id >= size())
        	return end();
        int tmpid = pos.id;
        Block *p = root->next;
        while(tmpid+1 > p->size())
        {
        	tmpid -= p->size();
        	p = p->next;
        }
        return iterator(this, p, p->datas + p->start+tmpid, pos.id);
	}
	void push_back(const T &value)
	{
		allocateprp();
		root->prv->push_back_block(value);
		totalsize++;
	}
	void pop_back()
	{
		if(totalsize == 0)
		{
			throw container_is_empty();
			return;
		}
		root->prv->pop_back_block();
		totalsize--;
	}
	void push_front(const T &value) 
	{
		allocateprp();
		root->next->push_front_block(value);
		totalsize++;
	}
	void pop_front()
	{
		if(totalsize == 0)
		{
			throw container_is_empty();
			return;
		}
		root->next->pop_front_block();
		totalsize--;
	}
};
}

#endif
