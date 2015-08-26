    /*
 * Tuple.h
 *
 *  Created on: Mar 10, 2015
 *      Author: chase
 */

#ifndef TUPLE_H_
#define TUPLE_H_


struct pack{};

template <typename T, typename... Tail>
struct tuple {
	T x;
	tuple<Tail...> rest;
	tuple(const T& x_init, const tuple<Tail...>& tail) : x{x_init}, rest{tail} {}
};

template <typename T> struct tuple<T> {
	T x;
	tuple(const T& x_init) : x{x_init} {}
};

template <typename T>
tuple<T>
operator+(pack, const T& x) {
	return tuple<T>{x};
}


template <typename T, typename T2>
tuple<T2, T> operator+(const tuple<T>& x, const T2& y) {
	return tuple<T2, T>(y, x);
}


template <typename T, typename... Tail>
tuple<T, Tail...>
operator+(const tuple<Tail...> & x, const T& y) {
	return tuple<T, Tail...>{y, x};
}


// I have try to comment these code and prog still work,so is that redundant?
//
//template <typename... T>
//struct head_type;
//
//template <typename T> struct head_type<T> { using type = T; };
//
//template <typename T, typename... Tail> struct head_type<T, Tail...> { using type = T;};
//
//template <typename... T>
//struct tail_type;
//
//template <typename T> struct tail_type<T> { using type = tuple<T>; };
//
//template <typename T, typename... Tail> struct tail_type<T, Tail...> {
//	using type = tuple<Tail...>;};
//


template <typename T>
void printFields(const tuple<T>& tup) {
	cout << tup.x;
}

template <typename... Args>
void printFields(const tuple<Args...>& tup) {
	printFields(tup.rest);
	cout << ", ";
	cout << tup.x;
}




#endif /* TUPLE_H_ */
