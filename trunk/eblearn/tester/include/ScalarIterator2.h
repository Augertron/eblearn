#ifndef SCALARITERATOR2_H_
#define SCALARITERATOR2_H_

#include "Idx.h"
#include <algorithm>

namespace ebl {

template<typename T>
class ScalarIterator {

public:
  ScalarIterator(Idx<T>& idx, bool* not_done)
    :ptr_(idx.idx_ptr()),
     spec_(idx.spec),
     not_done_(not_done) {

    *not_done_ = true;
    
    // if idx is 0-dimensional, pretend it's a size-1 vector.
    if ( spec_.ndim == 0 ) {
      spec_.unfold_inplace(0,1,1);
    }
    
    std::fill(inds_, inds_+spec_.ndim, 0);
    
    row_end_ = ptr_ + spec_.mod[spec_.ndim-1] * spec_.dim[spec_.ndim-1];
    iter_end_ = ptr_ + (spec_.mod[0] * spec_.dim[0]);

    // TODO: for contiguous dimensions, the step_sizes_ will be zero.
    // reshape spec to collapse such dimensions.
    for ( int dim_ind = 0; dim_ind < spec_.ndim-1; ++dim_ind ) {
      step_sizes_[dim_ind_] = spec_.mod[dim_ind_] - 
                              spec_.mod[dim_ind_+1]*spec_.dim[dim_ind_+1];
    }
    step_sizes_[spec_.ndim - 1] = spec_.mod[spec_.ndim - 1];
    dim_ind_ = spec_.ndim - 1;
    
    row_step_size_ = spec_.mod[spec_.ndim - 1];
    
    // for debugging 
   // row_end_ = idx.idx_ptr() + idx.dim(idx.order()-1);
  }

  inline void operator++() {
    //++ptr_;
    ptr_ += row_step_size_;
//    ptr_ += step_sizes_[dim_ind_];
    if ( ptr_ == row_end_ ) {
      if ( ptr_ == iter_end_ ) {
        *not_done_ = false;
      }
      else if ( spec_.ndim > 1 ) {
        --dim_ind_;
        ++inds_[dim_ind_];
        ptr_ += step_sizes_[dim_ind_];
        while ( inds_[dim_ind_] == spec_.dim[dim_ind_] && dim_ind_ > 0 ) {
          inds_[dim_ind_] = 0;
          --dim_ind_;
          ++inds_[dim_ind_];
          ptr_ += step_sizes_[dim_ind_];
        }
        dim_ind_ = spec_.ndim - 1;
        row_end_ = ptr_ + step_sizes_[dim_ind_] * spec_.dim[dim_ind_];
      }
    }
  }

  ScalarIterator<T>& operator=(const ScalarIterator<T>& other) {
    using std::copy;
    dim_ind_ = other.dim_ind_;
    ptr_ = other.ptr_;
    row_end_ = other.row_end_;
    iter_end_ = other.iter_end_;
    copy(other.inds_, other.inds_+other.spec_.ndim-1, inds_);
    copy(other.step_sizes_, other.step_sizes_ + other.spec_.ndim, step_sizes_);
    spec_ = other.spec_;
    return *this;
  }
  
  inline bool NotDone() const {
    return ptr_ != iter_end_;
  }
  
  inline double& operator*(){
    return *ptr_;
  }

private:
  // TODO(mkg): convert ints to intgs where appropriate.
  
  // local variable to operator++, allocated here for efficiency.
  int dim_ind_;
  
  // the current element
  T* ptr_;
  
  // marks the end of the current "row" 
  T* row_end_;
  
  // marks the end of the entire iteration
  T* iter_end_;
  
  // The first ndim-1 IDX indices of the the current element.
  double inds_[MAXDIMS-1];
  
  // step_sizes[i] is the amount to shift ptr_ by when 
  // looping dimension i (i.e. when incrementing dimension i-1).
  // We define step_sizes[ndim-1] to be mod[dim-1].
  int step_sizes_[MAXDIMS];
  
  // The amount to shift the iterator by 
  int row_step_size_;
  
  // This is not necessarily identical to the idx's spec.
  IdxSpec spec_;
  
  bool* not_done_;
};


//class SimpleIter_Base {
//  double* ptr_;
//  double* end_;
//  bool is_forward_;
//
//public:
//  
//  SimpleIter_Base(double* ptr, double* end) : ptr_(ptr), end_(end), is_forward_(true) {}
//  inline void incr(){
//    ++ptr_;
//  }
//  inline void decr(){
//    --ptr_; 
//  }
//  inline bool NotDone() {
//    return ptr_ != end_;
//  }
//};


class SimpleIter { //: public SimpleIter_Base {
public:
  SimpleIter(double* ptr, double* end) //: SimpleIter_Base(ptr,end) {}
    :ptr_(ptr), end_(end) {}
  
  inline void operator++(){
    ++ptr_;
  }
  
  inline bool NotDone() const {
    return ptr_ != end_;
  }
  
  double* ptr_;
  double* end_;
  
};


}



#endif /*SCALARITERATOR2_H_*/
