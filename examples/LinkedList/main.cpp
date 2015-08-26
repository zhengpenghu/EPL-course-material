/*
 * main.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: chase
 */

#include <iostream>
#include <stdint.h> // the eclipse built-in editor doesn't understand <cstdint>

#include "LinkedList.h"

using std::cout;
using std::endl;


int main(void) {
	LinkedList<int> fred;
	fred.push_back(42);
	fred.push_back(1);
	fred.push_front(2);
	/* ... */

	for (const auto& x : fred) {
		cout << x << endl;
	}
}




