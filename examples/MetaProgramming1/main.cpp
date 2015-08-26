#include <iostream>
#include <cstdint>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <complex>

using std::cout;
using std::endl;

namespace epl {

template <typename T>
struct rank;

template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };
template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <int R>
struct stype;

template <> struct stype<1> { using type = int; };
template <> struct stype<2> { using type = float; };
template <> struct stype<3> { using type = double; };

template <typename T> struct is_complex : public std::false_type {};
template <typename T> struct is_complex<std::complex<T>> : public std::true_type {};

template <bool p, typename T> struct ctype;
template <typename T> struct ctype<true, T> { using type = std::complex<T>; };
template <typename T> struct ctype<false, T> { using type = T; };


template <typename T1, typename T2>
struct choose_type {
	static constexpr int t1_rank = rank<T1>::value;
	static constexpr int t2_rank = rank<T2>::value;
	static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;

	using my_stype = typename stype<max_rank>::type;

	static constexpr bool t1_comp = is_complex<T1>::value;
	static constexpr bool t2_comp = is_complex<T2>::value;
	static constexpr  bool my_comp = t1_comp || t2_comp;

	using type = typename ctype<my_comp, my_stype>::type;
};

template <typename T1, typename T2>
typename choose_type<T1, T2>::type max(const T1& x, const T2& y) {
	if (x < y) { return y; }
	else { return x; }
}

template <typename T>
struct is_reference : public std::false_type {};

template <typename T>
struct is_reference<T&> : public std::true_type {};

/*
 * silly arcana in C++
 * T&& & becomes T&
 * T&& && becomes T&&
 *
 * The specialized is_reference template relies on this arcana and the method by which the compiler
 * deduces a template argument. The template argument "T" for is_reference can be deduced as either
 * a reference (e.g,. T could be int&) or as an ordinary type (T could be int).
 */

template <typename T>
const T foo(T&& x) {

	if (is_reference<T>::value) {
		cout << "cool, a reference]\n";
	} else {
		cout << "rats, no ref\n";
	}

	cout << "T was guessed as type " << typeid(T).name() << endl;
	return x;
}

}
using namespace epl;

int main(void) {
	int x = 42;
	int y = 13;

	cout << "x "; foo(x);
	cout << "42 "; foo(42);
    cout<<"x+y";foo(x+y);
	foo(x) = 3;

	cout << "the max of 5 and 3.14159 is " << max(5, 3.14159) << endl;
	cout << "type is " << typeid(max(5, 3.0f)).name() << endl;

	cout << "the max is: " << max(x, y) << endl;
	//max(x, y) = 0;
	cout << "the max is now " << max(x, 42) << endl;
	cout << "x is " << x << ", and y is " << y << endl;
}
