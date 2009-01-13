#include "IdxTest.h"

using namespace std;
using namespace ebl;

void IdxTest::setUp() {
}

void IdxTest::tearDown() {
}

// Testing Idx get
void IdxTest::test_Idx_get() {
  Idx<double> t0;
  Idx<double> t1(2);
  Idx<double> t2(2,2);
  Idx<double> t3(2,2,2);
  Idx<double> t4(2,2,2,2);
  Idx<double> t5(2,2,2,2,2);
  Idx<double> t6(2,2,2,2,2,2);
  Idx<double> t7(2,2,2,2,2,2,2);
  Idx<double> t8(2,2,2,2,2,2,2,2);
  t0.set(1);
  t1.set(1, 0);
  t2.set(1, 0, 1);
  t3.set(1, 0, 1, 0);
  t4.set(1, 0, 1, 0, 1);
  t5.set(1, 0, 1, 0, 1, 0);
  t6.set(1, 0, 1, 0, 1, 0, 1);
  t7.set(1, 0, 1, 0, 1, 0, 1, 0);
  t8.set(1, 0, 1, 0, 1, 0, 1, 0, 1);

  CPPUNIT_ASSERT_EQUAL(1.0, t0.get());
  CPPUNIT_ASSERT_EQUAL(1.0, t1.get(0));
  CPPUNIT_ASSERT_EQUAL(1.0, t2.get(0, 1));
  CPPUNIT_ASSERT_EQUAL(1.0, t3.get(0, 1, 0));
  CPPUNIT_ASSERT_EQUAL(1.0, t4.get(0, 1, 0, 1));
  CPPUNIT_ASSERT_EQUAL(1.0, t5.get(0, 1, 0, 1, 0));
  CPPUNIT_ASSERT_EQUAL(1.0, t6.get(0, 1, 0, 1, 0, 1));
  CPPUNIT_ASSERT_EQUAL(1.0, t7.get(0, 1, 0, 1, 0, 1, 0));
  CPPUNIT_ASSERT_EQUAL(1.0, t8.get(0, 1, 0, 1, 0, 1, 0, 1));
}

// Testing Srg set, changsize, growsize
void IdxTest::test_Srg() {
  Srg<double> *s = new Srg<double>(10);
  CPPUNIT_ASSERT_EQUAL(10, (int) s->size());
  s->set(3, 42);
  CPPUNIT_ASSERT_EQUAL(42.0, (double)s->get(3));
  s->changesize(8);
  CPPUNIT_ASSERT_EQUAL(8, (int) s->size());
  s->growsize(20);
  CPPUNIT_ASSERT_EQUAL(20, (int) s->size());
  s->growsize(10);
  CPPUNIT_ASSERT_EQUAL(20, (int) s->size());
  CPPUNIT_ASSERT_EQUAL(42.0, (double)s->get(3));
  s->changesize(0);
  CPPUNIT_ASSERT_EQUAL(0, (int) s->size());
  s->changesize(30);
  CPPUNIT_ASSERT_EQUAL(30, (int) s->size());
}

