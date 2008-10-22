#ifndef GENERATORS_HPP_
#define GENERATORS_HPP_

namespace ebl {

template<typename T> 
Counter<T>::Counter( const T& firstVal, const T& interval )
	:val(firstVal-interval),
	 interval(interval)
{}

template<typename T>
T Counter<T>::operator()(){
	val += interval;
	return val;
}

} // end namespace ebl

#endif /*GENERATORS_HPP_*/
