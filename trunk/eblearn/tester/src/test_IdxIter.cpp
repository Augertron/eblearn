#include "test_IdxIter.h"
#include "Idx.h"
#include "IdxIterators.h"
#include "Generators.h"
#include "Blas.h"
#include <iostream>
#include <ctime>
//#include <parallel/algorithm>
#include <iterator>

using namespace std;
using namespace ebl;

void test_ElemIter_shifting(){
	typedef double real;
	Idx<real> idx(3, 5, 2);
	int ii = 0;
	cout<<"Setting and printing the idx elems in sequence using ScalarIterator:"<<endl;
	for( Idx<real>::scalar_iterator iter(idx); iter.notdone(); ++iter){
		*iter = ii++;
		cout<<"idx's *iter = "<<*iter<<endl;
	}
	cout<<endl;
	

	int tpInds[3] = {1,0,2}; 
	Idx<real> idx2= idx.narrow(2,1,0).transpose(tpInds); //.narrow(1,3,1);

	
	cout<<"Printing idx2 elems in sequence using for loops:"<<endl;
	
	for( int ii = 0; ii < idx2.dim(0); ++ii ){
		for( int jj = 0; jj < idx2.dim(1); ++jj ){
			for( int kk = 0; kk < idx2.dim(2); ++kk ){
				cout<<"idx2.get("<<ii<<", "<<jj<<", "<<kk<<") = "<<idx2.get(ii,jj,kk)<<endl;
			}
		}
	}
	cout<<endl;
	
	cout<<"Printing the idx2 elems using iter:"<<endl;
	for( Idx<real>::scalar_iterator iter(idx2); iter.notdone(); ++iter){
		cout<<"idx2's "<<iter<<endl;//<<" at inds "<<iter.inds[0]<<", "<<iter.inds[1]<<", "<<iter.inds[2]<<endl;
	}
	
	cout<<endl<<"Printing the idx2 elems in reverse using ReverseScalarIterator:"<<endl;
	for( Idx<real>::reverse_scalar_iterator iter(idx2); iter.notdone(); ++iter){
		cout<<"idx2revers'es "<<*iter<<endl;//<<" at inds "<<iter.inds[0]<<", "<<iter.inds[1]<<", "<<iter.inds[2]<<endl;
	}
	
	cout<<endl<<"Testing parallel iteration; one forwards, one backwards"<<endl;
	{
		Idx<real>::reverse_scalar_iterator riter(idx2);
		Idx<real>::scalar_iterator iter(idx2);
		for( ; riter.notdone(); ++riter, ++iter ){
			cout<<"iter, riter = "<<iter<<"\t "<<riter<<endl;
		}
	}
	
	cout<<endl<<"Testing parallel iteration; both forwards"<<endl;
	for( Idx<real>::scalar_iterator ii(idx2), jj(idx2); ii.notdone(); ++ii, ++jj ){
		cout<<"ii, jj = "<<ii<<" "<<jj<<endl;
	}
	
}

void test_ElemIter_parallelism(){
	Idx<unsigned char> idx1(50, 1024, 1024); // 50 meg image stack
	
	clock_t startTime = clock();
	std::generate( idx1.scalars_begin(), idx1.scalars_end(), &std::rand );
	clock_t duration = clock() - startTime;
	cout<<"executed in "<<double(duration)/CLOCKS_PER_SEC<<endl;
}

void test_DimensionIterator_shifting(){
	
	Idx<double> idx1(2,3,4);
	for( int ii = 0; ii < idx1.dim(0); ++ii ){
		for( int jj = 0; jj < idx1.dim(1); ++jj ){
			for( int kk = 0; kk < idx1.dim(2); ++kk ){
				idx1.set( ii*100 + jj*10 + kk, ii,jj,kk);
			}
		}
	}
	
	cout<<"### iterating through dim 0 ###"<<endl;
	typedef Idx<double>::dimension_iterator DIter;
	for( DIter ii(idx1,0); ii.notdone(); ++ii){
		ii->pretty(stdout);
	}
	
	
	cout<<"### iterating through dim 1 ###"<<endl;
	for( DIter jj(idx1,1); jj.notdone(); ++jj){
		jj->pretty(stdout);
	}
	
	cout<<"### iterating through dim 2 ###"<<endl;
	for( DIter kk(idx1, 2); kk.notdone(); ++kk){
		kk->pretty(stdout);
	}
	
}

void test_matrixMultiply(){
	typedef Idx<double> Tensor;
	typedef Tensor::dimension_iterator DIter;
	typedef Tensor::scalar_iterator SIter;
	
	Tensor mat(2,3);
	Tensor vec(mat.dim(1));
	Tensor outvec(mat.dim(0));
	
	// fill with sequential numbers
	std::generate(mat.scalars_begin(), mat.scalars_end(), Counter<>());
	std::generate(vec.scalars_begin(), vec.scalars_end(), Counter<>());

	// print
	std::cout<<"mat:\n";
	mat.printElems(std::cout);
	std::cout<<"\nvec: ";
	vec.printElems(std::cout);
	std::cout<<"\n"<<std::endl;
	
	// iterate over rows
	DIter rowIter(mat,0);
	SIter vecElemIter(vec), outElemIter(outvec);
	for( ; rowIter.notdone(); ++rowIter, ++vecElemIter, ++outElemIter){
		*outElemIter = idx_dot(*rowIter, vec);
	}

	std::cout<<"output vec: ";
	outvec.printElems(std::cout);
	std::cout<<std::endl;
}

