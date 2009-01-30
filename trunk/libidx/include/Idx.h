/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef Idx_H
#define Idx_H

#define USING_STL_ITERS 1

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "LibidxDefines.h"
#include "Srg.h"

namespace ebl {

  /*! \mainpage libidx Library Main Page
   *
   * \section intro_sec Introduction
   *
   * This is the introduction.
   *
   * \section install_sec Installation
   *
   * \subsection step1 Step 1: TODO
   *  
   * TODO
   */

  const int MAXDIMS=8;

  // Forward declarations of iterators
  template<typename T> class ScalarIter;
  template<typename T> class ReverseScalarIter;
  template<typename T> class DimIter;
  template<typename T> class ReverseDimIter;

  ////////////////////////////////////////////////////////////////

  class dummyt {  bool someunk; };

  //! IdxSpec contains all the characteristics of an Idx,
  //! except the storage. It includes the order (number of dimensions)
  //! the offset, and the dim and mod arrays.
  //! having an IdxSpec class separate from Idx allows us to write
  //! generic manipulation functions that do not depend on the
  //! type of the storage.
  class IdxSpec {

  private:

    //! private method to set the order (number of dims) to n.
    //! Calling this with n = 0 deallocates the dim and mod arrays.
    //! The dim and mod arrays are reallocated as necessary when
    //! the order is increased.
    int setndim(int n);

    //! private method to set the order to n, and the dim and
    //! mod arrays to pre-allocated arrays ldim and lmod.
    int setndim(int n, intg *ldim, intg *lmod);

    //! number of dimensions
    int ndim;
    //! offset in the storage
    intg offset;
    //! array of sizes in each dimension
    intg *dim;
    //! array of strides in each dimension
    intg *mod;

    //! resize the spec and return the new footprint.
    //! this is private because only Idx can call this
    //! so that the storage gets properly resized.
    //! We do not allow the order to be changed with this,
    //! only the size of each dimension can be modified.
    //! The resized spec will be contiguous but will have
    //! the same offset as the original.
    intg resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
		intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);

    template<typename SizeIter> intg resize( SizeIter& dimsBegin, 
					     SizeIter& dimsEnd ){
      const int nArgDims = std::distance(dimsBegin, dimsEnd);

      // Error-check the supplied number of dims.
      if( ndim == 0 ){
	ylerror("Cannot call resize on a 0-dimensional IdxSpec.");
      }
      else if( ndim != nArgDims ){
	std::ostringstream oss;
	oss<<"Number of supplied dimension sizes ("<<nArgDims;
	oss<<") doesn't match IdxSpec's number of dims ("<<ndim<<")";
	ylerror(oss.str().c_str());
      }

      // copy dimensions to dim
      std::copy(dimsBegin, dimsEnd, dim);

      // set mod to be the partial sum of the dim sequence, in reverse order.
      typedef std::reverse_iterator<SizeIter> RIter;
      typedef std::reverse_iterator<intg*> RintgIter;
      std::partial_sum( RIter(dimsEnd-1),
			RIter(dimsBegin-1),
			RintgIter(mod+(nArgDims-1)),
			std::multiplies<intg>() );

      // return the memory footprint
      return mod[0] * dim[0] + offset;
    }

    //! set the offset and return the new value
    intg setoffset(intg o) { return offset = o; }

  public:

    //! the destructor of IdxSpec deallocates the dim and mod arrays.
    ~IdxSpec();

    //! assignment operator overloading.
    const IdxSpec& operator=( const IdxSpec& src);

    //! copy IdxSpec src into current IdxSpec
    void copy( const IdxSpec& src);

    //! copy constructor from IdxSpec src
    IdxSpec( const IdxSpec& src);

    //! This creates an IdxSpec0 with offset 0.
    //! This can be used to build an empty/blank IdxSpec.
    IdxSpec();

    //! Creates an IdxSpec0 with offset o.
    IdxSpec(intg o);

    //! Creates an IdxSpec1 with offset o.
    IdxSpec(intg o, intg size0);

    //! Creates an IdxSpec2 with offset o.
    IdxSpec(intg o, intg size0, intg size1);

