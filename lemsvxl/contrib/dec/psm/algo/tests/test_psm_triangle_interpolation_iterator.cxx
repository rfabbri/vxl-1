#include <testlib/testlib_test.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include "../psm_triangle_interpolation_iterator_aa.h"

static void test_psm_triangle_interpolation_iterator()
{
  START("psm_triangle_interpolation_iterator test");

  double verts_x1[] = {1.0, 4.0, 1.0};
  double verts_y1[] = {4.0, 4.0, 1.0};
  float values [] = {1.0, 3.0, 3.0};
  vil_image_view<float> img(5,5,1);
  img.fill(0.0f);

  psm_triangle_interpolation_iterator_aa<float> tri_it(verts_x1, verts_y1, values, 0, 1, 2);
  tri_it.reset();
  while (tri_it.next()) {
    int scany = tri_it.scany();
    int startx = tri_it.startx();
    int endx = tri_it.endx();
    for (int x=startx; x<endx; ++x) {
      float pixval = tri_it.pix_coverage(x);
      float value = tri_it.value_at(x);
      img(x,scany) += value;
    }
  }
  vil_save(img,"c:/research/psm/output/test_it1.tiff");

  double verts_x2[] = {4.0, 4.0, 1.0};
  double verts_y2[] = {4.0, 1.0, 1.0};
  float values2[] = {3.0, 1.0, 3.0};
  tri_it = psm_triangle_interpolation_iterator_aa<float>(verts_x2, verts_y2, values2);
  tri_it.reset();
  while (tri_it.next()) {
    int scany = tri_it.scany();
    int startx = tri_it.startx();
    int endx = tri_it.endx();
    for (int x=startx; x<endx; ++x) {
      float pixval = tri_it.pix_coverage(x);
      float value = tri_it.value_at(x);
      img(x,scany) += value;
    }
  }
  vil_save(img,"c:/research/psm/output/test_it2.tiff");


  return;
}

TESTMAIN( test_psm_triangle_interpolation_iterator );

