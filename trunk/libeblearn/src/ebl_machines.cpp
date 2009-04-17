/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include "ebl_machines.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // nn_machine_cscscf

  // the empty constructor (must call init afterwards)
  nn_machine_cscscf::nn_machine_cscscf()
    : layers_n<state_idx>(true) { // owns modules, responsible for deleting it
  }

  nn_machine_cscscf::nn_machine_cscscf(parameter &prm, intg ini, intg inj,
				       intg ki0, intg kj0, idx<intg> &tbl0, 
				       intg si0, intg sj0,
				       intg ki1, intg kj1, idx<intg> &tbl1, 
				       intg si1, intg sj1,
				       intg ki2, intg kj2, idx<intg> &tbl2,
				       intg outthick)
    : layers_n<state_idx>(true) { // owns modules, responsible for deleting it
    init(prm, ini, inj, ki0, kj0, tbl0, si0, sj0, ki1, kj1, tbl1, 
	 si1, sj1, ki2, kj2, tbl2, outthick);
  }
  
  nn_machine_cscscf::~nn_machine_cscscf() {}

  void nn_machine_cscscf::init(parameter &prm, intg ini, intg inj,
			       intg ki0, intg kj0, idx<intg> &tbl0, intg si0, 
			       intg sj0, intg ki1, intg kj1, idx<intg> &tbl1, 
			       intg si1, intg sj1, intg ki2, intg kj2, 
			       idx<intg> &tbl2, intg outthick) {
    idx<intg> tblmax = tbl0.select(1, 1);
    int tblmax0 = idx_max(tblmax);
    intg thick0 = 1 + tblmax0;
    intg c0_sizi = 1 + ini - ki0;
    intg c0_sizj = 1 + inj - kj0;
    intg s0_sizi = c0_sizi / si0;
    intg s0_sizj = c0_sizj / sj0;
    tblmax = tbl1.select(1, 1);
    int tblmax1 = idx_max(tblmax);
    intg thick1 = 1 + tblmax1;
    intg c1_sizi = 1 + s0_sizi - ki1;
    intg c1_sizj = 1 + s0_sizj - kj1;
    intg s1_sizi = c1_sizi / si1;
    intg s1_sizj = c1_sizj / sj1;
    tblmax = tbl2.select(1, 1);
    int tblmax2 = idx_max(tblmax);
    intg thick2 = 1 + tblmax2;
    intg c2_sizi = 1 + s1_sizi - ki2;
    intg c2_sizj = 1 + s1_sizj - kj2;
    
    // check that table values are consistent
    tblmax = tbl1.select(1, 0);
    if (idx_max(tblmax) != tblmax0) {
      cerr << "error: the maximum number of outputs of table0 (" << tblmax0 + 1;
      cerr << ") is different than the maximum number of inputs in table1 (";
      cerr << idx_max(tblmax) + 1 << ")." << endl;
      ylerror("tables inconsistency in nn_machine_cscscf constructor");
    }
    tblmax = tbl2.select(1, 0);
    if (idx_max(tblmax) != tblmax1) {
      cerr << "error: the maximum number of outputs of table1 (" << tblmax1 + 1;
      cerr << ") is different than the maximum number of inputs in table2 (";
      cerr << idx_max(tblmax) + 1 << ")." << endl;
      ylerror("tables inconsistency in nn_machine_cscscf constructor");
    }

    addModule(new nn_layer_convolution(prm, ki0, kj0, 1, 1, tbl0, thick0),
	      new state_idx(thick0, c0_sizi, c0_sizj));
    addModule(new nn_layer_subsampling(prm, si0, sj0, s0_sizi, s0_sizj, thick0),
	      new state_idx(thick0, s0_sizi, s0_sizj));
    addModule(new nn_layer_convolution(prm, ki1, kj1, 1, 1, tbl1, thick1),
	      new state_idx(thick1, c1_sizi, c1_sizj));
    addModule(new nn_layer_subsampling(prm, si1, sj1, s1_sizi, s1_sizj, thick1),
	      new state_idx(thick1, s1_sizi, s1_sizj));
    addModule(new nn_layer_convolution(prm, ki2, kj2, 1, 1, tbl2, thick2),
	      new state_idx(thick2, c2_sizi, c2_sizj));
    addLastModule(new nn_layer_full(prm, thick2, outthick));
  }

  ////////////////////////////////////////////////////////////////
  // helper function
  
  idx<intg> full_table(intg a, intg b) {
    idx<intg> m(a * b, 2);
    intg p = 0;
    for (intg j = 0; j < b; ++j) {
      for (intg i = 0; i < a; ++i) {
	m.set(i, p, 0);
	m.set(j, p, 1);
	p++;
      }
    }
    return m;
  }

  ////////////////////////////////////////////////////////////////
  // lenet5

  lenet5::lenet5(parameter &prm, intg image_height, intg image_width,
		 intg ki0, intg kj0, intg si0, intg sj0,
		 intg ki1, intg kj1, intg si1, intg sj1,
		 intg hid, intg output_size) {
    table0 = full_table(1, 6);
    table1 = idx<intg>(60, 2);
    table2 = full_table(16, hid);
    intg tbl[60][2] =
      {{0, 0},  {1, 0},  {2, 0},
       {1, 1},  {2, 1},  {3, 1},
       {2, 2},  {3, 2},  {4, 2},
       {3, 3},  {4, 3},  {5, 3},
       {4, 4},  {5, 4},  {0, 4},
       {5, 5},  {0, 5},  {1, 5},

       {0, 6},  {1, 6},  {2, 6},  {3, 6},
       {1, 7},  {2, 7},  {3, 7},  {4, 7},
       {2, 8},  {3, 8},  {4, 8},  {5, 8},
       {3, 9},  {4, 9},  {5, 9},  {0, 9},
       {4, 10}, {5, 10}, {0, 10}, {1, 10},
       {5, 11}, {0, 11}, {1, 11}, {2, 11},

       {0, 12}, {1, 12}, {3, 12}, {4, 12},
       {1, 13}, {2, 13}, {4, 13}, {5, 13},
       {2, 14}, {3, 14}, {5, 14}, {0, 14},

       {0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15}, {5, 15}};
    memcpy(table1.idx_ptr(), tbl, table1.nelements() * sizeof (intg));

    // WARNING: those two numbers must be changed
    // when image-height/image-width change
    // TODO: add assertion test here?
    intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
    intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

    this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
	       ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size);
  }

  ////////////////////////////////////////////////////////////////////////
  // lenet7

  lenet7::lenet7(parameter &prm, intg image_height, intg image_width, 
		 intg output_size) {
    intg ki0 = 5, kj0 = 5;
    intg si0 = 4, sj0 = 4;
    intg ki1 = 6, kj1 = 6;
    intg si1 = 3, sj1 = 3;
    intg hid = 100;

    table0 = full_table(1, 8);
    table1 = idx<intg>(96, 2);
    intg tbl1[96][2] =
      {{0,  0}, {2,  0}, {4,  0}, {5,  0},
       {0,  1}, {2,  1}, {4,  1}, {6,  1},
       {0,  2}, {2,  2}, {4,  2}, {7,  2},
       {0,  3}, {2,  3}, {5,  3}, {6,  3},
       {0,  4}, {2,  4}, {5,  4}, {7,  4},
       {0,  5}, {2,  5}, {6,  5}, {7,  5},
       {1,  6}, {3,  6}, {4,  6}, {5,  6},
       {1,  7}, {3,  7}, {4,  7}, {6,  7},
       {1,  8}, {3,  8}, {4,  8}, {7,  8},
       {1,  9}, {3,  9}, {5,  9}, {6,  9},
       {1, 10}, {3, 10}, {5, 10}, {7, 10},
       {1, 11}, {3, 11}, {6, 11}, {7, 11},
       {1, 12}, {2, 12}, {4, 12}, {5, 12},
       {1, 13}, {2, 13}, {4, 13}, {6, 13},
       {1, 14}, {2, 14}, {4, 14}, {7, 14},
       {1, 15}, {2, 15}, {5, 15}, {6, 15},
       {1, 16}, {2, 16}, {5, 16}, {7, 16},
       {1, 17}, {2, 17}, {6, 17}, {7, 17},
       {0, 18}, {3, 18}, {4, 18}, {5, 18},
       {0, 19}, {3, 19}, {4, 19}, {6, 19},
       {0, 20}, {3, 20}, {4, 20}, {7, 20},
       {0, 21}, {3, 21}, {5, 21}, {6, 21},
       {0, 22}, {3, 22}, {5, 22}, {7, 22},
       {0, 23}, {3, 23}, {6, 23}, {7, 23}};
    memcpy(table1.idx_ptr(), tbl1, table1.nelements() * sizeof (intg));
    table2 = full_table(24, hid);

    // WARNING: those two numbers must be changed
    // when image-height/image-width change
    // TODO: add assertion test here?
    intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
    intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

    this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
	       ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size);
  }

  ////////////////////////////////////////////////////////////////////////
  // lenet7_binocular

  lenet7_binocular::lenet7_binocular(parameter &prm, intg image_height, 
			   intg image_width, intg output_size) {
    intg ki0 = 5, kj0 = 5;
    intg si0 = 4, sj0 = 4;
    intg ki1 = 6, kj1 = 6;
    intg si1 = 3, sj1 = 3;
    intg hid = 100;

    table0 = idx<intg>(12, 2);
    intg tbl0[12][2] =
      {{0, 0},  {0, 1},  {1, 2},
       {1, 3},  {0, 4},  {1, 4},
       {0, 5},  {1, 5},  {0, 6},
       {1, 6},  {0, 7},  {1, 7}};
    memcpy(table0.idx_ptr(), tbl0, table0.nelements() * sizeof (intg));
    table1 = idx<intg>(96, 2);
    intg tbl1[96][2] =
      {{0,  0}, {2,  0}, {4,  0}, {5,  0},
       {0,  1}, {2,  1}, {4,  1}, {6,  1},
       {0,  2}, {2,  2}, {4,  2}, {7,  2},
       {0,  3}, {2,  3}, {5,  3}, {6,  3},
       {0,  4}, {2,  4}, {5,  4}, {7,  4},
       {0,  5}, {2,  5}, {6,  5}, {7,  5},
       {1,  6}, {3,  6}, {4,  6}, {5,  6},
       {1,  7}, {3,  7}, {4,  7}, {6,  7},
       {1,  8}, {3,  8}, {4,  8}, {7,  8},
       {1,  9}, {3,  9}, {5,  9}, {6,  9},
       {1, 10}, {3, 10}, {5, 10}, {7, 10},
       {1, 11}, {3, 11}, {6, 11}, {7, 11},
       {1, 12}, {2, 12}, {4, 12}, {5, 12},
       {1, 13}, {2, 13}, {4, 13}, {6, 13},
       {1, 14}, {2, 14}, {4, 14}, {7, 14},
       {1, 15}, {2, 15}, {5, 15}, {6, 15},
       {1, 16}, {2, 16}, {5, 16}, {7, 16},
       {1, 17}, {2, 17}, {6, 17}, {7, 17},
       {0, 18}, {3, 18}, {4, 18}, {5, 18},
       {0, 19}, {3, 19}, {4, 19}, {6, 19},
       {0, 20}, {3, 20}, {4, 20}, {7, 20},
       {0, 21}, {3, 21}, {5, 21}, {6, 21},
       {0, 22}, {3, 22}, {5, 22}, {7, 22},
       {0, 23}, {3, 23}, {6, 23}, {7, 23}};
    memcpy(table1.idx_ptr(), tbl1, table1.nelements() * sizeof (intg));
    table2 = full_table(24, hid);

    // WARNING: those two numbers must be changed
    // when image-height/image-width change
    // TODO: add assertion test here?
    intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
    intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

    this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
	       ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size);
  }

  ////////////////////////////////////////////////////////////////
  // supervised euclidean machine

  supervised_euclidean_machine::
  supervised_euclidean_machine(module_1_1<state_idx,state_idx> &m,
			       idx<double> &t, idxdim &dims)
    : fc_ebm2<state_idx,int,state_idx>(m, fout, (ebm_2<state_idx,int>&)fcost), 
      fcost(t), fout(dims) {
  }

  supervised_euclidean_machine::~supervised_euclidean_machine() {
  }

} // end namespace ebl