    //! Creates an IdxSpec3 with offset o.
    IdxSpec(intg o, intg size0, intg size1, intg size2);

    //! Generic constructor with offset.
    IdxSpec(intg o, intg s0, intg s1, intg s2, intg s3,
	    intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);

    //! Creates an IdxSpec of order n from arrays of dims and mods.
    //! The arrays are copied.
    IdxSpec(intg o, int n, intg *ldim, intg *lmod);

    //! return the offset of IdxSpec
    intg getoffset() { return offset; }

    //! return the order (number of dimensions).
    int getndim() { return ndim; }

    //! return the memory footprint, including the offset.
    //! The storage of an Idx containing this IdxSpec must
    //! be have at least this size.
    intg footprint()  {
      intg r = offset + 1;
      for(int i=0; i<ndim; i++){ r += mod[i]*(dim[i]-1); }
      return r;
    }

    //! total number of elements accessed by IdxSpec
    intg nelements() {
      intg r = 1;
      for(int i=0; i<ndim; i++){ r *= dim[i]; }
      return r;
    }

    //! returns true if the IdxSpec elements are
    //! in a continuous chunk of memory. This is useful
    //! to optimize iterators over the data.
    bool contiguousp() {
      intg size = 1; bool r = true;
      for(int i=ndim-1; i>=0; i--){
	if (size != mod[i]) r = false;
	size *= dim[i];
      }
      return r;
    }

    //! pretty-prints the IdxSpec on the specified file.
    void pretty(FILE *f);
    void pretty(std::ostream& out);

    //! select: return a new IdxSpec corresponding to
    //! a slice of the current IdxSpec with slice i
    //! in dimension d. In other words, if m is an
    //! IdxSpec of order 2 of size (10,4), the call
    //! IdxSpec p = m.select(0,3) will set p to
    //! an IdxSpec or order 1 containing the 4-th
    //! row of m.
    IdxSpec select(int d, intg i);
    //! select_into: same as select, but modifies an existing
    //! IdxSpec instead of returning a new one.
    intg select_into(IdxSpec *dst, int d, intg n);
    //! select_inplace: same as select, but modifies the
    //! current IdxSpec.
    intg select_inplace(int d, intg i);

    //! narrow: return a new IdxSpec in which the d-th
    //! dimension has been reduced to size s, starting
    //! at item o. In other words, if m is an
    //! IdxSpec of order 2 of size (10,4), the call
    //! IdxSpec p = m.narrow(0,6,2) will set p to
    //! an IdxSpec or order 2 of size (6,4) whose rows
    //! are rows 2 to 7 of m.
    IdxSpec narrow(int d, intg s, intg o);
    //! narrow_into: same as narrow, but modifies an existing
    //! IdxSpec instead of returning a new one.
    intg narrow_into(IdxSpec *dst, int d, intg s, intg o);
    //! narrow_inplace: same as narrow, but modifies the
    //! current IdxSpec.
    intg narrow_inplace(int d, intg s, intg o);

    //! transpose: transpose dimensions d1 and d2.
    IdxSpec transpose(int d1, int d2);
    //! transpose all dimensions through permutation matrix p.
    IdxSpec transpose(int *p);
    //! same as transpose, but modifies an existing
    //! IdxSpec instead of returning a new one.
    int transpose_into(IdxSpec *dst, int d1, int d2);
    //! same as transpose, but modifies an existing
    //! IdxSpec instead of returning a new one.
    int transpose_into(IdxSpec *dst, int *p);
    //! transpose_inplace: same as transpose, but modifies the
    //! current IdxSpec.
    int transpose_inplace(int d1, int d2);
    //! transpose_inplace: same as transpose, but modifies the
    //! current IdxSpec.
    int transpose_inplace(int *p);

