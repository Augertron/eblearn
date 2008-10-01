#ifndef __GENERATORS_H__
#define __GENERATORS_H__

namespace ebl {


template<typename T = int>
class Counter{
	
	typedef T value_type;
	value_type val;
	value_type interval;
	
public:
	
	Counter(const value_type& firstVal = 0, const value_type& interval = 1);
	
	inline value_type operator()();
};

} // end namespace ebl

#include "Generators.hpp"

#endif //__GENERATORS_H__
