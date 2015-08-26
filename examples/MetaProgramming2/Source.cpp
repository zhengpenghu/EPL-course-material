/*
 * Source.cpp
 *
 *  Created on: Mar 26, 2015
 *      Author: chase
 */



#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

/*
 * decltype( EXPR )
 *   a compiler directive (not a function) that
 *   a) DOES NOT evaluate the expression
 *   b) is replaced with the type of the EXPR
 */

template <typename IT>
void partition(IT b, IT e) {
	using T = decltype(*b);
	T piv_value = *b;

}

/* decltype can be combined with auto (in a very awkward syntax) for specifying
 * function return types where the return type depends on the argument types
 */

template <typename IT>
auto min(IT b, IT e) -> decltype(*b) {
	decltype(*b) m = *b;
	++b;
	while (b != e) {
		if (*b < m) { m = *b; }
		++b;
	}
	return m;
}


/* substitution failure example
 * During the compiler's resolution of overloaded functions,
 *    if the compiler discovers a template, the instantiation of which would result in a type error,
 *    then the compiler must ignore that template and remove the template from the set of
 *    	possibilities for resolving the overloaded function
 *
 * Note that the rule is a "type error" not a "syntax error", so the error in the template
 * must relate to the type of the function's parameter(s) or the function's return value
 *   -- NOT something in the function's body
 *
 *
 * the substitution failure can be put to good use, especially when combined with decltype
 *   -- let decltype(expr) produce either a type, or A TYPE ERROR!
 *   -- introduce overload resolution so that in the event of a type error we select a different function
 */

/* this distance function works only if IT is a random access iterator */
template <typename IT>
uint64_t distance_broken(IT b, IT e) {
	return e - b;
}

/* decltype(e - b) would be a type (unsigned long long probably) if b and e are random access
 * and it would be a type error if b and e are other iterators
 *
 * So.. let distance return decltype(e - b)
 */

template <typename IT>
auto _distance(IT b, IT e, int) -> decltype(e - b) { return e - b; }

/* cool, now we have a function that can only be used for random-access iterators
 * but... what do we do for forward iterators?
 */

template <typename IT>
uint64_t distance_broken_too(IT b, IT e) {
	uint64_t d = 0;
	while (b != e) { ++b; d += 1; }
	return d;
}

/* this distance function works fine for forward iterators, so why is it "broken"?
 * If we had a linked list x and we tried distance(x.begin(), x.end())
 *    1) the random access version of distance is a type error. So, it's ignored
 *    2) the forward version of distance is OK, so the overload is resolved in favor of forward
 *
 * BUT, if we had a vector x and we tried distance(x.begin(), x.end())
 *    1) the random access version is OK, so it's considered a possible resolution
 *    2) the forward version is also OK, so it's also considered a possible resolution
 * the compiler then declares that our code is ambiguous since there are two overloaded versions of the same function.
 *
 * The trick is to declare one of the two distance functions with ... as an argument
 */

template <typename IT>
uint64_t _distance(IT b, IT e, ...) { // note, there would never be an argument for the ...
	uint64_t d = 0;
	while (b != e) { ++b; d += 1; }
	return d;
}

/* So, how does this ... trick help?
 * If we had a linked list x and we tried distance(x.begin(), x.end())
 *    1) the random access version of distance is a type error. So, it's ignored
 *    2) the forward version of distance is OK, and the ... matches the 0 arguments after b, e
 *       so the overload is resolved in favor of forward version distance(b, e, ...)
 *
 * If we had a vector x and we tried distance(x.begin(), x.end())
 *    1) the random access version is OK, so it's considered a possible resolution
 *    2) the forward version with ... is also OK, so it's also considered a possible resolution
 * the compiler then decides that the version of the distance function with two explicit parameters
 * is a more precise match to the invocation ("exactly two" parameters matches better than "two or more")
 * and it uses the random-access version.
 */

template <typename IT>
auto distance(IT b, IT e) -> decltype(_distance(b, e, 0)) { return _distance(b, e, 0); }

/* Foo is a stupid "data structure" with a stupid iterator, but nicely illustrates how we
 * can use decltype and substition failure to select between the "forward iterator" style of
 * distance function and the "random-access iterator" style of distance function.
 *
 * In the iterator below, the forward iterator version of distance will always be zero
 * (since begin() and end() are always equal). But the random-access version of distance is
 * ten (since operator- always returns ten).
 */