    //! unfold: prepare an IdxSpec for a convolution.
    //! Returns an idx on the same storage as m (pointing to the
    //! same data) with an added dimension at the end obtained by
    //! "unfolding" the n -th dimension. The size of the new dimension
    //! is k. This essentially manipulates the mod array to make
    //! convolutions look like matrix-vector multiplies.
    IdxSpec unfold(int d, intg k, intg s);
    //! same as unfold, but modifies an existing
    //! IdxSpec instead of returning a new one.
    intg unfold_into(IdxSpec *dst, int d, intg k, intg s);
    //! unfold_into: same as unfold, but modifies the
    //! current IdxSpec.
    intg unfold_inplace(int d, intg k, intg s);

    // horrible syntax to declare a template class friend.
    // isn't C++ wonderful?
    template <class T> friend class IdxIter;
    template <class T> friend class IdxLooper;
    template <class T> friend class Idx;
    template <class T> friend class ScalarIter_Base;
    template <class T> friend class ScalarIter;
    template <class T> friend class ReverseScalarIter;
    template <class T> friend class DimIter_Base;
    template <class T> friend class DimIter;
    template <class T> friend class ReverseDimIter;


    friend bool same_dim(IdxSpec &s1, IdxSpec &s2);
  };

  //! return true if two idxspec have the same dimensions,
  //! i.e. if all their dimensions are equal (regardless
  //! of strides).
  bool same_dim(IdxSpec &s1, IdxSpec &s2);


  ////////////////////////////////////////////////////////////////

  //! Idx: main tensor class. This can represent vectors,
  //! matrices, and tensors up to 8 dimensions.
  //! An Idx is merely an access structure that
  //! points to the data. Several Idx can point to the same
  //! data.
  template <class T> class Idx {

    //! Pretty-prints elements to a stream.
    //friend std::ostream& operator<<( std::ostream& out, Idx<T>& tensor );

  private:

    //! pointer to the Srg structure that contains the data.
    Srg<T> *storage;

    //! increase size of storage to fit the current dimension
    void growstorage();

    //! increase size of storage to fit the current dimension + a given size
    void growstorage_chunk(intg s_chunk);

    //! Implementation of public printElems() method.
    void printElems_impl( int indent, std::ostream& );

  protected:
    //! fake constructor that does nothing.
    //! This is called by the IdxLooper constructor
    Idx(dummyt *dummy);

  public:

    // STL-like typedefs
    typedef T value_type;
    typedef ScalarIter<value_type> scalar_iterator;
    typedef ReverseScalarIter<value_type> reverse_scalar_iterator;
    typedef DimIter<value_type> dimension_iterator;
    typedef ReverseDimIter<value_type> reverse_dimension_iterator;

    /* -----  STL-style iterator creators  ----- */

    //! Returns an iterator over all the elements.
    scalar_iterator scalars_begin();

    //! Returns a terminated iterator over all the elements.
    scalar_iterator scalars_end();

    //! Returns a reversed-order iterator over all the elements.
    reverse_scalar_iterator scalars_rbegin();

    //! Returns a terminated reversed-order iterator over all the elements.
    reverse_scalar_iterator scalars_rend();

    //! Returns an iterator for the given dimension.
    dimension_iterator dim_begin( int dim );

    //! Returns a terminated iterator for the given dimension.
    dimension_iterator dim_end( int dim );

    //! Returns an iterator for the given dimension.
    reverse_dimension_iterator dim_rbegin( int dim );

    //! Returns a terminated iterator for the given dimension.
    reverse_dimension_iterator dim_rend( int dim );


    //! IdxSpec that contains the order,
    //! offset, dimensions and strides.
    IdxSpec spec;

    //! Pretty-prints IDx metadata to a file pointer.
    virtual void pretty(FILE *);
    virtual void pretty(std::ostream& out);

    //! Pretty-prints elements to a stream.
    virtual void printElems(); // calls printElems( std::cout );
    virtual void printElems( std::ostream& out );
    // void printElems( FILE* out );  doesn't work (cf implementation)

    //! destructor: unlocks the Srg.
    virtual ~Idx();

    // TODO: Find out why code such as Idx<float> = 1 compiles
    // (even without default operator below).
    // default operator below outputs an error that this is forbidden.
    virtual Idx<T>& operator=(T other){
      ylerror("Forbidden Idx assignment. Idx can only be assigned another Idx");
      return *this;
    }

    virtual Idx<T>& operator=(const Idx<T>& other) {
      Srg<T> *tmp = NULL;
      if (this->storage != NULL)
	tmp = this->storage;
      this->storage = other.storage;
      this->spec = other.spec;
      this->storage->lock();
      if (tmp)
	tmp->unlock();
      return *this;
    }

    virtual Idx<T> operator[](const intg i) {
      return this->select(0,i);
    }

    //! Copy constructor. Prevents implcit calls via '='.
    // Keep this 'explicit' until we decide to implement operator=.
    //Idx( Idx<T>& other );
    //Idx( const Idx<T>& other );

    //! generic constructor with IdxSpec.
    Idx(Srg<T> *srg, IdxSpec &s);

    //! constructor without offset: appends to current storage.
    Idx(Srg<T> *srg, int n, intg *dims, intg *mods);

    //! generic constructor with dims and mods creates
    //! the storage and set offset to zero.
    Idx(int n, intg *dims, intg *mods);

    //! specific constructors for each number of dimensions

    //! creates an Idx0 with existing Srg and offset.
    Idx(Srg<T> *srg, intg o);
    //! creates an Idx0 from scratch;
    Idx();

  Idx( const Idx<T>& other )
    :storage(other.storage),
      spec(other.spec)
	{
	  storage->lock();
	}

    // create an Idx0 and fill it with val.
    // removed because ambiguous.
    // Idx(T val);

    //! creates an Idx1 of size size0, with existing Srg and offset.
    Idx(Srg<T> *srg, intg o, intg size0);
    //! creates an Idx1 of size size0.
    Idx(intg size0);

    //! creates an Idx2 of size (size0,size1),
    //! with existing Srg and offset.
    Idx(Srg<T> *srg, intg o, intg size0, intg size1);
    //! creates an Idx2 of size (size0,size1).
    Idx(intg size0, intg size1);

    //! creates an Idx3 of size (size0,size1,size2),
    //! with existing Srg and offset.
    Idx(Srg<T> *srg, intg o, intg size0, intg size1, intg size2);
    //! creates an Idx3 of size (size0,size1,size2).
    Idx(intg size0, intg size1, intg size2);

    //! creates an Idx of any order with existing Srg and offset.
    Idx(Srg<T> *srg, intg o, intg s0, intg s1, intg s2, intg s3, intg s4=-1, 
	intg s5=-1, intg s6=-1, intg s7=-1);
    //! creates an Idx of any order.
    Idx(intg s0, intg s1, intg s2, intg s3, intg s4=-1, intg s5=-1, intg s6=-1,
	intg s7=-1);

    //! change the offset of an Idx. The Storage is
    //! resized accordingly. Returns the new offset.
    virtual intg setoffset(intg o);

    //! resize an Idx. The order (ndim) is not allowed to change.
    //! This is to prevent nasty bugs.
    virtual void resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
			intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);

    //! same as resize, but the storage is enlarged by a step of s_chunk 
    //! if needed
    virtual void resize_chunk(intg s_chunk, intg s0=-1, intg s1=-1, intg s2=-1,
			      intg s3=-1, intg s4=-1, intg s5=-1, intg s6=-1, 
			      intg s7=-1);
    /**
     * Resizes the Idx using the dimension sizes listed
     * in a sequence.
     * @param sizesBegin: An iterator type, points to beginning of size list.
     * @param sizesEnd: An iterator type, points to one past the last size.
     */
    template<typename SizeIter>
      void resize( SizeIter& sizesBegin, SizeIter& sizesEnd ){
      const int ndims = std::distance( sizesBegin, sizesEnd );
      if ( ndims > MAXDIMS ){
	std::ostringstream oss;
	oss<<"Number of dimensions ("<<ndims<<") exceeds MAX_DIMS (";
	oss<<MAXDIMS<<")";
	ylerror(oss.str().c_str());
      }

      std::vector<T> sizes(ndims+1);
      std::copy(sizesBegin, sizesEnd, sizes.begin());
      sizes.back() = -1;
      spec.resize( sizesBegin, sizesEnd );
      growstorage();
    }

    //! select: return a new Idx corresponding to
    //! a slice of the current Idx with slice i
    //! in dimension d. In other words, if m is an
    //! Idx of order 2 of size (10,4), the call
    //! Idx p = m.select(0,3) will set p to
    //! an Idx or order 1 containing the 4-th
    //! row of m.
    Idx<T> select(int d, intg i);

    //! narrow: return a new Idx in which the d-th
    //! dimension has been reduced to size s, starting
    //! at item o. In other words, if m is an
    //! Idx of order 2 of size (10,4), the call
    //! Idx p = m.narrow(0,6,2) will set p to
    //! an Idx or order 2 of size (6,4) whose rows
    //! are rows 2 to 7 of m.
    Idx<T> narrow(int d, intg s, intg o);

    //! Return an new Idx in which dimensions
    //! d1 and d2 are transposed. No data is actually
    //! moved around, this merely manipulates the Idx
    //! structure itself.
    Idx<T> transpose(int d1, int d2);

    //! Return an new Idx in which the dimensions
    //! are permuted using the permutation vector p.
    //! For example, if m is an idx of size (2,4,6),
    //! int p[] = {1,2,0}; m.transpose(p);
    //! returns an Idx of size (4,6,2). No data is actually
    //! moved around, this merely manipulates the Idx
    //! structure itself.
    Idx<T> transpose(int *p);

    //! Return a new Idx prepared for a convolution.
    //! Returns an idx on the same storage as m (pointing to the
    //! same data) with an added dimension at the end obtained by
    //! "unfolding" the n -th dimension. The size of the new dimension
    //! is k. This essentially manipulates the mod array to make
    //! convolutions look like matrix-vector multiplies.
    virtual Idx<T> unfold(int d, intg k, intg s);

    // field access

    //! return pointer to storage
    virtual Srg<T> *getstorage() { return storage; }

    //! return size of Idx in d-th dimension.
    virtual intg dim(int d) { return spec.dim[d]; }

    //! return const ptr to dims
    virtual const intg* dims(){ return spec.dim; }

    //! return stride of Idx in d-th dimension.
    virtual intg mod(int d) { return spec.mod[d]; }

    //! return const ptr to mods
    virtual const intg* mods(){ return spec.mod; }

    //! return order of Idx (number of dimensions).
    virtual int order() { return spec.ndim; }

    //! return offset of Idx.
    virtual intg offset() { return spec.offset; }

    //! return total number of elements
    virtual intg nelements() { return spec.nelements(); }

    //! return total footprint in the storage
    //! (index after last cell occupied in the storage)
    virtual intg footprint() { return spec.footprint(); }

    //! return true if elements of Idx are
    //! contiguous in memory.
    virtual bool contiguousp() { return spec.contiguousp(); }

    //! return element if this is an Idx0,
    //! otherwise generate an error
    //  T &operator*() {
    //    if (spec.ndim==0) {
    //      return *(storage->data + spec.offset);
    //    } else { ylerror("Idx::operator*: only an Idx0 can be dereferenced"); }
    //  }

    //! return pointer on data chunk (on first element)
    virtual T *idx_ptr() {  return storage->data + spec.offset; }

    //! return a pointer to an element (Idx0 version)
    virtual T *ptr() { if (spec.ndim != 0) ylerror("not an Idx0"); 
      return storage->data + spec.offset; }

    //! return a pointer to an element (Idx1 version)
    virtual T *ptr(intg i0);
    //! return a pointer to an element (Idx2 version)
    virtual T *ptr(intg i0, intg i1);
    //! return a pointer to an element (Idx3 version)
    virtual T *ptr(intg i0, intg i1, intg i2);
    //! return a pointer to an element (generic version)
    virtual T *ptr(intg i0, intg i1, intg i2, intg i3, intg i4=-1, intg i5=-1, 
		   intg i6=-1, intg i7=-1);

    //! return the value of an element (Idx0 version)
    virtual T get();
    //! return the value of an element (Idx1 version)
    virtual T get(intg i0);
    //! return the value of an element (Idx2 version)
    virtual T get(intg i0, intg i1);
    //! return the value of an element (Idx3 version)
    virtual T get(intg i0, intg i1, intg i2);
    //! return the value of an element (generic version)
    virtual T get(intg i0, intg i1, intg i2, intg i3, intg i4=-1, intg i5=-1, 
		  intg i6=-1, intg i7=-1);

    //! sets the value of an element (Idx0 version)
    virtual T set(T val);
    //! sets the value of an element (Idx1 version)
    virtual T set(T val, intg i0);
    //! sets the value of an element (Idx2 version)
    virtual T set(T val, intg i0, intg i1);
    //! sets the value of an element (Idx3 version)
    virtual T set(T val, intg i0, intg i1, intg i2);
    //! sets the value of an element (generic version)
    virtual T set(T val, intg i0, intg i1, intg i2, intg i3, intg i4=-1, 
		  intg i5=-1, intg i6=-1, intg i7=-1);

    //! print content of Idx on stream
    virtual int fdump(FILE *f);