// Testing IdxSpec constructors, select, transpose, unfold
void IdxTest::test_IdxSpec() {
  IdxSpec *sp = new IdxSpec(5, 4, 3);
  CPPUNIT_ASSERT_EQUAL(2, (int) sp->getndim());
  CPPUNIT_ASSERT_EQUAL(5, (int) sp->getoffset());
  CPPUNIT_ASSERT_EQUAL(4, (int) sp->select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(3, (int) sp->select(0, 0).nelements());
  delete sp;

  IdxSpec spec1(0, 5, 6);
  CPPUNIT_ASSERT_EQUAL(2, (int) spec1.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) spec1.getoffset());
  CPPUNIT_ASSERT_EQUAL(5, (int) spec1.select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(6, (int) spec1.select(0, 0).nelements());

  IdxSpec spec2(0, 10);
  CPPUNIT_ASSERT_EQUAL(1, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(10, (int) spec2.nelements());

  spec1.select_into(&spec2, 0, 2);
  CPPUNIT_ASSERT_EQUAL(1, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(12, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(6, (int) spec2.nelements());

  spec1.select_into(&spec2, 1, 4);
  CPPUNIT_ASSERT_EQUAL(1, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(4, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(5, (int) spec2.nelements());

  spec1.transpose_into(&spec2, 0, 1);
  CPPUNIT_ASSERT_EQUAL(2, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(6, (int) spec2.select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(5, (int) spec2.select(0, 0).nelements());

  spec1.unfold_into(&spec2, 1, 3, 1);
  CPPUNIT_ASSERT_EQUAL(3, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(5, (int) spec2.select(1, 0).select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(4, (int) spec2.select(0, 0).select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(3, (int) spec2.select(0, 0).select(0, 0).nelements());

  int p[2] = { 1, 0 };
  spec1.transpose_into(&spec2, p);
  CPPUNIT_ASSERT_EQUAL(2, (int) spec2.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) spec2.getoffset());
  CPPUNIT_ASSERT_EQUAL(6, (int) spec2.select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(5, (int) spec2.select(0, 0).nelements());

  IdxSpec specu(0, 2, 3, 2, 4, 5, 1);
  CPPUNIT_ASSERT_EQUAL(6, (int) specu.getndim());
  CPPUNIT_ASSERT_EQUAL(0, (int) specu.getoffset());
  CPPUNIT_ASSERT_EQUAL(240, (int) specu.nelements());
}

// Testing Idx constructors, select, narrow, transpose, unfold
void IdxTest::test_Idx_operations() {
  Idx<double> m0;
  CPPUNIT_ASSERT_EQUAL(0, (int) m0.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m0.offset());
  CPPUNIT_ASSERT_EQUAL(true, m0.contiguousp());

  Idx<int> m1((intg)5);
  CPPUNIT_ASSERT_EQUAL(1, (int) m1.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m1.offset());
  CPPUNIT_ASSERT_EQUAL(true, m1.contiguousp());
  CPPUNIT_ASSERT_EQUAL(5, (int) m1.dim(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) m1.mod(0));

  Idx<float> m2(3, 4);
  CPPUNIT_ASSERT_EQUAL(2, (int) m2.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m2.offset());
  CPPUNIT_ASSERT_EQUAL(true, m2.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) m2.dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) m2.dim(1));
  CPPUNIT_ASSERT_EQUAL(4, (int) m2.mod(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) m2.mod(1));

  Idx<float> m3(2, 4, 3);
  CPPUNIT_ASSERT_EQUAL(3, (int) m3.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m3.offset());
  CPPUNIT_ASSERT_EQUAL(true, m3.contiguousp());
  CPPUNIT_ASSERT_EQUAL(2, (int) m3.dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) m3.dim(1));
  CPPUNIT_ASSERT_EQUAL(3, (int) m3.dim(2));
  CPPUNIT_ASSERT_EQUAL(12, (int) m3.mod(0));
  CPPUNIT_ASSERT_EQUAL(3, (int) m3.mod(1));
  CPPUNIT_ASSERT_EQUAL(1, (int) m3.mod(2));

  Idx<float> m6(2, 4, 3, 2, 5, 4);
  CPPUNIT_ASSERT_EQUAL(6, (int) m6.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m6.offset());
  CPPUNIT_ASSERT_EQUAL(true, m6.contiguousp());
  CPPUNIT_ASSERT_EQUAL(2, (int) m6.dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) m6.dim(1));
  CPPUNIT_ASSERT_EQUAL(3, (int) m6.dim(2));
  CPPUNIT_ASSERT_EQUAL(2, (int) m6.dim(3));
  CPPUNIT_ASSERT_EQUAL(5, (int) m6.dim(4));
  CPPUNIT_ASSERT_EQUAL(4, (int) m6.dim(5));
  CPPUNIT_ASSERT_EQUAL(480, (int) m6.mod(0));
  CPPUNIT_ASSERT_EQUAL(120, (int) m6.mod(1));
  CPPUNIT_ASSERT_EQUAL(40, (int) m6.mod(2));
  CPPUNIT_ASSERT_EQUAL(20, (int) m6.mod(3));
  CPPUNIT_ASSERT_EQUAL(4, (int) m6.mod(4));
  CPPUNIT_ASSERT_EQUAL(1, (int) m6.mod(5));

  Idx<double> *nm3 = new Idx<double>(2,4,3);
  CPPUNIT_ASSERT_EQUAL(3, (int) nm3->order());
  CPPUNIT_ASSERT_EQUAL(0, (int) nm3->offset());
  CPPUNIT_ASSERT_EQUAL(true, nm3->contiguousp());
  CPPUNIT_ASSERT_EQUAL(2, (int) nm3->dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) nm3->dim(1));
  CPPUNIT_ASSERT_EQUAL(3, (int) nm3->dim(2));
  CPPUNIT_ASSERT_EQUAL(12, (int) nm3->mod(0));
  CPPUNIT_ASSERT_EQUAL(3, (int) nm3->mod(1));
  CPPUNIT_ASSERT_EQUAL(1, (int) nm3->mod(2));
  delete nm3;

  Idx<double> ma(3, 10, 20); 
  CPPUNIT_ASSERT_EQUAL(3, (int) ma.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) ma.offset());
  CPPUNIT_ASSERT_EQUAL(true, ma.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) ma.dim(0));
  CPPUNIT_ASSERT_EQUAL(10, (int) ma.dim(1));
  CPPUNIT_ASSERT_EQUAL(20, (int) ma.dim(2));
  CPPUNIT_ASSERT_EQUAL(200, (int) ma.mod(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) ma.mod(1));
  CPPUNIT_ASSERT_EQUAL(1, (int) ma.mod(2));

  Idx<double> ms = ma.select(0, 1);
  CPPUNIT_ASSERT_EQUAL(2, (int) ms.order());
  CPPUNIT_ASSERT_EQUAL(200, (int) ms.offset());
  CPPUNIT_ASSERT_EQUAL(true, ms.contiguousp());
  CPPUNIT_ASSERT_EQUAL(10, (int) ms.dim(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) ms.dim(1));
  CPPUNIT_ASSERT_EQUAL(20, (int) ms.mod(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) ms.mod(1));

  Idx<double> mn = ma.narrow(2, 8, 4);
  CPPUNIT_ASSERT_EQUAL(3, (int) mn.order());
  CPPUNIT_ASSERT_EQUAL(4, (int) mn.offset());
  CPPUNIT_ASSERT_EQUAL(false, mn.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) mn.dim(0));
  CPPUNIT_ASSERT_EQUAL(10, (int) mn.dim(1));
  CPPUNIT_ASSERT_EQUAL(8, (int) mn.dim(2));
  CPPUNIT_ASSERT_EQUAL(200, (int) mn.mod(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) mn.mod(1));
  CPPUNIT_ASSERT_EQUAL(1, (int) mn.mod(2));

  Idx<double> mt = ma.transpose(1, 2);
  CPPUNIT_ASSERT_EQUAL(3, (int) mt.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) mt.offset());
  CPPUNIT_ASSERT_EQUAL(false, mt.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) mt.dim(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) mt.dim(1));
  CPPUNIT_ASSERT_EQUAL(10, (int) mt.dim(2));
  CPPUNIT_ASSERT_EQUAL(200, (int) mt.mod(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) mt.mod(1));
  CPPUNIT_ASSERT_EQUAL(20, (int) mt.mod(2));

  Idx<double> mu = ma.unfold(2, 6, 2);
  CPPUNIT_ASSERT_EQUAL(4, (int) mu.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) mu.offset());
  CPPUNIT_ASSERT_EQUAL(false, mu.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) mu.dim(0));
  CPPUNIT_ASSERT_EQUAL(10, (int) mu.dim(1));
  CPPUNIT_ASSERT_EQUAL(8, (int) mu.dim(2));
  CPPUNIT_ASSERT_EQUAL(6, (int) mu.dim(3));
  CPPUNIT_ASSERT_EQUAL(200, (int) mu.mod(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) mu.mod(1));
  CPPUNIT_ASSERT_EQUAL(2, (int) mu.mod(2));
  CPPUNIT_ASSERT_EQUAL(1, (int) mu.mod(3));
}

  // testing Idx constructor from Srg, Idx resize
