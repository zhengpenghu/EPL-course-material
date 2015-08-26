/*
 * Source.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: chase
 */


#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

struct Base1 {
	int x;
	void doit(int v) { x = v; }
	void doit(void) {
		cout << "Hey! " << x << endl;
		cout << "this is: (Base1) " << this << endl;
	}
};
struct Base2 {
	int y;
	void doneit(void) {
		cout << "Yo " << y << endl;
		cout << "this is (Base2) " << this << endl;
	}
};

struct Derived : public Base1, public Base2 {
	int z;
	void doit(void) {
		cout << "Hey! " << z << endl;
		cout << "this is: (Derived) " << this << endl;
	}
};

struct Cake {
	virtual void eat(void) { cout << "yumm\n"; }
	virtual void chuck(void) { cout << "duck\n"; }
};

struct ChocolateCake : public Cake {
	void eat(void) { cout << "OMG!\n"; }
};

void consume(Cake& c) {
    c.eat();
    //c.eat();
}

int main(void) {
	ChocolateCake d;
	consume(d);
}