#if USING_STL_ITERS == 0
    // horrible syntax again. This time we have to
    // use U for type symbol otherwise the compiler
    // complains about T.

    template <class U> friend class IdxIter;
    template <class U> friend class IdxLooper;
#endif

  };

} // end namespace ebl

#include "IdxIterators.h"

namespace ebl {

#if USING_STL_ITERS == 0

  ////////////////////////////////////////////////////////////////
  // Idx Iterators are a subclass of Idx

  //! IdxLooper: a kind of iterator used by bloop
  //! and eloop macros. IdxLooper is a subclass of Idx,
  //! It is used as follows:
  //! for (IdxLooper z(&idx,0); z.notdone(); z.next()) { .... }
  template <class T> class IdxLooper : public Idx<T> {

  public:
    intg i;  // loop index
    intg dimd;  // number of elements to iterated upon
    intg modd; // stride in dimension being iterated upon

    //! generic constructor loops over dimensin ld
    IdxLooper(Idx<T> &idx, int ld);

    //! return true if loop is over
    bool notdone();

    //! increment to next item. Return pointer to data.
    T *next();

    void operator++();
  };

  ////////////////////////////////////////////////////////////////
  // Idx Iterators: gives you a pointer to the actual data,
  // unlike IdxLooper which gives you another Idx.