void IdxTest::test_Idx_resize() {
  Srg<float> *srgptr = new Srg<float>(0);
  Idx<float> m1(srgptr, srgptr->size(), 3, 4);

  CPPUNIT_ASSERT_EQUAL(m1.getstorage(), srgptr);
  CPPUNIT_ASSERT_EQUAL((int) m1.footprint(), (int) srgptr->size());
  CPPUNIT_ASSERT_EQUAL(m1.idx_ptr(), srgptr->data);
  CPPUNIT_ASSERT_EQUAL(2, (int) m1.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m1.offset());
  CPPUNIT_ASSERT_EQUAL(3, (int) m1.select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(4, (int) m1.select(0, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(true, m1.contiguousp());

  Idx<float> m2(srgptr, srgptr->size(), 2, 5);

  CPPUNIT_ASSERT_EQUAL(m2.getstorage(), srgptr);
  CPPUNIT_ASSERT_EQUAL((int) m2.footprint(), (int) srgptr->size());
  CPPUNIT_ASSERT_EQUAL(2, (int) m2.order());
  CPPUNIT_ASSERT_EQUAL(12, (int) m2.offset());
  CPPUNIT_ASSERT_EQUAL(2, (int) m2.select(1, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(5, (int) m2.select(0, 0).nelements());
  CPPUNIT_ASSERT_EQUAL(true, m2.contiguousp());

  Idx<float> m3(srgptr, srgptr->size(), 6);

  CPPUNIT_ASSERT_EQUAL(m3.getstorage(), srgptr);
  CPPUNIT_ASSERT_EQUAL((int) m3.footprint(), (int) srgptr->size());
  CPPUNIT_ASSERT_EQUAL(1, (int) m3.order());
  CPPUNIT_ASSERT_EQUAL(22, (int) m3.offset());
  CPPUNIT_ASSERT_EQUAL(6, (int) m3.nelements());
  CPPUNIT_ASSERT_EQUAL(true, m3.contiguousp());

  Idx<double> m(8, 3, 4, 5);

  CPPUNIT_ASSERT_EQUAL(4, (int) m.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m.offset());
  CPPUNIT_ASSERT_EQUAL(true, m.contiguousp());
  CPPUNIT_ASSERT_EQUAL(8, (int) m.dim(0));
  CPPUNIT_ASSERT_EQUAL(3, (int) m.dim(1));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.dim(2));
  CPPUNIT_ASSERT_EQUAL(5, (int) m.dim(3));
  CPPUNIT_ASSERT_EQUAL(60, (int) m.mod(0));
  CPPUNIT_ASSERT_EQUAL(20, (int) m.mod(1));
  CPPUNIT_ASSERT_EQUAL(5, (int) m.mod(2));
  CPPUNIT_ASSERT_EQUAL(1, (int) m.mod(3));
  CPPUNIT_ASSERT_EQUAL(480, (int) m.footprint());
  CPPUNIT_ASSERT_EQUAL(480, (int) m.getstorage()->size());
  
  m.resize(9, 4, 5, 6);

  CPPUNIT_ASSERT_EQUAL(4, (int) m.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m.offset());
  CPPUNIT_ASSERT_EQUAL(true, m.contiguousp());
  CPPUNIT_ASSERT_EQUAL(9, (int) m.dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.dim(1));
  CPPUNIT_ASSERT_EQUAL(5, (int) m.dim(2));
  CPPUNIT_ASSERT_EQUAL(6, (int) m.dim(3));
  CPPUNIT_ASSERT_EQUAL(120, (int) m.mod(0));
  CPPUNIT_ASSERT_EQUAL(30, (int) m.mod(1));
  CPPUNIT_ASSERT_EQUAL(6, (int) m.mod(2));
  CPPUNIT_ASSERT_EQUAL(1, (int) m.mod(3));
  CPPUNIT_ASSERT_EQUAL(1080, (int) m.footprint());
  CPPUNIT_ASSERT_EQUAL(1080, (int) m.getstorage()->size());

  m.resize(7, 2, 3, 4);

  CPPUNIT_ASSERT_EQUAL(4, (int) m.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m.offset());
  CPPUNIT_ASSERT_EQUAL(true, m.contiguousp());
  CPPUNIT_ASSERT_EQUAL(7, (int) m.dim(0));
  CPPUNIT_ASSERT_EQUAL(2, (int) m.dim(1));
  CPPUNIT_ASSERT_EQUAL(3, (int) m.dim(2));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.dim(3));
  CPPUNIT_ASSERT_EQUAL(24, (int) m.mod(0));
  CPPUNIT_ASSERT_EQUAL(12, (int) m.mod(1));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.mod(2));
  CPPUNIT_ASSERT_EQUAL(1, (int) m.mod(3));
  CPPUNIT_ASSERT_EQUAL(168, (int) m.footprint());
  CPPUNIT_ASSERT_EQUAL(1080, (int) m.getstorage()->size());
}

// Testing IdxIter and aloop macros
void IdxTest::test_IdxIter() {
  Idx<double> m(3, 4);
  Idx<double> p(4, 3);

  CPPUNIT_ASSERT_EQUAL(2, (int) m.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) m.offset());
  CPPUNIT_ASSERT_EQUAL(true, m.contiguousp());
  CPPUNIT_ASSERT_EQUAL(3, (int) m.dim(0));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.dim(1));
  CPPUNIT_ASSERT_EQUAL(4, (int) m.mod(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) m.mod(1));

  CPPUNIT_ASSERT_EQUAL(2, (int) p.order());
  CPPUNIT_ASSERT_EQUAL(0, (int) p.offset());
  CPPUNIT_ASSERT_EQUAL(true, p.contiguousp());
  CPPUNIT_ASSERT_EQUAL(4, (int) p.dim(0));
  CPPUNIT_ASSERT_EQUAL(3, (int) p.dim(1));
  CPPUNIT_ASSERT_EQUAL(3, (int) p.mod(0));
  CPPUNIT_ASSERT_EQUAL(1, (int) p.mod(1));

  // this is for test purpose: this is
  // a bad way to loop over an Idx.
  // Use aloop macro instead.
  for (intg i0 = 0; i0<3; i0++) {
    for (intg i1 = 0; i1<4; i1++) {
      m.set(10*i0+i1, i0, i1);
      CPPUNIT_ASSERT_EQUAL((double) 10*i0+i1, m.get(i0, i1));
    }
  }

  // Testing IdxIter on m (contiguous)
  double *ptr = m.idx_ptr();
  CPPUNIT_ASSERT_EQUAL(true, m.contiguousp());

#if USING_STL_ITERS == 1
  for( ScalarIter<double> iter(m); iter.notdone(); ++iter) {
    CPPUNIT_ASSERT_EQUAL(*(ptr++), *iter);
  }
#else
  double *idxptr;
  IdxIter<double> idx;
  for (idxptr = idx.init(m); idx.notdone(); idxptr = idx.next() ) {
    CPPUNIT_ASSERT_EQUAL(*(ptr++), *idxptr);
  }
#endif

  // Testing IdxIter on m.narrow(1,2,1) (non-contiguous)
  Idx<double> z = m.narrow(1, 2, 1);
  ptr = z.idx_ptr();
  int jump = 0;
  CPPUNIT_ASSERT_EQUAL(false, z.contiguousp());
  
#if USING_STL_ITERS == 1
  for( ScalarIter<double> ziter(z); ziter.notdone(); ++ziter ) {
    CPPUNIT_ASSERT_EQUAL(*ptr, *ziter);
    ptr++;
    jump++;
    if (jump == 2) {
      ptr += 2;
	jump = 0;
    }
  }
#else
  double *zptr;
  IdxIter<double> ziter;
  for (zptr = ziter.init(z); ziter.notdone(); zptr = ziter.next() ) {
    CPPUNIT_ASSERT_EQUAL(*ptr, *zptr);
    ptr++;
    jump++;
    if (jump == 2) {
      ptr += 2;
	jump = 0;
    }
  }
  delete zptr;
#endif
  
  // Testing idx_aloop1() on qq
  Idx<double> qq(4, 5);
  //{ idx_aloop1(p,qq,double) { *p = 2 * p.i; } }
  int ii = 0;
  {
    idx_aloop1(p, qq, double) {
      *p = 2 * ++ii;
    }
  }
  ii = 0;
  {
    idx_aloop1(p, qq, double) {
      CPPUNIT_ASSERT_EQUAL((double) 2 * ++ii, *p);
    }
  }

  // Testing idx_aloop2() on m and p
  {
    idx_aloop2(lm, m, double, lp, p, double) {
      *lp = *lm;
    }
  }
  ptr = m.idx_ptr();
  {
    idx_aloop1(lp, p, double) {
      CPPUNIT_ASSERT_EQUAL(*(ptr++), *lp);
    }
  }
}

