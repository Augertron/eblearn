#include "ebl_preprocessing_test.h"

#include <ostream>

#define __SHOW__

#ifdef __GUI__
#include "libidxgui.h"
#endif

extern string *gl_data_dir;
extern string *gl_data_errmsg;

using namespace std;
using namespace ebl;

void ebl_preprocessing_test::setUp() {
}

void ebl_preprocessing_test::tearDown() {
}

void ebl_preprocessing_test::test_rgb_to_yp() {
  ostringstream path, s;
  path << *gl_data_dir << "/barn.png";
  idx<float> im = load_image<float>(path.str());
  rgb_to_yp_module<float> pp(9);
  rgb_to_ypuv_module<float> ppuv(9);
  im = im.shift_dim(2, 0);
  state_idx<float> in(im.get_idxdim()), out(1, 1, 1), outuv(1, 1, 1);
  idx_copy(im, in.x);
  pp.fprop(in, out);
  idx<float> im2 = out.x.shift_dim(0, 2);
  ppuv.fprop(in, outuv);
  idx<float> im3 = outuv.x.shift_dim(0, 2);
  im = im.shift_dim(0, 2);
#ifdef __GUI__  
#ifdef __SHOW__
  uint h = 0, w = 0;
  new_window("ebl_preprocessing_test");
  s.str(""); s << "RGB " << im;
  draw_matrix(im, s.str().c_str(), h, w);
  h += im.dim(0);
  s.str(""); s << "Yp " << im2;
  draw_matrix(im2, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im2.dim(0);  
  s.str(""); s << "YpUV " << im3;
  draw_matrix(im3, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im3.dim(0);  
#endif
#endif
  
  sleep(10);
}