  //! IdxIter allows to iterate over all elements of an Idx.
  //! Although it can be used directly, it is easier to use
  //! it with the idx_aloopX macros. Example:
  //!  IdxIter<double> idx;
  //!  for ( idx.init(m); idx.notdone(); idx.next() ) {
  //!    printf("%g ",*idx);
  //!  }
  //! Here is an example that uses the aloop macro to fill up
  //! an Idx with numbers corresponding to the loop index:
  //! IdxIter<double> idx;
  //! idx_aloop_on(idx,m) { *idx = idx.i; }
  //! At any point during the loop, the indices of the element
  //! being worked on is stored in idx.d[k] for k=0
  //! to idx.order()-1.
  template <class T> class IdxIter {

  public:
    //! pointer to current item
    T *data;
    //! number of elements visited so far (loop index)
    intg i;
    //! total number of elements in Idx
    intg n;
    //! dimension being looped over
    int j;
    //! loop index array for non-contiguous Idx
    intg d[MAXDIMS];
    //! pointer to Idx being looped over.
    Idx<T> *iterand;

    //! empty constructor;
    IdxIter();

    //! Initialize an IdxIter to the start of
    //! the Idx passed as argument.
    T *init(Idx<T> &idx);

    //! Return true while the loop is not completed
    bool notdone();

    //! Increments IdxIter to next element
    T *next();

    //! dereferencing operator: returns data item.
    T& operator*() { return *data; }

  };

  ////////////////////////////////////////////////////////////////
#endif // if USING_STL_ITERS == 0

} // end namespace ebl

////////////////////////////////////////////////////////////////
#include "IdxIterators.hpp"
#include "Idx.hpp"

#endif