// Testing IdxLooper and bloop/eloop macros
void IdxTest::test_Idx_macros() {
  Idx<double> m(3, 4);
  Idx<double> p(3, 5);

  int offset = 0;
  {
    idx_bloop1(lm, m, double) {
      CPPUNIT_ASSERT_EQUAL(1, (int) lm.order());
      CPPUNIT_ASSERT_EQUAL(offset, (int) lm.offset());
      CPPUNIT_ASSERT_EQUAL(4, (int) lm.dim(0));
      CPPUNIT_ASSERT_EQUAL(1, (int) lm.mod(0));
      CPPUNIT_ASSERT_EQUAL(offset + 4, (int) lm.footprint());
      CPPUNIT_ASSERT_EQUAL(true, lm.contiguousp());
      offset += 4;
    }
  }
  offset = 0;
  int offset2 = 0;
  {
    idx_bloop2(lm, m, double, lp, p, double) {
      CPPUNIT_ASSERT_EQUAL(offset, (int) lm.offset());
      CPPUNIT_ASSERT_EQUAL(4, (int) lm.dim(0));
      CPPUNIT_ASSERT_EQUAL(1, (int) lm.mod(0));
      CPPUNIT_ASSERT_EQUAL(offset + 4, (int) lm.footprint());
      CPPUNIT_ASSERT_EQUAL(true, lm.contiguousp());
      offset += 4;

      CPPUNIT_ASSERT_EQUAL(offset2, (int) lp.offset());
      CPPUNIT_ASSERT_EQUAL(5, (int) lp.dim(0));
      CPPUNIT_ASSERT_EQUAL(1, (int) lp.mod(0));
      CPPUNIT_ASSERT_EQUAL(offset2 + 5, (int) lp.footprint());
      CPPUNIT_ASSERT_EQUAL(true, lp.contiguousp());
      offset2 += 5;
    }
  }

  {
    intg i = 0;
    idx_bloop1(lm, m, double) {
      idx_bloop1(llm, lm, double) {
	llm.set(i++);
      }
    }
  }

  {
    int i = 0;
    idx_bloop1(lm, m, double) {
      idx_bloop1(llm, lm, double) {
	CPPUNIT_ASSERT_EQUAL(i++, (int) llm.get());
      }
    }
  }

  {
    int i = -4;
    int j = 0;
    idx_eloop1(lm, m, double) {
      idx_eloop1(llm, lm, double) {
	i += 4;
	CPPUNIT_ASSERT_EQUAL(i, (int) llm.get());
	if (i >= 8)
	  i = ++j - 4;
      }
    }
  }
}

