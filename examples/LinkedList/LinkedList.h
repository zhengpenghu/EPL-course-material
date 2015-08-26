/*
 * LinkedList.h
 *
 *  Created on: Jan 27, 2015
 *      Author: chase
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_
#include <cstdint>
#include <stdexcept>

template <typename ElemType>
class LinkedList {
	using Same = LinkedList<ElemType>;
private:

	struct Cell {
		Cell* next = this;
		Cell* prev = this;
		ElemType data;

		Cell(const ElemType& x) : data{x} {}
	};

	Cell* dummy = makeDummy();
	uint64_t length = 0;

	/* the dummy cell has uninitialized (empty) data object inside it */
	static Cell* makeDummy(void) {
		/* the syntax
		 * 		operator new(size)
		 * differs from the normal use of the new operator which looks like this
		 *      new T
		 * Like all operator overloadings, there are two ways to invoke operator new, the first way (above) is
		 * function call syntax. The second way is operator syntax. With most operators, the function call
		 * syntax and the operator syntax have identical behavor. For example, if you have a function
		 *      T operator+(T x, T y) { some code here }
		 * then you can invoke that function in two ways and get precisely the same effect
		 *      T c = a + b; // operator syntax
		 *      T c = operator+(a, b); // function call syntax
		 * With most operators, the function syntax and the operator syntax are identical -- it's just syntactic sugar
		 * For some reason in C++ (bad judgment?) the new and delete operators are the exception.
		 * When you invoke "new T", it actually does two things, it allocates memory and then invokes a constructor for T.
		 * When you invoke "operator new(size), the constructor is not executed, you only allocate space.
		 * For even more obscure reasons, if you do want to run the constructor, you can do so with another syntactic
		 * variant of the new operator (it's called "placement new"). This perverse syntax goes as follows
		 *      new (address) T
		 * Where address is an expression of type T*. The compiler interprets placement new as a command to run the
		 * constructor for type T at the address you specified. So, to make a long story slightly shorter and vastly
		 * more precise....
		 *       T* p = new T{args...};
		 * is equivalent to
		 *       T* p = (T*) operator new(sizeof(T));
		 *       new (p) T{args...};
		 * where T is a type, and args... is a list of arguments that you want to pass to the constructor
		 */
		Cell* p = (Cell*) ::operator new(sizeof(Cell)); // p points to uninitialized memory, no constructors are run
		p->next = p;
		p->prev = p;
		return p;
	}

	static void deleteDummy(Cell* p) {
		::operator delete(p); // deallocates memory without running any destructors
	}

public:
	explicit LinkedList(int initial_size) {
		for (int k = 0; k < initial_size; k += 1) {
			append(ElemType{});
		}
	}

	LinkedList(void) {}
	LinkedList(const LinkedList<ElemType>& that) { copy(that); }


	template<typename T>
	LinkedList(const LinkedList<T>& that);

	~LinkedList(void) { destroy(); }

	Same& operator=(const Same& that) {
		if (this != &that) {
			destroy();
			copy(that);
		}
		return *this;
	}

	bool isEmpty(void) const { return head() == dummy; }

	void push_back(const ElemType& x) {
		Cell* p = new Cell{x};
		insertAfter(p, tail());
	}

	void push_front(const ElemType& x) {
		Cell* p = new Cell{x};
		insertAfter(p, head()->prev);
	}

	void pop_back(void) {
		if (isEmpty()) { throw std::out_of_range{"fatal error: removing from an empty list"}; }
		remove(tail());
	}

	void pop_front(void) {
		if (isEmpty()) { throw std::out_of_range{"fatal error: removing from an empty list"}; }
		remove(head());
	}

	class iterator {
	private:
		Cell* position = nullptr;
	public:
		iterator(void) {}
		iterator(Cell* p) : position{p} {}

		ElemType& operator*(void) { return position->data; }
		iterator& operator++(void) {
			position = position->next;
			return *this;
		}
		iterator operator++(int) {
			iterator t{*this}; // copy myself
			this->operator++(); // increment myself (pre-increment)
			return t; // return the copy of my old value
		}
		bool operator==(const iterator& rhs) const {
			return this->position == rhs.position;
		}
		bool operator!=(const iterator& rhs) const {
			return ! (*this == rhs);
		}

		iterator& operator--(void) {
			position = position->prev;
			return *this;
		}

		iterator operator--(int) {
			iterator t{*this};
			this->operator--();
			return t;
		}
	};

	iterator begin(void) {
		return iterator(head());
	}

	iterator end(void) {
		iterator result{tail()};
		++result;
		return result;
	}

private:
	/* with the dummy cell, the head is actually dummy->next
	 * and the tail is dummy->prev. In an empty list (and only in an
	 * empty list) both these pointers point to dummy.
	 */
	Cell* head(void) const { return dummy->next; }
	Cell* tail(void) const { return dummy->prev; }

	/* insert newCell into the linked list immediately after
	 * location, i.e., location->next == newCell
	 */
	void insertAfter(Cell* newCell, Cell* location) {
		newCell->prev = location;
		newCell->next = location->next;
		location->next->prev = newCell;
		location->next = newCell;
	}

	void remove(Cell* p) {
		p->prev->next = p->next;
		p->next->prev = p->prev;
		delete p;
	}

	void destroy(void);
	void copy(const Same& rhs) {
		Cell* p = rhs.head();
		while (p != rhs.dummy) {
			insert(p->data);
			p = p->next;
		}
	}

	template <typename T>
	friend class LinkedList;
};

template <typename T>
void LinkedList<T>::destroy(void) {
	Cell* p = dummy->next;
	while (p != dummy) {
		Cell* q = p;
		p = p->next;
		delete q;
	}
	deleteDummy(dummy);
}



#endif /* LINKEDLIST_H_ */
