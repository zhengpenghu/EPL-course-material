/*
 * main.cpp
 *
 *  Created on: Jan 29, 2015
 *      Author: chase
 */

#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

class Foo;

Foo* global_pointer;
int global_int = 0;

class Foo {
	const char* msg;
	int data = 0;
public:
	Foo(void) {
		msg = "default";
		global_int += 1;
		data = global_int;
		cout << "constructing default: " << data << endl;
	}

	Foo(const char* msg) {
		cout << "constructing: " << msg << endl;
		this->msg = msg;
	}

	~Foo(void) {
		cout << "destructing: " << msg << " " << data << endl;
	}


	void doit(int x) {
		global_pointer = this;
		global_int = x;
	}
};

void global_doit(Foo * p, int x) {
	global_pointer = p;
	global_int = x;
}


int main(void) {
	Foo f{"first"};
	Foo g{"second"};
	Foo* h = new Foo;
	delete h;
}


