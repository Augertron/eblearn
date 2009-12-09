/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef idx_H
#define idx_H

#define USING_STL_ITERS 0

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "defines.h"
#include "srg.h"

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
  //template<typename T> class ReverseDimIter;

  ////////////////////////////////////////////////////////////////

  class dummyt {  bool someunk; };
  class idxdim;

  //! idxspec contains all the characteristics of an idx,
  //! except the storage. It includes the order (number of dimensions)
  //! the offset, and the dim and mod arrays.
  //! having an idxspec class separate from idx allows us to write
  //! generic manipulation functions that do not depend on the
  //! type of the storage.
  class idxspec {

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
    //! dimension in which reside the "channels" of the data, e.g. (R,G,B) for
    //! an image. This is useful because it may be at different dimensions
    //! based on the context, usually 3rd dim for images, but 1st for networks.
    //! if equal to -1, no channels are present or have been initialized.
    //! this can be set using set_chandim() method.
    int chandim;

    //! resize the spec and return the new footprint.
    //! this is private because only idx can call this
    //! so that the storage gets properly resized.
    //! We do not allow the order to be changed with this,
    //! only the size of each dimension can be modified.
    //! The resized spec will be contiguous but will have
    //! the same offset as the original.
    intg resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
		intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);
    
    //! resize the spec and return the new footprint,
    //! using dimensions contained in an idxdim. 
    //! The order is not allowed to change.
    intg resize(const idxdim &d);

    //! resize dimension <dimn> with size <size> and return new footprint.
    //! only already allocated dimensions can be resized 
    //! (order is not allowed to change).
    intg resize1(intg dimn, intg size);

    template<typename SizeIter> intg resize( SizeIter& dimsBegin, 
					     SizeIter& dimsEnd ){
      const int nArgDims = std::distance(dimsBegin, dimsEnd);

      // Error-check the supplied number of dims.
      if( ndim == 0 ){
	eblerror("Cannot call resize on a 0-dimensional idxspec.");
      }
      else if( ndim != nArgDims ){
	std::ostringstream oss;
	oss<<"Number of supplied dimension sizes ("<<nArgDims;
	oss<<") doesn't match idxspec's number of dims ("<<ndim<<")";
	eblerror(oss.str().c_str());
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

    //! initialize a spec with offset o and 8 dimensions
    void init_spec(intg o, intg s0, intg s1, intg s2, intg s3, 
		   intg s4, intg s5, intg s6, intg s7);

  public:

    //! the destructor of idxspec deallocates the dim and mod arrays.
    virtual ~idxspec();

    //! assignment operator overloading.
    const idxspec& operator=(const idxspec& src);

    //! copy idxspec src into current idxspec
    void copy(const idxspec& src);

    //! copy constructor from idxspec src
    idxspec(const idxspec& src);

    ////////////////////////////////////////////////////////////////
    //! specific constructors for each number of dimensions

    //! This creates an idxspec0 with offset 0.
    //! This can be used to build an empty/blank idxspec.
    idxspec();

    //! Creates an idxspec0 with offset o.
    idxspec(intg o);

    //! Creates an idxspec1 with offset o.
    idxspec(intg o, intg size0);

    //! Creates an idxspec2 with offset o.
    idxspec(intg o, intg size0, intg size1);

    //! Creates an idxspec3 with offset o.
    idxspec(intg o, intg size0, intg size1, intg size2);

    //! Generic constructor with offset.
    idxspec(intg o, intg s0, intg s1, intg s2, intg s3,
	    intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);
    
    //! Creates an idxspec of order n from arrays of dims and mods.
    //! The arrays are copied.
    idxspec(intg o, int n, intg *ldim, intg *lmod);

    //! construct an idxspec from an array of dimensions contained in an idxdim,
    //! with offset o
    idxspec(intg o, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! access methods

    //! return the offset of idxspec
    intg getoffset() { return offset; }

    //! return the order (number of dimensions).
    int getndim() { return ndim; }

    //! return the memory footprint, including the offset.
    //! The storage of an idx containing this idxspec must
    //! be have at least this size.
    intg footprint();

    //! total number of elements accessed by idxspec
    intg nelements();

    //! returns true if the idxspec elements are
    //! in a continuous chunk of memory. This is useful
    //! to optimize iterators over the data.
    bool contiguousp();

    ////////////////////////////////////////////////////////////////
    //! print methods

    //! pretty-prints the idxspec on the specified file.
    void pretty(FILE *f);
    void pretty(std::ostream& out);

    ////////////////////////////////////////////////////////////////
    //! manipulation methods

    //! select: return a new idxspec corresponding to
    //! a slice of the current idxspec with slice i
    //! in dimension d. In other words, if m is an
    //! idxspec of order 2 of size (10,4), the call
    //! idxspec p = m.select(0,3) will set p to
    //! an idxspec or order 1 containing the 4-th
    //! row of m.
    idxspec select(int d, intg i);
    //! select_into: same as select, but modifies an existing
    //! idxspec instead of returning a new one.
    intg select_into(idxspec *dst, int d, intg n);
    //! select_inplace: same as select, but modifies the
    //! current idxspec.
    intg select_inplace(int d, intg i);

    //! narrow: return a new idxspec in which the d-th
    //! dimension has been reduced to size s, starting
    //! at item o. In other words, if m is an
    //! idxspec of order 2 of size (10,4), the call
    //! idxspec p = m.narrow(0,6,2) will set p to
    //! an idxspec or order 2 of size (6,4) whose rows
    //! are rows 2 to 7 of m.
    idxspec narrow(int d, intg s, intg o);
    //! narrow_into: same as narrow, but modifies an existing
    //! idxspec instead of returning a new one.
    intg narrow_into(idxspec *dst, int d, intg s, intg o);
    //! narrow_inplace: same as narrow, but modifies the
    //! current idxspec.
    intg narrow_inplace(int d, intg s, intg o);

    //! transpose: transpose dimensions d1 and d2.
    idxspec transpose(int d1, int d2);
    //! transpose all dimensions through permutation matrix p.
    idxspec transpose(int *p);
    //! same as transpose, but modifies an existing
    //! idxspec instead of returning a new one.
    int transpose_into(idxspec *dst, int d1, int d2);
    //! same as transpose, but modifies an existing
    //! idxspec instead of returning a new one.
    int transpose_into(idxspec *dst, int *p);
    //! transpose_inplace: same as transpose, but modifies the
    //! current idxspec.
    int transpose_inplace(int d1, int d2);
    //! transpose_inplace: same as transpose, but modifies the
    //! current idxspec.
    int transpose_inplace(int *p);

    //! unfold: prepare an idxspec for a convolution.
    //! Returns an idx on the same storage as m (pointing to the
    //! same data) with an added dimension at the end obtained by
    //! "unfolding" the n -th dimension. The size of the new dimension
    //! is k. This essentially manipulates the mod array to make
    //! convolutions look like matrix-vector multiplies.
    idxspec unfold(int d, intg k, intg s);
    //! same as unfold, but modifies an existing
    //! idxspec instead of returning a new one.
    intg unfold_into(idxspec *dst, int d, intg k, intg s);
    //! unfold_into: same as unfold, but modifies the
    //! current idxspec.
    intg unfold_inplace(int d, intg k, intg s);

    //! set dimension in which reside the "channels" of the data,
    //! e.g. (R,G,B) for
    //! an image. This is useful because it may be at different dimensions
    //! based on the context, usually 3rd dim for images, but 1st for networks.
    //! by default equal to -1, i.e. no channels are present or have been
    //! initialized.
    void set_chandim(int d);
    
    //! return dimension in which channels are (or -1 if not set).
    int get_chandim();

    ////////////////////////////////////////////////////////////////
    //! friends

    // horrible syntax to declare a template class friend.
    // isn't C++ wonderful?
    friend class idxdim;
    template <class T> friend class idxiter;
    template <class T> friend class idxlooper;
    template <class T> friend class idx;
    template <class T> friend class ScalarIter_Base;
    template <class T> friend class ScalarIter;
    template <class T> friend class ReverseScalarIter;
    template <class T> friend class DimIter_Base;
    template <class T> friend class DimIter;
    //template <class T> friend class ReverseDimIter;

    friend bool same_dim(idxspec &s1, idxspec &s2);
    //! idxspec print operator.
    friend std::ostream& operator<<(std::ostream& out, idxspec& d);
  };

  //! return true if two idxspec have the same dimensions,
  //! i.e. if all their dimensions are equal (regardless
  //! of strides).
  bool same_dim(idxspec &s1, idxspec &s2);

  ////////////////////////////////////////////////////////////////

  //! idx: main tensor class. This can represent vectors,
  //! matrices, and tensors up to 8 dimensions.
  //! An idx is merely an access structure that
  //! points to the data. Several idx can point to the same
  //! data.
  template <class T> class idx {
  private:

    //! pointer to the Srg structure that contains the data.
    Srg<T> *storage;

    //! a pointer to a dimensions descriptor.
    //! this is allocated when get_idxdim() method is called and destroyed by
    //! destructor.
    idxdim *pidxdim;

    //! increase size of storage to fit the current dimension
    void growstorage();

    //! increase size of storage to fit the current dimension + a given size
    void growstorage_chunk(intg s_chunk);

    //! Implementation of public printElems() method.
    void printElems_impl( int indent, std::ostream& );

  protected:
    //! fake constructor that does nothing.
    //! This is called by the idxlooper constructor
    idx(dummyt *dummy);

  public:

    //! idxspec that contains the order,
    //! offset, dimensions and strides.
    idxspec spec;

    //! destructor: unlocks the Srg.
    virtual ~idx();

    // TODO: Find out why code such as idx<float> = 1 compiles
    // (even without default operator below).
    // default operator below outputs an error that this is forbidden.
    virtual idx<T>& operator=(T other){
      eblerror("Forbidden idx assignment: it can only be assigned another idx");
      return *this;
    }

    virtual idx<T>& operator=(const idx<T>& other) {
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

    virtual idx<T> operator[](const intg i) {
      return this->select(0,i);
    }

    //! Copy constructor. Prevents implcit calls via '='.
    // Keep this 'explicit' until we decide to implement operator=.
    //idx( idx<T>& other );
    //idx( const idx<T>& other );

    //! generic constructor with dims and mods creates
    //! the storage and set offset to zero.
    idx(int n, intg *dims, intg *mods);
        
  idx(const idx<T>& other)
    : storage(other.storage), pidxdim(NULL), spec(other.spec) {
      storage->lock();
    }

    ////////////////////////////////////////////////////////////////
    //! constructors initialized with an array

    //! creates an idx2 of size (s0, s1) and fills it with mat, expected
    //! to be of size (s0, s1).
    idx(const T *mat, intg s0, intg s1);

    ////////////////////////////////////////////////////////////////
    //! specific constructors for each number of dimensions

    //! creates an idx0 from scratch;
    idx();
    //! creates an idx1 of size size0.
    idx(intg size0);
    //! creates an idx2 of size (size0,size1).
    idx(intg size0, intg size1);
    //! creates an idx3 of size (size0,size1,size2).
    idx(intg size0, intg size1, intg size2);
    //! creates an idx of any order.
    idx(intg s0, intg s1, intg s2, intg s3, intg s4=-1, intg s5=-1, intg s6=-1,
	intg s7=-1);
    //! creates an idx from an array of dimensions contained in an idxdim
    idx(const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from existing Srg and offset

    //! generic constructor with idxspec.
    idx(Srg<T> *srg, idxspec &s);
    //! constructor with existing storage and array pointers for dim and mod
    idx(Srg<T> *srg, intg o, intg n, intg *dims, intg *mods);
    //! creates an idx0 with existing Srg and offset.
    idx(Srg<T> *srg, intg o);
    //! creates an idx1 of size size0, with existing Srg and offset.
    idx(Srg<T> *srg, intg o, intg size0);
    //! creates an idx2 of size (size0,size1),
    //! with existing Srg and offset.
    idx(Srg<T> *srg, intg o, intg size0, intg size1);
    //! creates an idx3 of size (size0,size1,size2),
    //! with existing Srg and offset.
    idx(Srg<T> *srg, intg o, intg size0, intg size1, intg size2);
    //! creates an idx of any order with existing Srg and offset.
    idx(Srg<T> *srg, intg o, intg s0, intg s1, intg s2, intg s3, intg s4=-1, 
	intg s5=-1, intg s6=-1, intg s7=-1);
    idx(Srg<T> *srg, intg o, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! STL-like iterators 

    //! STL-like typedefs
    typedef T value_type;
    typedef ScalarIter<value_type> scalar_iterator;
    typedef ReverseScalarIter<value_type> reverse_scalar_iterator;
    typedef DimIter<value_type> dimension_iterator;
    //    typedef ReverseDimIter<value_type> reverse_dimension_iterator;

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
    //reverse_dimension_iterator dim_rbegin( int dim );

    //! Returns a terminated iterator for the given dimension.
    //    reverse_dimension_iterator dim_rend( int dim );

    ////////////////////////////////////////////////////////////////
    //! resize methods

    //! change the offset of an idx. The Storage is
    //! resized accordingly. Returns the new offset.
    virtual intg setoffset(intg o);

    //! resize an idx. The order (ndim) is not allowed to change.
    //! This is to prevent nasty bugs.
    virtual void resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
			intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);

    //! resize an idx with dimensions contained in an idxdim. 
    //! The order is not allowed to change.
    virtual void resize(const idxdim &d);

    //! resize 1 dimension of an idx. The order is not allowed to change.
    virtual void resize1(intg dimn, intg size);

    //! same as resize, but the storage is enlarged by a step of s_chunk 
    //! if needed
    virtual void resize_chunk(intg s_chunk, intg s0=-1, intg s1=-1, intg s2=-1,
			      intg s3=-1, intg s4=-1, intg s5=-1, intg s6=-1, 
			      intg s7=-1);


    /**
     * Resizes the idx using the dimension sizes listed
     * in a sequence.
     * @param sizesBegin: An iterator type, points to beginning of size list.
     * @param sizesEnd: An iterator type, points to one past the last size.
     */
    template<typename SizeIter>
      void resize( SizeIter& sizesBegin, SizeIter& sizesEnd) {
      const int ndims = std::distance( sizesBegin, sizesEnd );
      if ( ndims > MAXDIMS ){
	std::ostringstream oss;
	oss<<"Number of dimensions ("<<ndims<<") exceeds MAX_DIMS (";
	oss<<MAXDIMS<<")";
	eblerror(oss.str().c_str());
      }

      std::vector<T> sizes(ndims+1);
      std::copy(sizesBegin, sizesEnd, sizes.begin());
      sizes.back() = -1;
      spec.resize( sizesBegin, sizesEnd );
      growstorage();
    }

    ////////////////////////////////////////////////////////////////
    //! idx manipulation methods

    //! select: return a new idx corresponding to
    //! a slice of the current idx with slice i
    //! in dimension d. In other words, if m is an
    //! idx of order 2 of size (10,4), the call
    //! idx p = m.select(0,3) will set p to
    //! an idx or order 1 containing the 4-th
    //! row of m.
    idx<T> select(int d, intg i);

    //! narrow: return a new idx in which the d-th
    //! dimension has been reduced to size s, starting
    //! at item o. In other words, if m is an
    //! idx of order 2 of size (10,4), the call
    //! idx p = m.narrow(0,6,2) will set p to
    //! an idx or order 2 of size (6,4) whose rows
    //! are rows 2 to 7 of m.
    idx<T> narrow(int d, intg s, intg o);

    //! Return an new idx in which dimensions
    //! d1 and d2 are transposed. No data is actually
    //! moved around, this merely manipulates the idx
    //! structure itself.
    idx<T> transpose(int d1, int d2);

    //! Return an new idx in which the dimensions
    //! are permuted using the permutation vector p.
    //! For example, if m is an idx of size (2,4,6),
    //! int p[] = {1,2,0}; m.transpose(p);
    //! returns an idx of size (4,6,2). No data is actually
    //! moved around, this merely manipulates the idx
    //! structure itself.
    idx<T> transpose(int *p);

    //! Return a new idx prepared for a convolution.
    //! Returns an idx on the same storage as m (pointing to the
    //! same data) with an added dimension at the end obtained by
    //! "unfolding" the n -th dimension. The size of the new dimension
    //! is k. This essentially manipulates the mod array to make
    //! convolutions look like matrix-vector multiplies.
    virtual idx<T> unfold(int d, intg k, intg s);

    //! Returns a new idx with an order n.
    //! if n == 1, the data is viewed as a 1D idx, regardless of its
    //!   current order.
    //! if n > 1 and n > current order, then extra dimensions of size 1
    //!   are added.
    //! if n > 1 and n < current order, this is undefined, an error is raised.
    //! if n == current order, an identical idx is returned.
    // TODO: forbid this function as it conflicts with the forbidden
    // order-change law? unfold also conflicts with this law?
    idx<T> view_as_order(int n);

    //! set dimension in which reside the "channels" of the data,
    //! e.g. (R,G,B) for
    //! an image. This is useful because it may be at different dimensions
    //! based on the context, usually 3rd dim for images, but 1st for networks.
    //! by default equal to -1, i.e. no channels are present or have been
    //! initialized.
    //! Warning: this is hasn't the effect of shifting an existing channel
    //! dimension into a new place, for that purpose use shift_chan();
    void set_chandim(int d);

    //! return dimension in which channels are (or -1 if not set).
    int get_chandim();

    //! Return an new idx in which dimension d is shifted to position pos,
    //! keeping the ordering of other dimensions.
    //! For example, if m is an idx of size (2,4,6),
    //! m.shift_dim(2, 0); returns an idx of size (6,2,4). No data is actually
    //! moved around, this merely manipulates the idx structure itself.
    idx<T> shift_dim(int d, int pos);

    //! Return an new idx in which channels dimension (if defined) is shifted
    //! to position pos, keeping the ordering of other dimensions.
    //! For example, if m is an idx of size (2,4,6) with chandim 2,
    //! m.shift_chan(0); returns an idx of size (6,2,4). No data is actually
    //! moved around, this merely manipulates the idx structure itself.
    //! Warning: current idx won't be modified, only the returned idx will be.
    idx<T> shift_chan(int pos);
    
    ////////////////////////////////////////////////////////////////
    //! field access methods

    //! return pointer to storage
    virtual Srg<T> *getstorage() { return storage; }

    //! return size of idx in d-th dimension.
    virtual intg dim(int d) { return spec.dim[d]; }

    //! return const ptr to dims
    virtual const intg* dims(){ return spec.dim; }

    //! return stride of idx in d-th dimension.
    virtual intg mod(int d) { return spec.mod[d]; }

    //! return const ptr to mods
    virtual const intg* mods(){ return spec.mod; }

    //! return order of idx (number of dimensions).
    virtual int order() { return spec.ndim; }

    //! return offset of idx.
    virtual intg offset() { return spec.offset; }

    //! return total number of elements
    virtual intg nelements() { return spec.nelements(); }

    //! return total footprint in the storage
    //! (index after last cell occupied in the storage)
    virtual intg footprint() { return spec.footprint(); }

    //! return true if elements of idx are
    //! contiguous in memory.
    virtual bool contiguousp() { return spec.contiguousp(); }

    //! return element if this is an idx0,
    //! otherwise generate an error
    //  T &operator*() {
    //    if (spec.ndim==0) {
    //      return *(storage->data + spec.offset);
    //    } else { eblerror("idx::operator*: only an idx0 can be dereferenced");}
    //  }
 
    //! return true if this idx has same order and dimensions as idxdim d.
    //! i.e. if all their dimensions are equal (regardless of strides).
    virtual bool same_dim(const idxdim &d);

    //! return true if this idx has same order and dimensions s0 .. s7
    //! i.e. if all their dimensions are equal (regardless of strides).
    virtual bool same_dim(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
			  intg s6, intg s7);

    //! returns a reference to an idxdim object (owned by this idx)
    //! containing the order and dimensions of this idx.
    virtual idxdim& get_idxdim();

   ////////////////////////////////////////////////////////////////
    //! data access methods

    //! return pointer on data chunk (on first element)
    virtual T *idx_ptr() {  return storage->data + spec.offset; }

    //! return a pointer to an element (idx0 version)
    virtual T *ptr() { if (spec.ndim != 0) eblerror("not an idx0"); 
      return storage->data + spec.offset; }

    //! return a pointer to an element (idx1 version)
    virtual T *ptr(intg i0);
    //! return a pointer to an element (idx2 version)
    virtual T *ptr(intg i0, intg i1);
    //! return a pointer to an element (idx3 version)
    virtual T *ptr(intg i0, intg i1, intg i2);
    //! return a pointer to an element (generic version)
    virtual T *ptr(intg i0, intg i1, intg i2, intg i3, intg i4=-1, intg i5=-1, 
		   intg i6=-1, intg i7=-1);

    //! return the value of an element (idx0 version)
    virtual T get();
    //! return the value of an element (idx1 version)
    virtual T get(intg i0);
    //! return the value of an element (idx2 version)
    virtual T get(intg i0, intg i1);
    //! return the value of an element (idx3 version)
    virtual T get(intg i0, intg i1, intg i2);
    //! return the value of an element (generic version)
    virtual T get(intg i0, intg i1, intg i2, intg i3, intg i4=-1, intg i5=-1, 
		  intg i6=-1, intg i7=-1);

    //! sets the value of an element (idx0 version)
    virtual T set(T val);
    //! sets the value of an element (idx1 version)
    virtual T set(T val, intg i0);
    //! sets the value of an element (idx2 version)
    virtual T set(T val, intg i0, intg i1);
    //! sets the value of an element (idx3 version)
    virtual T set(T val, intg i0, intg i1, intg i2);
    //! sets the value of an element (generic version)
    virtual T set(T val, intg i0, intg i1, intg i2, intg i3, intg i4=-1, 
		  intg i5=-1, intg i6=-1, intg i7=-1);

    ////////////////////////////////////////////////////////////////
    //! print methods

    //! Pretty-prints elements to a stream.
    //friend std::ostream& operator<<( std::ostream& out, idx<T>& tensor );

    //! Pretty-prints IDx metadata to a file pointer.
    virtual void pretty(FILE *);
    virtual void pretty(std::ostream& out);

    //! Pretty-prints elements to a stream.
    virtual void printElems(); // calls printElems( std::cout );
    virtual void printElems( std::ostream& out );
    // void printElems( FILE* out );  doesn't work (cf implementation)

    //! print content of idx on stream
    virtual int fdump(FILE *f);

#if USING_STL_ITERS == 0
    // horrible syntax again. This time we have to
    // use U for type symbol otherwise the compiler
    // complains about T.

    template <class U> friend class idxiter;
    template <class U> friend class idxlooper;
#endif

  };

  //! idx print operator.
  template <class T> 
    std::ostream& operator<<(std::ostream& out, idx<T>& m);  

} // end namespace ebl

#include "idx_iterators.h"

namespace ebl {

#if USING_STL_ITERS == 0

  ////////////////////////////////////////////////////////////////
  // idx Iterators are a subclass of idx

  //! idxlooper: a kind of iterator used by bloop
  //! and eloop macros. idxlooper is a subclass of idx,
  //! It is used as follows:
  //! for (idxlooper z(&idx,0); z.notdone(); z.next()) { .... }
  template <class T> class idxlooper : public idx<T> {

  public:
    intg i;  // loop index
    intg dimd;  // number of elements to iterated upon
    intg modd; // stride in dimension being iterated upon

    //! generic constructor loops over dimensin ld
    idxlooper(idx<T> &m, int ld);

    //! return true if loop is over
    bool notdone();

    //! increment to next item. Return pointer to data.
    T *next();

    void operator++();
  };

  ////////////////////////////////////////////////////////////////
  // idx Iterators: gives you a pointer to the actual data,
  // unlike idxlooper which gives you another idx.

  //! idxiter allows to iterate over all elements of an idx.
  //! Although it can be used directly, it is easier to use
  //! it with the idx_aloopX macros. Example:
  //!  idxiter<double> idx;
  //!  for ( idx.init(m); idx.notdone(); idx.next() ) {
  //!    printf("%g ",*idx);
  //!  }
  //! Here is an example that uses the aloop macro to fill up
  //! an idx with numbers corresponding to the loop index:
  //! idxiter<double> idx;
  //! idx_aloop_on(idx,m) { *idx = idx.i; }
  //! At any point during the loop, the indices of the element
  //! being worked on is stored in idx.d[k] for k=0
  //! to idx.order()-1.
  template <class T> class idxiter {

  public:
    //! pointer to current item
    T *data;
    //! number of elements visited so far (loop index)
    intg i;
    //! total number of elements in idx
    intg n;
    //! dimension being looped over
    int j;
    //! loop index array for non-contiguous idx
    intg d[MAXDIMS];
    //! pointer to idx being looped over.
    idx<T> *iterand;

    //! empty constructor;
    idxiter();

    //! Initialize an idxiter to the start of
    //! the idx passed as argument.
    T *init(idx<T> &m);

    //! Return true while the loop is not completed
    bool notdone();

    //! Increments idxiter to next element
    T *next();

    //! dereferencing operator: returns data item.
    T& operator*() { return *data; }

  };

  ////////////////////////////////////////////////////////////////
#endif // if USING_STL_ITERS == 0

  //! This class allows to extract dimensions information from existing idx
  //! objects in order to create other idx objects with the same order without
  //! knowning their order in advance. It does not allow to change the order
  //! but one can modify the size of a particular dimension via the setdim 
  //! method.
  class idxdim {
  private:
    intg	dims[MAXDIMS];	// size of each dimensions
    intg	ndim;		// order
    int         chandim;        // dimension with channels (-1 by default)
    
  public:

    ////////////////////////////////////////////////////////////////
    // constructors
    
    //! Empty constructor, creates an empty idxdim.
    idxdim();
    
    //! Create an idxdim based on the information found in an idxspec.
    idxdim(const idxspec &s);
    
    //! Create an idxdim based on the information found in an idx<T>.
    template <class T> idxdim(const idx<T> &i);
    
    //! Create an idxdim based on the information found in an idxdim.
    idxdim(const idxdim &s);
    
    //! Generic constructor.
    idxdim(intg s0, intg s1=-1, intg s2=-1, intg s3=-1,
	   intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);

    //! Destructor.
    virtual ~idxdim();

    ////////////////////////////////////////////////////////////////
    // set dimensions
    
    //! Change the dimensions dimn to size size.
    void setdim(intg dimn, intg size);

    //! Extract dimensions information from an idx
    template <class T>
      void setdims(const idx<T> &i);
    
    //! Extract dimensions information from an idxdim
    void setdims(const idxdim &s);
     
    //! Extract dimensions information from an idxspec
    void setdims(const idxspec &s);

    //! Insert a dimension of size dim_size at position pos.
    bool insert_dim(intg dim_size, uint pos);    
    
    //! set dimension in which reside the "channels" of the data,
    //! e.g. (R,G,B) for
    //! an image. This is useful because it may be at different dimensions
    //! based on the context, usually 3rd dim for images, but 1st for networks.
    //! by default equal to -1, i.e. no channels are present or have been
    //! initialized.
    void set_chandim(int d);
    
    //! return dimension in which channels are (or -1 if not set).
    int get_chandim();

    ////////////////////////////////////////////////////////////////
    // get dimensions

    //! Returns the order.
    intg order() const;
    
    //! Returns the size of dimensions dimn.
    intg dim(intg dimn) const;

    //! Return true if dimensions and order are equal.
    bool operator==(const idxdim& other);

    ////////////////////////////////////////////////////////////////
    
    // friends
    friend class idxspec;
  };

  //! idxdim print operator.
  std::ostream& operator<<(std::ostream& out, const idxdim& d);

  // TODO: move into image.h when idx specializes into image
  // TODO: templatize
  class rect {
  public:
  rect(uint h0_, uint w0_, uint height_, uint width_)
    : h0(h0_), w0(w0_), height(height_), width(width_) {}
    rect() {}
    //  rect(const rect &r) : h0(r.h0), w0(r.w0), height(r.height), width(r.width) {}
    virtual ~rect() {}
    bool overlap(const rect &r);
    uint h0, w0, height, width;

    // friends
    friend std::ostream& operator<<(std::ostream& out, const rect& r);
  };

} // end namespace ebl

////////////////////////////////////////////////////////////////
#include "idx_iterators.hpp"
#include "idx.hpp"

#endif