struct Foo {
	struct iterator {
		bool operator==(iterator) const { return true; }
		bool operator!=(iterator) const { return false; }
		int operator*(void) const { return 42; }
		iterator& operator++(void) { return *this; }

		/* if we comment-out the next line, this iterator becomes forward only */
		int64_t operator-(iterator) const { return 10; }
	};

	iterator begin(void) const { return iterator{};}
	iterator end(void) const { return iterator{}; }
};

/* sometimes we want to use substitution failure to "enable" or disable methods or operators in a template
 * class. In the Bar class below, the operator for adding Bar x to Bar y (i.e., Bar::operator+(Bar)
 * only makes sense if the template argument T also supports addition.
 * Rather than permitting the syntax error that would result from instantiating the operator if
 * T were a type (e.g., int*) where addition is not possible (can't add two pointers), we disable the
 * function completely. The client gets an error either way, but by disabling the operator, we keep
 * the possibility of writing a different operator later.
 */
template <typename T>
struct Bar {
	T x;

	Bar<decltype(x + x)> operator+(const Bar<T>& b) { return Bar<T>{x + b.x}; }
};



/* we can generalize this idea, "enabling functions", and the standard library contains a
 * meta function std::enable_if<bool, T> for doing exactly that. In the code below, I
 * implement this function and add my own convenience template alias EnableIf to make it easier to use
 */

template <bool, typename T> struct enable_if; // template declaration
template <typename T> struct enable_if<true, T> { // when the condition is true, export T
	using type = T;
};

template <typename T> struct enable_if<false, T> {}; // when the condition is false, export nothing

/*
 * If we use enable_if<condition, T>::type and the condition is false, then we'll get an error, since
 * there is no "type" defined in the false specialization of enable_if. So, if we want to create a function
 * with a type error, then we can simply use enable_if to specify the return type of the function.
 *
 * Since the intended use of enable_if is to specify the return type of functions, and since most functions have
 * return types, enable_if has a second template argument that is the return type we really want.''
 *
 * Here's a simple enable_if example where the function doit is enabled only if sizeof(X) is less than 5.
 * So, if X is a type like int, then doit compiles. If X is a type like double, then doit doesn't exist!
 * Note that when doit exists, its return type is "void".
 */

template <typename X>
typename enable_if< (sizeof(X) < 5) , void>::type
doit(X x ) {
	cout << "Hello World\n";
}

/* the std::enable_if syntax gets tedious, so I prefer creating a type alias to get rid of the ::type (and the typename)
 */
template <bool p, typename T>
using EnableIf = typename enable_if<p, T>::type;

template <typename X>
EnableIf<(sizeof(X) > 5), void> doit(X x) {
	cout << "bigger input, still Hello!\n";
}

/* EnableIf gives us the ability to turn a feature on/off based on a boolean value
 * But.... how do we get the boolean value? What about asking the question
 *
 * Does type T have a T::foo function?
 * Can two objects of type T, x and y be added together?
 * Well, with a little bit of decltype, we can write meta functions that give us the booleans we're loooking for
 *
 * Let's start with "has_foo_method" which should return true if and only if T::foo(void) exists
 * to keep things super simple, we'll limit the question to a static method foo
 */


#include <memory>

template <typename T, typename not_used=decltype(*std::declval<T>())>
std::true_type are_you_pointer(T x) {
	return std::true_type{};
}

std::false_type are_you_pointer(...) {
	return std::false_type{};
}

template <typename T>
struct AreYouPointer : public decltype(
										are_you_pointer(std::declval<T>())
										) {};


#include <vector>
void enable_tests(void) {
	boolalpha(cout);
	int x;

	cout << AreYouPointer<int>::value << endl;
	cout << AreYouPointer<int*>::value << endl;
	cout << AreYouPointer<std::shared_ptr<int>>::value << endl;
	cout << AreYouPointer<std::vector<int>::iterator>::value << endl;
//	cout << are_you_pointer(x) << endl;
//	cout << are_you_pointer(&x) << endl;
//	std::shared_ptr<int*> p;
//	cout << are_you_pointer(p) << endl;
}


int main(void) {
	enable_tests();
	return 0;

	int x[] = { 5, 3, 42, 7, 9 };
	cout << "min x: " << min(x, x + 5) << endl;

	Foo f;
	cout << "distance x: " << distance(f.begin(), f.end()) << endl;

	doit(42);
	doit(3.5);
}
