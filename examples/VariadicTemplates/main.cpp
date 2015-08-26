/*
 * main.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: chase
 */

#include <stdio.h>
#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

struct Print_exception {
	const char* msg;
	Print_exception(const char* m) : msg(m) {}
};

struct missing_argument_exception : public Print_exception {
	using Print_exception::Print_exception;
};

struct extra_argument_exception : public Print_exception {
	using Print_exception::Print_exception;
};

void Print(const char* s) {
	uint32_t k = 0;
	while (s[k]) {
		if (s[k] == '%') {
			throw missing_argument_exception("not enough arguments provided to print");
		}
		cout << s[k];
		k += 1;
	}
}

template <typename T>
void Print(const char* s, T arg) {
	uint32_t k = 0;
	while (s[k]) {
		if (s[k] == '%') {
			cout << arg;
			Print(&s[k+1]);
			return;
		}
		cout << s[k];
		k += 1;
	}
	throw extra_argument_exception("too many arguments provided to print");
}

template <typename T1, typename... ExtraArgTypes>
void Print(const char* s, T1 arg, ExtraArgTypes... extra_args) {
	uint32_t k = 0;
	while (s[k]) {
		if (s[k] == '%') {
			cout << arg;
			Print(&s[k+1],extra_args...);
			return;
		}
		cout << s[k];
		k += 1;
	}
	throw extra_argument_exception("too many arguments provided to print");
}

struct Foo {

};

std::ostream& operator<<(std::ostream& out, Foo) {
	return (out << "a Foo object");
}

int main(void) {
	int x = 42;
	const char* y = "the answer";
	double z = 3.14159;

	try {
		Print("Hello World oops!\n");
		Print("x is % that's cool!\n", 42);
		Print("x is % and y is % and z is %\n", x, y, z);
		Print("% x is % and y is % and z is %\n", Foo{}, x, y, z);
	} catch(Print_exception& e) {
		cout << "\noh crap! an exception: " << e.msg << endl;
	}
}
























#ifdef _notdef_


struct Print_exception {
	const char* msg;
	Print_exception(const char* m) : msg(m) {}
};

struct missing_argument_exception : public Print_exception {
	using Print_exception::Print_exception;
};

struct extra_argument_exception : public Print_exception {
	using Print_exception::Print_exception;
};

void Print(const char fmt[]) {
	uint64_t k = 0;
	while (fmt[k]) {
		if (fmt[k] == '%') {
			if (fmt[k+1] == '%') {
				k += 1;
			} else {
				throw missing_argument_exception("Print called with missing arguments");
			}
		}
		cout << fmt[k];
		k += 1;
	}
}

template <typename T, typename... More>
void Print(const char fmt[], T arg, More... others) {
	uint64_t k = 0;
	while (fmt[k]) {
		if (fmt[k] == '%') {
			if (fmt[k+1] == '%') {
				k += 1;
			} else {
				cout << arg;
				Print(&fmt[k+1], others...);
				return;
			}
		}
		cout << fmt[k];
		k += 1;
	}
	throw extra_argument_exception("Print called with extra arguments");
}

int main(void) {
	int x = 42;
	const char* y = "the answer";
	double z = 3.14159;
	try {
		Print("x is % y is % and z is %\n", x, y, z);
	} catch(Print_exception& e) {
		cout << "Print exception: " << e.msg << endl;
	}
}

#endif
