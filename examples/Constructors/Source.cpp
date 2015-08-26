#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

/*
 * Design Challenge
 *
 * I have a linked list cell type Cell<T>. I want to have "delete myCell" unlink
 * the cell from a linked list and then deallocate the object inside (this may or may not
 * be bad style!)
 *
 * Since my destructor will have serious side effects, assume I want to make the Cell
 * object allocatable only on the heap -- i.e., I want to make it impossible to declare
 * variables of type Cell<T>, only Cell<T>*
 *
 * What constructors should I have, what should my destructor look like?
 */

template <typename T>
struct Cell {
private:
	using Same = Cell<T>;
	T data;
	Same* next;
	Same* prev;

	Cell(T&& x) : data(std::forward(x)) {}
};
