/*
 * Source.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: chase
 */

#include <iostream>
#include <cstdint>
#include <utility>

using std::cout;
using std::endl;
using std::swap;

//template <typename FI>
//FI partition(FI b, FI e) {
//	/* invariant, all values in [b, p) are less than or equal to piv_val
//	 * and all values in [p, t) are strictly larger than piv_val
//	 */
//
//	FI p{b};
//	FI t{b};
//	++t;
//
//	/* invariant is established since [b,p) and [p, t) are both empty */
//	while (!(t == e)) {
//		if (*t < *p) { // next value is too small, need to move it to the small section
//			swap(*t, *p); // swap it with the value at partition pos, the first of our large values
//			++p; // increment the partition pos, moving it past the small value we just place there
//			swap(*t, *p);
//		}
//		++t;
//	}
//
//	return p;
//}

void qsort(void* p, size_t n, size_t obj_size, int (*fun)(void*, void*));

struct IntCompThing {
public:
	bool operator()(int x, int y) { return x < y; }
};

struct IntCompGTThing {
public:
	bool operator()(int x, int y) { return x > y; }
};

bool intLessThan(int x, int y) { return x < y; }
bool intGreaterThan(int x, int y) { return x > y; }

template <typename FI, typename Comp>
FI partition(FI b, FI e, Comp f) {
	/* use *b as pivot value */
	using T = typename std::iterator_traits<FI>::value_type;

	T piv = *b;
	FI p{b};
	FI p2{b};
	++p;
	FI t{b};
	++t;

	/* invariant:
	 * elements in [b, p) are strictly less than piv
	 * elements in [p, t) are not less than piv
	 * p2 is one position behind p, i.e., ++p2 == p
	 * p > b
	 * p2 >= b
	 *
	 * i.e., p2 is the "last of the smalls" while p is "first of the bigs"
	 * We've saved the pivot value at *b, at the end, we swap this value int p2
	 * Note: if the pivot is the smallest value, then p2 will remain equal to b
	 */

	while (! (t == e)) {
		if (f(*t, piv)) {
			swap(*t, *p);
			++p;
			++p2;
		}
		++t;
	}
	swap(*b, *p2);

	return p2;
}

template <typename RAI>
uint64_t distance(RAI b, RAI e, std::random_access_iterator_tag t) {
	return e - b;
}

template <typename FI>
uint64_t distance(FI b, FI e, std::forward_iterator_tag t) {
	uint64_t len = 0;
	while (b != e) {
		len += 1;
		++b;
	}
	return len;
}

template <typename T>
struct Iterator_traits {
	typedef typename T::value_type value_type;
	using iterator_category = typename T::iterator_category;
};

template <typename T>
struct Iterator_traits<T*> {
	using value_type = T;
	using iterator_category = std::random_access_iterator_tag;
};

template <typename I>
uint64_t distance(I b, I e) {
	typename Iterator_traits<I>::iterator_category x{};
	return distance(b, e, x);
}


template <typename FI, typename Comp=std::less<typename std::iterator_traits<FI>::value_type>>
void quickSort(FI b, FI e, Comp f = Comp{}) {
	/* check base case (length of sequence is less than 2) */
	if (distance(b, e) < 2) { return; }

	FI p = partition(b, e, f);

	/* recurisvely sort [b, p) and (p, e) */
	quickSort(b, p, f);
	++p;
	quickSort(p, e, f);
}

template <typename T>
struct DefaultCompareLT { // also known as "std::less"
	bool operator()(const T& x, const T& y) {
		return x < y;
	}
};


template <typename FI>
void quickSort(FI b, FI e) {
	using T = typename std::iterator_traits<FI>::value_type;
	quickSort(b, e, std::less<T>{});
}

int main(void) {
	int x[] = { 4, 2, 7, 3, 9, 5, 8, 1, 6, 0, 5, 5, 3, 3 };
	const uint32_t n = sizeof(x) / sizeof(x[0]);

	const char* prefix = "";
	for (const auto& v : x) {
		cout << prefix << v;
		prefix = ", ";
	}
	cout << endl;

	quickSort(x, x + n);


	prefix = "";
	for (const auto& v : x) {
		cout << prefix << v;
		prefix = ", ";
	}
	cout << endl;


}


