/*
 * main.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: chase
 */


#include <cstdint>
#include <iostream>
#include <string>

using std::cout;
using std::endl;


#include "Tuple.h"
#include "Functions.h"

int main(void) {
//in the tuple.h
    auto x = pack{} + 42 + 3.14159 + 3 + std::string("Hello World\n");
	printFields(x);
    //tuple----x
    //         rest
    //           |
    //           ------x
    //              |
    //              ---rest
//in the function.h
	auto f = IdentityFunction{} + IdentityFunction{};
	cout << f(21) << endl;
	auto g = _1 - (_1 - _2);
    //auto g=_1-_2;
    //cout<<g.g.g;
    cout<<"before cout"<<endl;
    cout << g(25, 5, 10) << endl;
    //25-(25-5)
    //first -(second-first)
    //totally lost to understand it
}

