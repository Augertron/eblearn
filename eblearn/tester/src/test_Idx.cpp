#include "test_Idx.h"
#include "Generators.h"
#include "Idx.h"
#include "IdxIterators.h"
#include <iostream>

using namespace ebl;

void test_printElems(){
	
	// A 4-dimensional tensor
	Idx<double> tensor(2,3,4,5);
	
	// Fill it with sequential integers.
	std::generate(tensor.scalars_begin(), tensor.scalars_end(), Counter<double>());
	
	// Pretty-print to standard output
	tensor.printElems(std::cout);
}

